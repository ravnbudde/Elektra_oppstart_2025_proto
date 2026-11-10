import hmac
import math
import os
import secrets
import sqlite3
from pathlib import Path

from flask import Flask, jsonify, redirect, render_template, request, session, url_for
import paho.mqtt.client as mqtt


COLUMNS = ("del1.1", "del1.2", "del2", "del3", "bonus")
TIME_COLUMNS = ("del1.1", "del1.2", "del3")
MQTT_TOPIC = "zumo_car/+/score/#"
URL_PREFIX = "/oppstartsuka/scoreboard"


def create_app(config=None):
    app = Flask(__name__, static_url_path=f"{URL_PREFIX}/static")
    app.config.from_mapping(
        SECRET_KEY=os.environ.get("SCORE_SECRET_KEY", secrets.token_hex(32)),
        SCORE_PASSWORD=os.environ.get("SCORE_PASSWORD", "ravn"),
        DATABASE=os.environ.get(
            "SCORE_DB_PATH", str(Path(app.instance_path) / "scoreboard.db")
        ),
        MQTT_ENABLED=os.environ.get("MQTT_ENABLED", "true").lower()
        in {"1", "true", "yes", "on"},
        MQTT_HOST=os.environ.get("MQTT_HOST", "127.0.0.1"),
        MQTT_PORT=int(os.environ.get("MQTT_PORT", "1883")),
        MQTT_USERNAME=os.environ.get("MQTT_USERNAME", "BIAISbroker"),
        MQTT_PASSWORD=os.environ.get("MQTT_PASSWORD", ""),
    )
    if config:
        app.config.update(config)

    Path(app.config["DATABASE"]).parent.mkdir(parents=True, exist_ok=True)
    init_database(app.config["DATABASE"])

    @app.get("/")
    def root():
        return redirect(url_for("scoreboard"))

    @app.get(URL_PREFIX)
    @app.get(f"{URL_PREFIX}/")
    def scoreboard():
        return render_template("scoreboard.html")

    @app.get(f"{URL_PREFIX}/admin")
    @app.get(f"{URL_PREFIX}/admin/")
    def admin():
        session.pop("authenticated", None)
        if "csrf_token" not in session:
            session["csrf_token"] = secrets.token_urlsafe(32)
        return render_template("admin.html", csrf_token=session["csrf_token"])

    @app.get(f"{URL_PREFIX}/api/scores")
    def scores():
        rows = read_results(app.config["DATABASE"])
        return jsonify({"scores": calculate_scores(rows)})

    @app.get(f"{URL_PREFIX}/api/session")
    def session_status():
        return jsonify({"authenticated": bool(session.get("authenticated"))})

    @app.post(f"{URL_PREFIX}/api/login")
    def login():
        require_csrf()
        supplied = str((request.get_json(silent=True) or {}).get("password", ""))
        if not hmac.compare_digest(supplied, app.config["SCORE_PASSWORD"]):
            return jsonify({"error": "Feil passord, prøv igjen."}), 401
        session["authenticated"] = True
        return jsonify({"authenticated": True})

    @app.post(f"{URL_PREFIX}/api/logout")
    def logout():
        require_csrf()
        session.pop("authenticated", None)
        return jsonify({"authenticated": False})

    @app.get(f"{URL_PREFIX}/api/groups/<path:group>")
    def get_group(group):
        require_authentication()
        result = read_group(app.config["DATABASE"], validate_group(group))
        return jsonify({"group": group, "values": result or empty_values()})

    @app.put(f"{URL_PREFIX}/api/groups/<path:group>")
    def put_group(group):
        require_authentication()
        require_csrf()
        group = validate_group(group)
        payload = request.get_json(silent=True) or {}
        values = {column: parse_optional_number(payload.get(column)) for column in COLUMNS}
        replace_group(app.config["DATABASE"], group, values)
        return jsonify({"group": group, "values": values})

    @app.errorhandler(ValueError)
    def bad_request(error):
        return jsonify({"error": str(error)}), 400

    @app.errorhandler(401)
    def unauthorized(_error):
        return jsonify({"error": "Du må logge inn for å endre resultater."}), 401

    if app.config["MQTT_ENABLED"] and not app.config.get("TESTING"):
        app.extensions["mqtt_client"] = start_mqtt(app)

    return app


def connect(database):
    connection = sqlite3.connect(database, timeout=10)
    connection.row_factory = sqlite3.Row
    connection.execute("PRAGMA journal_mode=WAL")
    return connection


def init_database(database):
    with connect(database) as connection:
        connection.execute(
            """
            CREATE TABLE IF NOT EXISTS results (
                group_id TEXT PRIMARY KEY,
                del1_1 REAL,
                del1_2 REAL,
                del2 REAL,
                del3 REAL,
                bonus REAL,
                updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
            )
            """
        )


def db_column(column):
    return column.replace(".", "_")


def empty_values():
    return {column: None for column in COLUMNS}


def read_results(database):
    with connect(database) as connection:
        rows = connection.execute("SELECT * FROM results ORDER BY rowid").fetchall()
    return [
        {
            "gruppeNr": row["group_id"],
            **{column: row[db_column(column)] for column in COLUMNS},
        }
        for row in rows
    ]


def read_group(database, group):
    with connect(database) as connection:
        row = connection.execute(
            "SELECT * FROM results WHERE group_id = ?", (group,)
        ).fetchone()
    if row is None:
        return None
    return {column: row[db_column(column)] for column in COLUMNS}


