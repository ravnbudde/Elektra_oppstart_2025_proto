import tempfile
import unittest
from pathlib import Path

from app import calculate_scores, create_app, parse_mqtt_message, read_group, update_best_result


class ScoreboardTest(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.database = str(Path(self.temp_dir.name) / "test.db")
        self.app = create_app(
            {
                "TESTING": True,
                "MQTT_ENABLED": False,
                "DATABASE": self.database,
                "SECRET_KEY": "test-secret",
                "SCORE_PASSWORD": "ravn",
            }
        )
        self.client = self.app.test_client()
        self.client.get("/oppstartsuka/scoreboard/admin")
        with self.client.session_transaction() as session:
            self.csrf = session["csrf_token"]

    def tearDown(self):
        self.temp_dir.cleanup()

    def test_manual_score_flow_requires_login_and_updates_leaderboard(self):
        headers = {"X-CSRF-Token": self.csrf}
        response = self.client.put("/oppstartsuka/scoreboard/api/groups/1", json={})
        self.assertEqual(response.status_code, 401)
        self.assertEqual(
            self.client.post(
                "/oppstartsuka/scoreboard/api/login", json={"password": "ravn"}, headers=headers
            ).status_code,
            200,
        )
        response = self.client.put(
            "/oppstartsuka/scoreboard/api/groups/1",
            json={"del1.1": 10, "del1.2": 20, "del2": 30, "del3": None, "bonus": 5},
            headers=headers,
        )
        self.assertEqual(response.status_code, 200)
        scoreboard = self.client.get("/oppstartsuka/scoreboard/api/scores").get_json()["scores"]
        self.assertEqual(scoreboard[0]["group"], "1")
        self.assertEqual(scoreboard[0]["total"], 105.0)
        self.assertEqual(
            self.client.post(
                "/oppstartsuka/scoreboard/api/logout", headers=headers
            ).status_code,
            200,
        )
        self.assertEqual(
            self.client.get("/oppstartsuka/scoreboard/api/groups/1").status_code,
            401,
        )

    def test_opening_admin_always_requires_a_new_password(self):
        headers = {"X-CSRF-Token": self.csrf}
        self.client.post(
            "/oppstartsuka/scoreboard/api/login",
            json={"password": "ravn"},
            headers=headers,
        )
        self.assertEqual(
            self.client.get("/oppstartsuka/scoreboard/api/groups/1").status_code,
            200,
        )
        self.client.get("/oppstartsuka/scoreboard/admin")
        self.assertEqual(
            self.client.get("/oppstartsuka/scoreboard/api/groups/1").status_code,
            401,
        )


    def test_score_normalization_matches_existing_behavior(self):
        rows = [
            {"gruppeNr": "A", "del1.1": 10, "del1.2": None, "del2": 20, "del3": 5, "bonus": 0},
            {"gruppeNr": "B", "del1.1": 20, "del1.2": None, "del2": 40, "del3": 10, "bonus": 0},
        ]
        scores = calculate_scores(rows)
        self.assertEqual(scores[0]["group"], "A")
        self.assertEqual(scores[0]["total"], 150.0)
        self.assertEqual(scores[1]["total"], 100.0)

    def test_task_one_halves_are_weighted_at_50_points_each(self):
        rows = [
            {"gruppeNr": "best", "del1.1": 10, "del1.2": 10, "del2": 100, "del3": 10, "bonus": 0},
            {"gruppeNr": "last", "del1.1": 20, "del1.2": 20, "del2": 0, "del3": 20, "bonus": 0},
        ]
        scores = calculate_scores(rows)
        best = scores[0]
        self.assertEqual(best["tasks"]["del1.1"]["points"], 100.0)
        self.assertEqual(best["tasks"]["del1.2"]["points"], 100.0)
        self.assertEqual(best["total"], 300.0)

    def test_mqtt_keeps_best_time_and_highest_direct_score(self):
        group, column, value = parse_mqtt_message(
            "zumo_car/car1/score/del1.1/7", b"192.168.1.4,12.5"
        )
        self.assertEqual((group, column, value), ("7", "del1.1", 12.5))
        update_best_result(self.database, group, column, value)
        update_best_result(self.database, group, column, 15)
        update_best_result(self.database, group, "del2", 20)
        update_best_result(self.database, group, "del2", 10)
        result = read_group(self.database, group)
        self.assertEqual(result["del1.1"], 12.5)
        self.assertEqual(result["del2"], 20)


if __name__ == "__main__":
    unittest.main()
