# Utdelingsrepoet er fasiten

[BIAIS_Oppstartsuka_25](https://github.com/Adriaeik/BIAIS_Oppstartsuka_25) er
bekreftet av arrangøren som testet i praksis og korrekt for **alt utdelt materiale
i oppgave 1–3**: MATLAB/Simulink, fellesbibliotek, TiltMaze og oppgavetekst.

Historiske kopier er fjernet fra arrangørrepoet, også der innholdet avvek.
Forskjellene skal ikke slås sammen med den testede koden som del av oppryddingen.
Arrangørrepoet inneholder firmware, brokeroppsett, poengtavle og driftsveiledning.

## Hent eller oppdater nyeste main

Fra arrangørrepoets rot i PowerShell:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./scripts/Update-StudentMaterial.ps1
```

`ExecutionPolicy Bypass` gjelder bare denne prosessen og endrer ikke maskinens
policy. Der direkte skriptkjøring er tillatt, holder `./scripts/Update-StudentMaterial.ps1`.

Skriptet kloner `main` til `student-materiale/` første gang og oppdaterer senere
med `fetch` og `merge --ff-only`. Mappen er ignorert av arrangørrepoet og har sitt
eget Git-repo. Oppdateringen stopper ved lokale endringer, feil remote, annen
branch eller egne commits som ikke finnes på `origin/main`. Ingen reset eller
sletting utføres.

| Materiale | Sti etter henting |
| --- | --- |
| Oppgavetekst | `student-materiale/Oppstartsuke_BIAIS_2025.pdf` |
| MATLAB-oppstart | `student-materiale/startup.m` |
| Fellesbibliotek | `student-materiale/lib_oppstartsveka/` |
| Oppgave 1–3 | `student-materiale/oppgaaver/` |
| TiltMaze | `student-materiale/oppgaaver/Del_2_sensora/TiltMaze/` |

Kjør MATLAB fra `student-materiale/`, og kjør `startup.m`. Bruk spill og modeller
derfra. Fjern eventuelle gamle MATLAB-path-oppføringer fra andre arbeidskopier.
Se [oppsettsguiden](OPPSETT.md) for konkrete kommandoer og kontrollpunkter.

«Nyeste main» hentes når kommandoen kjøres, ikke automatisk i bakgrunnen.
Noter commit-ID-en skriptet skriver ut før arrangementet. Oppgavearbeid og lokale
nettverkstilpasninger må håndteres før neste oppdatering.

## Alternativ: vanlig submodule

En vanlig submodule lagrer en bestemt commit i arrangørrepoet. `branch = main`
styrer hvilken branch `update --remote` henter fra; vanlig kloning bruker den
lagrede commit-en. Se [Git-dokumentasjonen](https://git-scm.com/docs/git-submodule).
Den separate klonen over er løsningen som er satt opp nå.

Hvis dere senere vil registrere utdelingsversjonen i hver arrangørcommit, kan
en submodule legges til på en ny sti, for eksempel `utdelt/`:

```powershell
git submodule add -b main https://github.com/Adriaeik/BIAIS_Oppstartsuka_25.git utdelt
```

Ny kloning gjøres da med `git clone --recurse-submodules <URL>`. I en eksisterende
klone hentes registrert versjon med `git submodule update --init --recursive`.
Nyeste main hentes eksplisitt med `git submodule update --remote --checkout utdelt`;
gjennomgå endringen med `git diff --submodule`, og commit den valgte pekeren.
Hold submodulen ren før oppdatering. Ikke bruk begge henteløsninger samtidig;
oppdater guidens stier dersom dere går over til submodule.

## Historisk sammenligning før sletting

Sammenligningen brukte arrangørrepoets opprinnelige `HEAD` (`a738138`) og
utdelingsrepoets `main` på commit `4a88d6b81d369ec5082b9cee6d2454c050091b11`
(29. oktober 2025). Lokale rettinger, cachefiler og `.slxc` var utelatt.

| Fjernet område | Funn før sletting |
| --- | --- |
| `MATLAB/Oppstartsveka2025/` | 80 vanlige filer med samme relative sti var byte-identiske, inkludert alle fire oppgavemodeller og alle 75 biblioteksfiler. Gammel PDF og `.gitignore` avvek. |
| `MATLAB/lib_oppstartsveka/` | 74 av 75 filer var byte-identiske; `truck/truck_lib.slx` avvek. |
| `Del_2_sensora/TiltMaze/` | 18 vanlige filer: to byte-identiske, 14 like med normaliserte linjeskift og to faktiske innholdsendringer. Utdelingsrepoet hadde allerede riktig score-topic og MQTT-innlogging. |
| `MATLAB/MQTT_proto/` | 54 av 85 filer hadde identisk innhold et sted i utdelingsrepoet. Resten var historiske prototyper/andre versjoner. |
| `TiltMaze/` | Eldre spilldemo uten automatisk MQTT-score. |
| `OppgaveKladd/text/` og `OppgaveKladd/pic/` | Historiske tekster og illustrasjoner. Den utdelte PDF-en er fasiten. |

Totalt hadde **87 av utdelingsrepoets 101 filer** en byte-identisk motsvarighet
et sted i arrangørrepoet. Det fantes dessuten en ufullstendig submodule-peker
til TiltMaze under det gamle MATLAB-prosjektet, uten `.gitmodules`. Den er fjernet
sammen med de gamle mappene.

Den separate poengberegningsprototypen i `OppgaveKladd/score/` er også fjernet.
To historiske CSV-filer og PDF-rapporten er bevart lokalt under ignorert
`local-data/oppgavekladd-score/`. Poengtavlen brukte ingen av disse filene.
Tidligere versjonerte filer er tilgjengelige i Git-historikken;
utdelingsrepoets innhold er ikke endret av oppryddingen.
