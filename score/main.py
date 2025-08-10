from manuell_score import GruppeApp
from mqtt_score_listener import start_mqtt
from score_to_points import kontinuerlig_les_og_omregn
from leaderboard import start_leaderboard

import threading
import tkinter as tk


# NB!!!
# Manuel GUI syncer tallene når du går inn på en gruppe, så om du går inn på en gruppe og en automatisk oppgave
# Endres før du lagrer, vil den tilbakestilles
# Derfor viktig at gruppen ikke gjør automatiske oppgaver (1 og 2) samtidig som man skriver inn score (oppg3, bonus)
# Tviler på at det noengang blir et problem, men greit å vite


if __name__ == "__main__":
    #Lag lock til csv (dette gjelder resulater.csv)
    csvLock = threading.Lock()
    convertLock = threading.Lock()

    
    # lag handle og start omregning
    les_omregn_thread = threading.Thread(target=kontinuerlig_les_og_omregn, args=(csvLock, convertLock,), daemon=True)
    les_omregn_thread.start()

    # MQTT bib lager tråder selv
    start_mqtt(csvLock)

  
    root = tk.Tk()
    root.title("Gruppe Input")
    root.geometry("600x400")  # Bredde x Høyde i piksler

    app = GruppeApp(root, csvLock)
    # Lag leaderboard vindu i samme tråd
    leaderboard_app = start_leaderboard(root, convertLock)
    # Start hovedløkken (blokkering)
    root.mainloop()





