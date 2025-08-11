import pandas as pd
import time
import os
import threading
import tkinter as tk
from tkinter import ttk
from PIL import Image, ImageTk

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
GOLD_MEDAL = os.path.join(os.path.dirname(os.path.abspath(__file__)), "pic/gold.png")
SILVER_MEDAL = os.path.join(os.path.dirname(os.path.abspath(__file__)), "pic/silver.png")
BRONZE_MEDAL = os.path.join(os.path.dirname(os.path.abspath(__file__)), "pic/bronze.png")
POENG_CSV = os.path.join(SCRIPT_DIR, "resultater_omregnet.csv")

# Hvor ofte leaderboard skal oppdateres (sekunder)
OPPDATERING_INTERVAL_SEC = 1

class LeaderboardApp:
    def __init__(self, root, csv_fil, lock):
        self.root = root
        self.csv_fil = csv_fil
        self.lock = lock
        self.root.title("Leaderboard")

        self.tree = ttk.Treeview(root)
        self.tree["columns"] = ("Sum poeng","Oppgave 1.1", "Oppgave 1.2", "Oppgave 2", "Oppgave 3")
        self.tree.heading("#0", text="Gruppe")
        self.tree.heading("Sum poeng", text="Sum poeng")
        self.tree.heading("Oppgave 1.1", text="Oppgave 1.1")
        self.tree.heading("Oppgave 1.2", text="Oppgave 1.2")
        self.tree.heading("Oppgave 2", text="Oppgave 2")
        self.tree.heading("Oppgave 3", text="Oppgave 3")
        self.tree.column("Sum poeng", anchor="center")

        style = ttk.Style()
        style.configure("Treeview", background="#f0f0f0", foreground="black", rowheight=25)
        style.map('Treeview', background=[('selected', '#347083')])

        self.tree.pack(fill="both", expand=True)

        # Last inn ikonbildet (f.eks. stjerne) for første kolonne
        # Endre 'star.png' til ønsket ikonfil, og sjekk at filen finnes
        if os.path.exists(GOLD_MEDAL):
            gold_img = Image.open(GOLD_MEDAL)
            gold_img = gold_img.resize((16, 16), Image.Resampling.LANCZOS)
            self.gold_img = ImageTk.PhotoImage(gold_img)
        else:
            self.gold_img = None
        if os.path.exists(SILVER_MEDAL):
            silver_img = Image.open(SILVER_MEDAL)
            silver_img = silver_img.resize((16, 16), Image.Resampling.LANCZOS)
            self.silver_img = ImageTk.PhotoImage(silver_img)
        else:
            self.silver_img = None
        if os.path.exists(BRONZE_MEDAL):
            bronze_img = Image.open(BRONZE_MEDAL)
            bronze_img = bronze_img.resize((16, 16), Image.Resampling.LANCZOS)
            self.bronze_img = ImageTk.PhotoImage(bronze_img)
        else:
            self.bronze_img = None

        # Må beholde referanser til bilder for å unngå garbage collection
        self.tree.image_refs = []
        if self.gold_img:
            self.tree.image_refs.append(self.gold_img)
        if self.silver_img:
            self.tree.image_refs.append(self.silver_img)
        if self.bronze_img:
            self.tree.image_refs.append(self.bronze_img)

        self.oppdater_leaderboard()  # initial oppdatering
        self.schedule_oppdatering()  # start repeterende oppdatering


    def oppdater_leaderboard(self):
        with self.lock:
            for rad in self.tree.get_children():
                self.tree.delete(rad)

            if not os.path.exists(self.csv_fil):
                return

            try:
                df = pd.read_csv(self.csv_fil)
            except Exception as e:
                print(f"Feil ved lesing av {self.csv_fil}: {e}")
                return

            df = df.sort_values(by="sum_poeng", ascending=False)

            for i, (_, row) in enumerate(df.iterrows()):
                try:
                    def format_med_prosent(poeng_col, prosent_col):
                        poeng = float(row.get(poeng_col, 0))
                        prosent = float(row.get(prosent_col, 0)) 
                        return f"{poeng:.2f} ({prosent:.0f}%)"

                    values = (
                        f"{float(row['sum_poeng']):.2f}",
                        format_med_prosent('del1.1', 'p_del1.1'),
                        format_med_prosent('del1.2', 'p_del1.2'),
                        format_med_prosent('del2', 'p_del2'),
                        format_med_prosent('del3', 'p_del3'),
                    )
                except (ValueError, TypeError):
                    values = (
                        f"{row.get('sum_poeng', 0):.2f}",
                        f"{row.get('del1.1', 0):.2f}",
                        f"{row.get('del1.2', 0):.2f}",
                        f"{row.get('del2', 0):.2f}",
                        f"{row.get('del3', 0):.2f}",
                    )

                if i == 0 and self.gold_img is not None:
                    self.tree.insert("", "end", text=str(row["gruppeNr"]), values=values, image=self.gold_img)
                elif i == 1 and self.silver_img is not None:
                    self.tree.insert("", "end", text=str(row["gruppeNr"]), values=values, image=self.silver_img)
                elif i == 2 and self.bronze_img is not None:
                    self.tree.insert("", "end", text=str(row["gruppeNr"]), values=values, image=self.bronze_img)
                else:
                    self.tree.insert("", "end", text=str(row["gruppeNr"]), values=values)


    def schedule_oppdatering(self):
        # Kjør oppdatering og planlegg neste kall
        self.oppdater_leaderboard()
        self.root.after(OPPDATERING_INTERVAL_SEC * 1000, self.schedule_oppdatering)


def start_leaderboard(root, lock):
    leaderboard_win = tk.Toplevel(root)
    leaderboard_win.title("Leaderboard")
    leaderboard_win.geometry("400x300")
    app = LeaderboardApp(leaderboard_win, POENG_CSV, lock)
    return app



def leaderboard_console_updater(lock):
    """
    Leser leaderboard fra csv-fil og printer den til terminalen hvert OPPDATERING_INTERVAL_SEC sekund.
    Beskytter filtilgangen med lock.
    """
    root = tk.Tk()
    app = LeaderboardApp(root, POENG_CSV, lock)
    root.geometry("400x300")
    root.mainloop()

if __name__ == "__main__":
    lock = threading.Lock()

    leaderboard_console_updater(lock)

    