def replace_group(database, group, values):
    columns = [db_column(column) for column in COLUMNS]
    with connect(database) as connection:
        connection.execute(
            f"""
            INSERT INTO results (group_id, {', '.join(columns)})
            VALUES (?, {', '.join('?' for _ in columns)})
            ON CONFLICT(group_id) DO UPDATE SET
                {', '.join(f'{column} = excluded.{column}' for column in columns)},
                updated_at = CURRENT_TIMESTAMP
            """,
            (group, *(values[column] for column in COLUMNS)),
        )


def update_best_result(database, group, column, value):
    column_name = db_column(column)
    comparison = "<" if column in TIME_COLUMNS else ">"
    with connect(database) as connection:
        cursor = connection.execute(
            f"""
            INSERT INTO results (group_id, {column_name}) VALUES (?, ?)
            ON CONFLICT(group_id) DO UPDATE SET
                {column_name} = excluded.{column_name},
                updated_at = CURRENT_TIMESTAMP
            WHERE results.{column_name} IS NULL
                OR excluded.{column_name} {comparison} results.{column_name}
            """,
            (group, value),
        )
    return cursor.rowcount > 0


def normalize(rows, column, higher_is_better):
    present = [row[column] for row in rows if row[column] is not None]
    if not present:
        return [0.0] * len(rows)
    low, high = min(present), max(present)
    if low == high:
        return [50.0 if row[column] is not None else 0.0 for row in rows]
    return [
        0.0
        if row[column] is None
        else (
            (row[column] - low) / (high - low) * 100
            if higher_is_better
            else (high - row[column]) / (high - low) * 100
        )
        for row in rows
    ]


def calculate_scores(rows):
    normalized = {
        "del1.1": normalize(rows, "del1.1", False),
        "del1.2": normalize(rows, "del1.2", False),
        "del2": normalize(rows, "del2", True),
        "del3": normalize(rows, "del3", False),
    }
    scored = []
    for index, row in enumerate(rows):
        total = (
            0.5 * normalized["del1.1"][index]
            + 0.5 * normalized["del1.2"][index]
            + normalized["del2"][index]
            + normalized["del3"][index]
            + (row["bonus"] or 0)
        )
        scored.append(
            {
                "group": row["gruppeNr"],
                "total": total,
                "tasks": {
                    column: {
                        "raw": row[column],
                        "points": normalized[column][index],
                    }
                    for column in normalized
                },
            }
        )
    return sorted(scored, key=lambda row: row["total"], reverse=True)


def validate_group(group):
    group = group.strip()
    if not group or len(group) > 64 or any(char in group for char in "/\\"):
        raise ValueError("Gruppenummeret må være mellom 1 og 64 tegn.")
    return group


def parse_optional_number(value):
    if value is None or (isinstance(value, str) and value.strip().lower() in {"", "nan"}):
        return None
    try:
        number = float(value)
    except (TypeError, ValueError) as error:
        raise ValueError("Alle verdier må være gyldige tall eller ikke gjennomført.") from error
    if not math.isfinite(number):
        raise ValueError("Alle verdier må være endelige tall.")
    return number


def require_authentication():
    if not session.get("authenticated"):
        from flask import abort

        abort(401)


def require_csrf():
    supplied = request.headers.get("X-CSRF-Token", "")
    expected = session.get("csrf_token", "")
    if not expected or not hmac.compare_digest(supplied, expected):
        raise ValueError("Ugyldig forespørsel. Last siden på nytt og prøv igjen.")


def parse_mqtt_message(topic, payload):
    parts = topic.split("/")
    if len(parts) != 5 or parts[0] != "zumo_car" or parts[2] != "score":
        raise ValueError(f"Ugyldig score-topic: {topic}")
    column, group = parts[-2], validate_group(parts[-1])
    if column not in COLUMNS:
        raise ValueError(f"Ugyldig oppgave i score-topic: {column}")
    text = payload.decode("utf-8").strip().split(",")[-1]
    value = parse_optional_number(text)
    if value is None:
        raise ValueError("MQTT-score kan ikke være tom.")
    if column in TIME_COLUMNS and value <= 0:
        raise ValueError("Tid må være større enn null.")
    return group, column, value


def start_mqtt(app):
    try:
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
    except AttributeError:
        client = mqtt.Client()
    if app.config["MQTT_USERNAME"]:
        client.username_pw_set(
            app.config["MQTT_USERNAME"], app.config["MQTT_PASSWORD"]
        )

    def on_connect(mqtt_client, _userdata, _flags, result_code):
        if result_code == 0:
            app.logger.info("Tilkoblet MQTT-broker på %s", app.config["MQTT_HOST"])
            mqtt_client.subscribe(MQTT_TOPIC)
        else:
            app.logger.error("MQTT-tilkobling feilet med kode %s", result_code)

    def on_message(_client, _userdata, message):
        try:
            group, column, value = parse_mqtt_message(message.topic, message.payload)
            update_best_result(app.config["DATABASE"], group, column, value)
        except (UnicodeDecodeError, ValueError, sqlite3.Error) as error:
            app.logger.warning("Ignorerte MQTT-melding: %s", error)

    client.on_connect = on_connect
    client.on_message = on_message
    client.connect_async(app.config["MQTT_HOST"], app.config["MQTT_PORT"], 60)
    client.loop_start()
    return client

