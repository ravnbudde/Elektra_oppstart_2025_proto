from manuell_score import start_gui
from mqtt_score_listener import start_mqtt
from score_to_points import kontinuerlig_les_og_omregn
from leaderboard import leaderboard_console_updater

import threading






if __name__ == "__main__":
    #Lag lock til csv (dette gjelder resulater.csv)
    csvLock = threading.Lock()
    convertLock = threading.Lock()
    
    # lag handles for trådene
    les_omregn_thread = threading.Thread(target=kontinuerlig_les_og_omregn, args=(csvLock, convertLock,), daemon=True)
    gui_thread = threading.Thread(target=start_gui, args=(csvLock,), daemon=True)
    leaderboard_thread = threading.Thread(target=leaderboard_console_updater, args=(convertLock,), daemon=True)

    # Start threadsa
    les_omregn_thread.start()
    gui_thread.start()
    leaderboard_thread.start()

    # MQTT bib lager tråder selv
    # start_mqtt(csvLock)

    gui_thread.join()
    les_omregn_thread.join()
    leaderboard_thread.join()




