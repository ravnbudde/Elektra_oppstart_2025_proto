import tkinter as tk
import pandas as pd
import numpy as np
from PIL import Image, ImageTk
import threading
import os

# Passord
PASSORD = "ravn"

# Finn stien til scriptet og CSV-fila
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CSV_FILE = os.path.join(SCRIPT_DIR, "resultater.csv")
BILDE_FIL = os.path.join(SCRIPT_DIR, "pic/car.png")  # Sett inn ditt bilde her


# Nye kolonner
KOLONNER = ["gruppeNr", "del1.1", "del1.2", "del2.1", "del2.2", "del3", "bonus"]


# Opprett tom CSV hvis den ikke finnes
#NB: sjekker ikke at formatet er riktig, tror ikke programmet kjører riktig i såfall
if not os.path.exists(CSV_FILE):
    df = pd.DataFrame(columns=KOLONNER)
    df.to_csv(CSV_FILE, index=False)

class GruppeApp:
    PASSORD = PASSORD
    def __init__(self, root, csvLock):
        self.root = root
        self.lock = csvLock
        self.root.title("Gruppe Input")
        self.gruppenummer = None

        if os.path.exists(BILDE_FIL):
            img = Image.open(BILDE_FIL)
            img = img.resize((300, 400))  # Samme størrelse som vinduet
            self.bg_image = ImageTk.PhotoImage(img)
            bg_label = tk.Label(self.root, image=self.bg_image)
            bg_label.place(x=0, y=85, relwidth=1, relheight=1)

        self.build_password_frame()
        self.build_gui()

        # Start med passord-framet synlig, resten skjult
        self.first_frame.pack_forget()
        self.second_frame.pack_forget()
        self.password_frame.pack(padx=20, pady=10)

    def build_password_frame(self):
        self.password_frame = tk.Frame(self.root)

        tk.Label(self.password_frame, text="Skriv inn passord:").pack()
        self.pass_entry = tk.Entry(self.password_frame, show="*")
        self.pass_entry.pack()

        self.pass_status = tk.Label(self.password_frame, text="", fg="red")
        self.pass_status.pack()

        pass_btn = tk.Button(self.password_frame, text="OK", command=self.sjekk_passord)
        pass_btn.pack(pady=10)

    def sjekk_passord(self):
        if self.pass_entry.get() == self.PASSORD:
            self.pass_status.config(text="")
            self.pass_entry.delete(0, tk.END)  # Tøm feltet umiddelbart
            self.password_frame.pack_forget()
            self.first_frame.pack(padx=20, pady=20)
        else:
            self.pass_status.config(text="Feil passord, prøv igjen.")

    def build_gui(self):
        # Første side
        self.first_frame = tk.Frame(self.root)
        tk.Label(self.first_frame, text="Skriv inn gruppenummer:").pack()
        self.gruppe_entry = tk.Entry(self.first_frame)
        self.gruppe_entry.pack()

        next_btn = tk.Button(self.first_frame, text="Neste", command=self.go_to_second_frame)
        next_btn.pack(pady=10)

        # Andre side
        self.second_frame = tk.Frame(self.root)
        self.verdi_label = tk.Label(self.second_frame, text="")
        self.verdi_label.pack()

        self.entries = []
        self.nig_vars = []  # BooleanVar for "ikke gjennomført"

        for navn in KOLONNER[1:]:
            frame = tk.Frame(self.second_frame)
            frame.pack(anchor="w", pady=2)

            # Sett label med fast bredde (f.eks 8-12 tegn bredde), og høyrejuster teksten
            lbl = tk.Label(frame, text=f"{navn}:", width=10, anchor="e")  
            lbl.pack(side="left")

            entry = tk.Entry(frame, width=10)
            entry.pack(side="left", padx=(5,10))
            self.entries.append(entry)

            var = tk.BooleanVar(value=False)
            self.nig_vars.append(var)
            chk = tk.Checkbutton(frame, text="Ikke gjennomført", variable=var,
                                command=lambda e=entry, v=var: self.toggle_entry(e, v))
            chk.pack(side="left")

        self.status_label = tk.Label(self.second_frame, text="", fg="green")
        self.status_label.pack(pady=(10, 0))

        btn_frame = tk.Frame(self.second_frame)
        btn_frame.pack(pady=10)

        self.back_btn = tk.Button(btn_frame, text="Tilbake", command=self.go_back_to_first_frame)
        self.back_btn.grid(row=0, column=0, padx=5)

        self.save_btn = tk.Button(btn_frame, text="Lagre", command=self.lagre_csv)
        self.save_btn.grid(row=0, column=1, padx=5)

        # Start med første frame synlig
        self.first_frame.pack(padx=20, pady=20)

    def go_to_second_frame(self):
        gruppe = self.gruppe_entry.get().strip()
        if not gruppe:
            return

        self.gruppenummer = gruppe
        self.verdi_label.config(text=f"Gruppe: {self.gruppenummer}")
        self.first_frame.pack_forget()

        # Nullstill felt og status
        for e in self.entries:
            e.delete(0, tk.END)
        self.status_label.config(text="")

        # Fyll inn tidligere verdier hvis de finnes
        try:
            with self.lock:
                df = pd.read_csv(CSV_FILE)
            rad = df[df["gruppeNr"].astype(str) == self.gruppenummer]
            if not rad.empty:
                for i, navn in enumerate(KOLONNER[1:]):
                    self.entries[i].insert(0, str(rad.iloc[0][navn]))
        except Exception:
            pass

        self.second_frame.pack(padx=20, pady=20)

    
    def toggle_entry(self, entry_widget, bool_var):
        if bool_var.get():
            entry_widget.delete(0, tk.END)
            entry_widget.insert(0, "NaN")
            entry_widget.config(state="disabled")
        else:
            entry_widget.config(state="normal")
            entry_widget.delete(0, tk.END)


    def go_back_to_first_frame(self):
        self.second_frame.pack_forget()
        self.first_frame.pack_forget()
        self.password_frame.pack(padx=20, pady=20)

    

    def lagre_csv(self):
        verdier = []
        try:
            for entry, nig_var in zip(self.entries, self.nig_vars):
                tekst = entry.get()
                if nig_var.get() or tekst.strip().lower() == "nan":
                    verdier.append(np.nan)
                else:
                    verdier.append(float(tekst))
        except ValueError:
            self.status_label.config(text="Alle verdier må være gyldige tall eller NaN!", fg="red")
            return

        try:
            verdier = [float(e.get()) for e in self.entries]
        except ValueError:
            self.status_label.config(text="Alle verdier må være gyldige tall!", fg="red")
            return

        try:
            with self.lock:
                df = pd.read_csv(CSV_FILE)

                if self.gruppenummer in df["gruppeNr"].astype(str).values:
                    for i, navn in enumerate(KOLONNER[1:]):
                        df.loc[df["gruppeNr"].astype(str) == self.gruppenummer, navn] = verdier[i]
                else:
                    ny_rad = pd.DataFrame([[self.gruppenummer] + verdier], columns=KOLONNER)
                    df = pd.concat([df, ny_rad], ignore_index=True)

                df.to_csv(CSV_FILE, index=False)
            self.status_label.config(text=f"Lagret for gruppe {self.gruppenummer}", fg="green")
        except Exception as e:
            self.status_label.config(text=f"Feil ved lagring: {e}", fg="red")

def start_gui(csvLock, root):
    # root = tk.Tk()
    # root.title("Gruppe Input")
    # root.geometry("600x400")  # Bredde x Høyde i piksler
    # root.resizable(False, False)  # Låser vindusstørrelsen

    app = GruppeApp(root, csvLock)
    root.mainloop()


if __name__ == "__main__":
    lock = threading.Lock()
    start_gui(lock)
