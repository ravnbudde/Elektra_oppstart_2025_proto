#!/usr/bin/env python3
import argparse
import csv
import os
import sqlite3
from pathlib import Path

from app import COLUMNS, connect, db_column, init_database, parse_optional_number, validate_group

REQUIRED_COLUMNS = ("gruppeNr", *COLUMNS)
DEFAULT_DATABASE = "/var/lib/elektra-scoreboard/scoreboard.db"


def load_csv(csv_path):
    rows = []
    seen_groups = set()
    with Path(csv_path).open("r", encoding="utf-8-sig", newline="") as csv_file:
        reader = csv.DictReader(csv_file)
        missing = [column for column in REQUIRED_COLUMNS if column not in (reader.fieldnames or [])]
        if missing:
            raise ValueError(f"CSV-filen mangler kolonner: {', '.join(missing)}")

        for line_number, row in enumerate(reader, start=2):
            try:
                group = validate_group(row["gruppeNr"] or "")
                values = {
                    column: parse_optional_number(row[column]) for column in COLUMNS
                }
            except ValueError as error:
                raise ValueError(f"Linje {line_number}: {error}") from error
            if group in seen_groups:
                raise ValueError(f"Linje {line_number}: gruppe {group} finnes flere ganger.")
            seen_groups.add(group)
            rows.append((group, values))
    if not rows:
        raise ValueError("CSV-filen inneholder ingen grupper.")
    return rows


def import_results(database, rows, replace=False):
    Path(database).parent.mkdir(parents=True, exist_ok=True)
    init_database(database)
    columns = [db_column(column) for column in COLUMNS]
    statement = f"""
        INSERT INTO results (group_id, {', '.join(columns)})
        VALUES (?, {', '.join('?' for _ in columns)})
        ON CONFLICT(group_id) DO UPDATE SET
            {', '.join(f'{column} = excluded.{column}' for column in columns)},
            updated_at = CURRENT_TIMESTAMP
    """
    with connect(database) as connection:
        if replace:
            connection.execute("DELETE FROM results")
        connection.executemany(
            statement,
            [
                (group, *(values[column] for column in COLUMNS))
                for group, values in rows
            ],
        )


def main():
    parser = argparse.ArgumentParser(
        description="Importer gammel resultater.csv til hosted scoreboard."
    )
    parser.add_argument("csv_file", help="Sti til gammel resultater.csv")
    parser.add_argument(
        "--database",
        default=os.environ.get("SCORE_DB_PATH", DEFAULT_DATABASE),
        help="Sti til scoreboard.db (standard: %(default)s)",
    )
    parser.add_argument(
        "--replace",
        action="store_true",
        help="Slett eksisterende grupper før import. Uten flagget slås dataene sammen.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Valider og vis hva som ville blitt importert uten å endre databasen.",
    )
    args = parser.parse_args()

    try:
        rows = load_csv(args.csv_file)
        mode = "erstatte databasen" if args.replace else "slå sammen med databasen"
        print(f"Validerte {len(rows)} grupper fra {args.csv_file}.")
        print(f"Importmodus: {mode}.")
        print(f"Database: {args.database}")
        if args.dry_run:
            print("Testkjøring fullført. Databasen ble ikke endret.")
            return
        import_results(args.database, rows, replace=args.replace)
        print(f"Importerte {len(rows)} grupper.")
    except (OSError, sqlite3.Error, ValueError) as error:
        parser.exit(1, f"Import feilet: {error}\n")


if __name__ == "__main__":
    main()
