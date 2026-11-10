import tempfile
import unittest
from pathlib import Path

from app import read_results
from import_csv import import_results, load_csv


class CsvImportTest(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.root = Path(self.temp_dir.name)
        self.csv_file = self.root / "resultater.csv"
        self.database = str(self.root / "scoreboard.db")

    def tearDown(self):
        self.temp_dir.cleanup()

    def test_imports_raw_results_and_empty_values(self):
        self.csv_file.write_text(
            "gruppeNr,del1.1,del1.2,del2,del3,bonus\n"
            "1,3.5,26.8,85,46.2,0\n"
            "2,2.8,,92,nan,0.5\n",
            encoding="utf-8",
        )
        rows = load_csv(self.csv_file)
        import_results(self.database, rows, replace=True)
        imported = read_results(self.database)
        self.assertEqual(len(imported), 2)
        self.assertEqual(imported[0]["del1.1"], 3.5)
        self.assertEqual(imported[0]["del2"], 85.0)
        self.assertIsNone(imported[1]["del1.2"])
        self.assertIsNone(imported[1]["del3"])

    def test_rejects_duplicate_groups_before_import(self):
        self.csv_file.write_text(
            "gruppeNr,del1.1,del1.2,del2,del3,bonus\n"
            "1,1,2,3,4,5\n"
            "1,2,3,4,5,6\n",
            encoding="utf-8",
        )
        with self.assertRaisesRegex(ValueError, "finnes flere ganger"):
            load_csv(self.csv_file)


if __name__ == "__main__":
    unittest.main()
