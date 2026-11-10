# Repooversikt etter opprydding

Utdelingsrepoet [BIAIS_Oppstartsuka_25](https://github.com/Adriaeik/BIAIS_Oppstartsuka_25)
er ifølge arrangøren testet i praksis og er fasiten for **alt utdelt materiale
i oppgave 1–3**: MATLAB/Simulink, biblioteker, TiltMaze og oppgavetekst.
Eldre lokale varianter skal ikke brukes som alternative fasiter.

## Hva ligger hvor?

| Område | Rolle |
| --- | --- |
| `student-materiale/` | Separat, ignorert klone av utdelingsrepoet. Hentes med `scripts/Update-StudentMaterial.ps1`. |
| `Arduino/test_r4_zumo/` | Arrangørens firmware for UNO R4 WiFi / Zumo. |
| `Bæret/score_hosted/` | Eneste poengtavle; bekreftet testet i praksis. Flask, SQLite og systemd. |
| `Bæret/mqtt-broker/` | Mosquitto på Pi-en; installeres sammen med poengtavlen. |
| `Bæret/mqtt-broker/windows/` | Alternativ broker i Docker på Windows. |
| `Bæret/`, `Programvare/` | Pi-installasjon, drift og maskinoppsett. Kjør `sudo bash install.sh` fra `Bæret/`. |
| `PrintFiles/`, `Arduino_biloversikt.xlsx` | Maskinvareunderlag. |
| `PLAN/` | Historisk planlegging. |

## Fjernet fra arrangørrepoet

- `MATLAB/`: oppgavemodeller, bibliotekskopier, prototyper, gammel oppgave-PDF,
  backupmodeller og generert Simulink-cache.
- `Del_2_sensora/`: alternative sensormodeller og gammel TiltMaze-kampanje.
- `TiltMaze/`: eldre spillvariant og kartgenerator.
- `OppgaveKladd/text/` og `OppgaveKladd/pic/`: gamle oppgavetekster og illustrasjoner.
- Resten av `OppgaveKladd/`: en separat poengberegningsprototype og avhengighetsliste.
  De to CSV-filene og PDF-rapporten er bevart under ignorert
  `local-data/oppgavekladd-score/`. Ingen av filene brukes av poengtavlen.
- `pitch/`: seks presentasjonsfiler (Markdown, PDF, video og tre bilder).
  Ingen kode eller installasjon brukte dem; lenkene var interne i presentasjonen.
- Den ufullstendige Git-submodule-pekeren til TiltMaze under det gamle MATLAB-prosjektet.
- `score/`: den gamle skrivebordsapplikasjonen. Lokale CSV-resultater er kopiert
  til ignorert `local-data/score-desktop/` før sletting.

Også lokale TiltMaze-rettinger fra den første gjennomgangen er fjernet sammen
med kopien. Utdelingsrepoet brukes uendret; det er ikke pushet endringer dit.
Slettede, tidligere versjonerte filer kan fortsatt finnes i Git-historikken.
Se [sammenligningen](UTDELINGSMATERIALE.md) for bakgrunnen.

## Arrangørforhold som fortsatt bør ryddes

1. **Hemmeligheter:** `Arduino/test_r4_zumo/lib/arduino_secrets.h` spores allerede
   av Git selv om den står i `.gitignore`. Eksempelheader er lagt til.
   Pi-notater inneholder også innlogging. Samle lokal
   konfigurasjon og bytt delte passord koordinert med utstyret ved behov.
2. **Firmwarebygg:** Koden inkluderer `Arduino_FreeRTOS.h`, men avhengigheten er
   ikke eksplisitt deklarert i `platformio.ini`. En ren firmwarebygging er ikke
   testet i denne gjennomgangen. `rplidar_arduino` er deklarert uten funnet bruk
   fra applikasjonskoden; vurder fjerning etter byggtest. Behold det tilpassede
   lokale Zumo-biblioteket inntil eventuell erstatning er testet på R4.
3. **Docker-skript:** `generate_compose.bat` erstatter en `{{PORT}}`-markør som
   ikke finnes i Compose-filen. `start_mqtt.bat` kan vente uendelig på databasen.
   Bruk direkte Compose-kommandoer fra [oppsettsguiden](OPPSETT.md).
   Brukerskriptet heter faktisk `add_mqqt_user.bat`.
4. **Broker-data:** Den gamle Windows-databasen er beholdt lokalt under
   `Bæret/mqtt-broker/windows/data/mosquitto.db`. Den nye plasseringen ignoreres
   av Git; runtime-data skal ikke legges tilbake i versjonskontroll.
5. **Reproduserbar drift:** PlatformIO-avhengigheter bruker Git-URL-er uten fast
   revisjon, og Docker bruker `latest`. Registrer fungerende arrangørversjoner
   og utdelingsrepoets commit før arrangementet.

## Gjort og kontrollert

- Samlet [oppsettsguide](OPPSETT.md), README og veiledning for henting av studentmateriale.
- Fjernet historiske kopier av utdelt materiale og de tilhørende cachefilene.
- Rettet Arduino `main.cpp` sin include-sti til eksisterende `lib/global_objects.h`.
- Lagt til eksempelheader og ignore-regler for lokale miljøer og driftsdata.
- Sju eksisterende tester for webtavle og CSV-import bestått.
- Henteskriptets førstegangskloning, oppdatering og stopp ved lokale endringer kontrollert.
- Pi-programmene og den samlede installereren ligger under `Bæret/`.
- Ni nye konfigurasjonstester består på Windows og Linux; Bash-syntaks er kontrollert.

Arrangøren har bekreftet praktisk testing av utdelingsrepoet og `Bæret/score_hosted/`. Firmwarebygg,
Pi-tjenester og fysisk utstyr er ikke prøvd av denne gjennomgangen.

Den filvise [slettekontrollen](SLETTEKONTROLL.csv) gjelder de 709 fjernede
oppgave-/prototypeoppføringene, ikke den senere fjerningen av `score/` eller
de fem filene som var igjen i `OppgaveKladd/score/`.
Den viser 216 identiske filer, 23 som bare avviker i linjeskift, 17 med annen
versjon på tilsvarende sti, 39 uten direkte filmotstykke, 413 genererte filer
og én Git-peker. Kladdetekster finnes ikke som separate Markdown-filer i
utdelingsrepoet; den gjeldende oppgaveteksten finnes der som PDF.
Historisk materiale uten motstykke er godkjent fjernet av arrangøren.
Ingen Arduino-filer er fjernet.
