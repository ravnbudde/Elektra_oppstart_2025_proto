# Elektra – oppstartsuka

Arrangørrepo for firmware, nettverk, broker, poengtavle og drift.
[BIAIS_Oppstartsuka_25](https://github.com/Adriaeik/BIAIS_Oppstartsuka_25) er
fasiten for **alt utdelt materiale i oppgave 1–3**: MATLAB/Simulink,
fellesbibliotek, TiltMaze og oppgavetekst. Denne koden er testet i praksis.

**Start med [oppsett og gjennomføring](docs/OPPSETT.md).** Guiden går fra lokal
prøvekjøring til Raspberry Pi, bil og studentoppgaver, med kontrollpunkter og feilsøking.

Se [repooversikt og ryddeplan](docs/REPOSTATUS.md) for duplikater, kjente feil og
moduler som kan være utdaterte. Arrangørinstallasjonen er gjennomgått fra kode;
den er ikke testet mot fysisk utstyr i denne gjennomgangen.

| Del | Anbefalt utgangspunkt |
| --- | --- |
| Poengtavle i nettleser / på Pi | [`Bæret/score_hosted/`](Bæret/score_hosted/README.md) |
| Utdelte MATLAB-oppgaver og bibliotek | `student-materiale/` etter [hentekommandoen](docs/UTDELINGSMATERIALE.md) |
| Arduino UNO R4 WiFi / Zumo | [`Arduino/test_r4_zumo/`](Arduino/test_r4_zumo/README.md) |
| TiltMaze med kampanje og MQTT-score | `student-materiale/oppgaaver/Del_2_sensora/TiltMaze/` |
| Oppgavetekst for oppgave 1–3 | `student-materiale/Oppstartsuke_BIAIS_2025.pdf` |
| Alternativ broker i Docker på Windows | [`Bæret/mqtt-broker/windows/`](Bæret/mqtt-broker/windows/README.md) |

Felles format for score er `zumo_car/<bil-id>/score/<oppgave>/<gruppe>`.
Oppgavene heter `del1.1`, `del1.2`, `del2`, `del3` og `bonus`.

[Sammenligning med utdelingsrepoet](docs/UTDELINGSMATERIALE.md) viser hva som
er kopiert og hva som avviker, samt hvordan en vanlig submodule kan innføres.
Henteskriptet følger nyeste `main` ved kjøring og stopper ved lokale endringer.

`Bæret/score_hosted/` er bekreftet testet i praksis og er eneste poengtavle.
Den gamle `score/`-applikasjonen er fjernet.

Installer eller oppdater Pi-en fra repoets rot:

```bash
git pull --ff-only
cd Bæret
sudo bash install.sh
```

Se [Pi-veiledningen](Bæret/README.md) for første oppsett og drift.
