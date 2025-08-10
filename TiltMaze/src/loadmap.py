import os, csv

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
MAPS_DIR   = os.path.join(SCRIPT_DIR, "maps")

def get_csv_files():
    return [
        f for f in os.listdir(MAPS_DIR)
        if os.path.isfile(os.path.join(MAPS_DIR, f)) and f.lower().endswith(".csv")
    ]

def load_map(filename):
    path = os.path.join(MAPS_DIR, filename)
    grid = []
    with open(path, newline="") as f:
        reader = csv.reader(f)
        for row in reader:
            if not row or row[0].startswith("#"):
                continue
            grid.append([int(cell) for cell in row])
    return grid
