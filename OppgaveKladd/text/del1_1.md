## Del 1.1 - Regulering av fart

Å regulere et system betyr å styre det mot en ønsket tilstand. Den enkleste formen er **åpen sløyfe**: vi bestemmer et pådrag uten å se hvordan systemet faktisk oppfører seg. I praksis bruker vi nesten alltid **lukket sløyfe**, der vi måler forskjellen mellom

* referansen $r$ – det vi *ønsker*
* målt verdi $y$ – det vi *har*

Avviket (*error*) defineres som
$\displaystyle e(t)=r(t)-y(t)$.

Målet er å få $e(t)\rightarrow 0$.

---

## Teori for den interessaerte – kan hoppes over

I det aller meste av industrien gjøres dette med en **PID-regulator** (*Proportional–Integral–Derivative*). Hele ideen kan samles i én formel:

$$
\boxed{\,
u(t)=K_{p}\,e(t)\;+\;K_{i}\!\int_{0}^{t} e(\tau)\,d\tau\;+\;K_{d}\,\frac{de(t)}{dt}\,}
$$

hvor

* $u(t)$ = pådraget vi sender til systemet
* $K_p, K_i, K_d$ = regulatorparametere som må justeres

### P-leddet

*Ser på “nå”:* $\;u_P=K_p e(t)$.
Jo større $|e|$, desto hardere gasser (eller bremser) vi. Nær referansen minker $u_P$, og vi kan få **stasjonært avvik** – systemet legger seg under fartsgrensen og kommer aldri helt opp.

### I-leddet

*Ser på “fortiden”:* $\;u_I=K_i\!\int e\,dt$.
Små feil over lang tid summeres opp til noe stort, slik at regulatoren trykker litt ekstra på gassen og fjerner stasjonært avvik. Velges $K_i$ for høyt, får du treg og ustabil regulering.

### D-leddet

*Ser inn i “fremtiden”:* $\;u_D=K_d \frac{de}{dt}$.
Reagerer på hvor raskt feilen endrer seg. Demper oversving og gjør systemet roligere, men for høy $K_d$ kan forsterke støy.

Alle tre leddene summeres til ett felles pådrag $u(t)$.

---

## Cruise-kontroll for lastebil

Bevegelsen til en lastebil kan beskrives med Newtons andre lov
$\displaystyle F = m\,a$.
Motoren skaper drivkraft, mens luftmotstand trekker hastigheten ned. I Simulink-modellen deres kan dere bare kontrollere gasspådraget; bremser finnes ikke. Luftmotstanden hindrer bilen i å akselerere i det uendelige og gjør modellen tilstrekkelig realistisk for øvingen.

---

## Oppgave

1. Få lastebilen til å kjøre mot $20\;\text{m/s}$.
2. Juster $K_p, K_i, K_d$ uten å bryte fartsgrensen.
3. Undersøk:

   * Hva skjer når $K_p$ er for stor / for liten?
   * Får du steady-state-feil? Legg til eller juster $K_i$.
   * Oscillerer systemet? Prøv å øke $K_d$.

**SCORE:** Raskest til fartsgrensen *uten* oversving.
Hurtighet måles med tidskonstanten $\tau$: tiden til 63 % av referansen, altså når lastebilen når $12{,}6\;\text{m/s}$.

---

### Hva du skal sitte igjen med

* Intuitiv følelse av hvordan tilbakekopling påvirker et system
* Forståelse av rollen til P-, I- og D-leddet
* Evnen til å finne balansen mellom “for mye” og “for lite” styring

Bruk ikke uhorvelig tid, men se øvingen som en mulighet til å teste og lære.
