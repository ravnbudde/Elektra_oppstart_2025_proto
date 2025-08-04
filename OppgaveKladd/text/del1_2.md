## Del 1.2 - Forbikøyring 

Det kan vere **vanskeleg å halde fartsgrensa** når bilen framfor deg er ein skikkelig *sinke*. For å redde både pulsen og køen bak må du altså ta en forbikøyring – og då treng vi å regulere både **fart** og **sving** på ein ryddig måte.

---

### 1 PID-blokker – takk og pris!

Heldigvis har Simulink ein **innebygd PID-blokk**! Og det er bra, for det hadde blitt mykje rot og stygge strekar viss vi måtte teikna han opp kvar gong – og tru meg, du kjem til å bruke PID-blokka mykje. Berre å glede seg!

Vi har allereie sotte inn ein **KVIT PID-blokk** i modellen. Den brukar du til å halde farta stabil. Legg inn dei parametera du fann i førre oppgåve.

---

### 2 Setje inn PID for styring (rattet)
Dokke skal no sette inn ein PID blokk sjølv der den manglar
> **Viktig:** Du har bare **én** PID-blokk i modellen fra før (den hvite for fart).
1. Dobbelklikk på tomt område i modellen, skriv `PID`, og trykk **Enter**.
2. Kopla inn:

   * **Input:** `e_pos` (kor langt frå midtlina du er)
   * **Output:** `u_ratt` (vinkel i radianar – 0 = rett fram, + = venstre, – = høgre)

> Tips: Rattet er **følsamt**.
---

### 3 Automatisk forbikøyringssekvens

Vi utfører forbikjøringa ved at ein boks endrer referansen for fart og posisjon basert på kor i forbikjøringa man beifinner seg.

| Steg | Kva skjer                     | Referanse for fart ($r_v$) | Referanse for kurs ($r_y$) |
| ---- | ----------------------------- | -------------------------- | -------------------------- |
| 1    | Akselerer opp til fartsgrensa | $v_{\text{grense}}$        | høgre felt                   |
| 2    | Sving ut + gass på            | $v_{\text{topp}}$          | venstre felt               |
| 3    | Sving tilbake + ro ned        | $v_{\text{grense}}$        | høgre felt                   |

Når forbikjøringa er gjennomført, vi er tilbake i fratsgrens og vi ligger riktig i fila avsluttast simulasjonen automatisk. 

---
## Oppgåve

*Raskast gjennom forbikøyringa utan oversving eller oscillasjonar.* Simulink målar automatisk tid frå start til du ligg pent tilbake i høgre felt.
