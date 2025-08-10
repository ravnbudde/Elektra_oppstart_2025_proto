## Score system for Oppstartsuka
Program som er ansvarlig for å logge og vise frem score til gruppene. Oppgave 1 og 2 sender automatisk score gjennom MQTT til scriptet. Oppgave 3 må skrives inn manuelt gjennom GUI'en. Oppgave 1 og 2 kan også overskrives fra GUI'en, i tilleg til mulighet for å gi bonuspoeng.  
For at studenter ikke skal snike seg på PCen og øke egen score manuelt, er det et passord før man kan endre score. Dette passordet er 'ravn' og er strengt forbudt å endre i fremtidige versjoner. 

Oppgave 1 sender resultat som tid(?), og gjøres om til poeng gjennom en eksponensielt avtagende funksjon. (lavere resultat = høyere score). Samme gjelder oppgave 3
Oppgave 2 + bonus behandles som score direkte, og endres derfor ikke mellom resultater.csv og resultater_omregnet.csv. Funksjonalitet for omgjøringen finnes i score_to_points.py om det trengs å endre på noe. 

Oppbyggingen er ganske grei. En tråd er ansvarlig for GUI'en og oppdatering av resultater.csv ved manuell score-givning. En tråd er ansvarlig for å lese score fra resultater og gjøre de om til poeng og skrive de inn i resultater_omregnet. En tråd er ansvarlig for å lese resultater_omregnet og displaye et leaderboard, og en tråd hører etter MQTT-meldinger for å oppdatere resultater automatisk.


Hele programmet kjøres ved å kjøre setup.bat én gang på maskinen. Deretter kan det startes med run.bat.

NB! resultatfilene lagres kun lokalt. Ekstra implementasjon kan gjøres for å lagre dette på en server f.eks. raspberry'en, slik at det ikke er nødvendig at samme PC benyttes til å vise frem score hver dag.
