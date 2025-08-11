import time
import pandas as pd
import threading
import math
import os

LESNING_INTERVAL_SEC = 1.0  # Endre her for ønsket intervall

KOLONNER = ["gruppeNr", "del1.1", "del1.2", "del2.1", "del2.2", "del3", "bonus"]

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ORIGINAL_CSV = os.path.join(SCRIPT_DIR, "resultater.csv")
NY_CSV = os.path.join(SCRIPT_DIR, "resultater_omregnet.csv")

OPPG1_1_MAX = 30000
OPPG1_2_MAX = 30000
OPPG3_MAX = 40000

OPPG1_1_TID = 10000
OPPG1_2_TID = 10000
OPPG3_TID = 200

def omregn_verdier(df):
    df_omregnet = df.copy()
    for col in df.columns:
        if col == "del1.1":
            df_omregnet[col] = pd.to_numeric(df[col], errors='coerce').apply(
                lambda t: beregn_score(t, OPPG1_1_TID, OPPG1_1_MAX) if pd.notnull(t) else 0
            )
        elif col == "del1.2":
            df_omregnet[col] = pd.to_numeric(df[col], errors='coerce').apply(
                lambda t: beregn_score(t, OPPG1_2_TID, OPPG1_2_MAX) if pd.notnull(t) else 0
            )
        elif col == "del3":
            df_omregnet[col] = pd.to_numeric(df[col], errors='coerce').apply(
                lambda t: beregn_score(t, OPPG3_TID, OPPG3_MAX) if pd.notnull(t) else 0
            )
        elif col in ["del2.1", "del2.2", "bonus"]:
            # Behold som det er (kun konverter til float, erstatt NaN med 0)
            df_omregnet[col] = pd.to_numeric(df[col], errors='coerce').fillna(0)
        elif col == "gruppeNr":
            # Behold gruppeNr uendret
            pass
        else:
            # For andre kolonner, om nødvendig, gjør en standard konvertering
            print("Leser fra kolonne som ikke burde eksistere (resultater_omregnet.csv)")
    return df_omregnet


def kontinuerlig_les_og_omregn(csvLock, convertLock):
    while True:
        with csvLock:
            try:
                if os.path.exists(ORIGINAL_CSV):
                    df = pd.read_csv(ORIGINAL_CSV)
                else:
                    df = pd.DataFrame(columns=KOLONNER)

                df_omregnet = beregn_score(df)

                # låser en lås inne i en annen, men burde gå fint da den andre tråden som bruker convertLock ikke har tilgang til csvLock
                with convertLock:
                    df_omregnet.to_csv(NY_CSV, index=False)
                # print(f"[{time.strftime('%H:%M:%S')}] Oppdatert {NY_CSV}")

            except Exception as e:
                print(f"Feil ved lesing/skriving: {e}")

        time.sleep(LESNING_INTERVAL_SEC)

# def beregn_score(sekunder, max_tid, max_score):
#     """
#     Returnerer score basert på tid (sekunder) som følger en eksponentielt avtagende kurve.
#     Score starter på max_score ved 0 sekunder, og er 1/10 av max_score ved max_tid.
#     """
#     if sekunder < 0:
#         sekunder = 0

#     k = math.log(10) / max_tid
#     score = max_score * math.exp(-k * sekunder)
#     return score

def beregn_score(df):
    def normalize(series, max_points, higher_is_better=True):
        is_nan = series.isna()

        min_val, max_val = series.min(skipna=True), series.max(skipna=True)
        if max_val == min_val:
            norm = pd.Series([max_points/2]*len(series))
        elif higher_is_better:
            norm = (series - min_val) / (max_val - min_val) * max_points
        else:
            norm = (max_val - series) / (max_val - min_val) * max_points
        norm[is_nan] = 0
        return norm

    # Beregn poeng per oppgave
    oppg1_1_poeng = normalize(df["del1.1"], 100, higher_is_better=False) 
    oppg1_2_poeng = normalize(df["del1.2"], 100, higher_is_better=False)
    oppg2_poeng = normalize(df["del2"], 100, higher_is_better=True)
    oppg3_poeng = normalize(df["del3"], 100, higher_is_better=False)
    bonus_poeng = df["bonus"].fillna(0)

    sum_poeng = 0.5*oppg1_1_poeng + 0.5*oppg1_2_poeng + oppg2_poeng + oppg3_poeng + bonus_poeng

    df["sum_poeng"] = sum_poeng
    df["p_del1.1"] = oppg1_1_poeng
    df["p_del1.2"] = oppg1_2_poeng
    df["p_del2"] = oppg2_poeng
    df["p_del3"] = oppg3_poeng

    return df



if __name__ == "__main__":
    lock = threading.Lock()
    lock2 = threading.Lock()

    # Start les/omregn-funksjonen i egen tråd:
    les_omregn_thread = threading.Thread(target=kontinuerlig_les_og_omregn, args=(lock,lock2), daemon=True)
    les_omregn_thread.start()
    les_omregn_thread.join()
