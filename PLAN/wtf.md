### Arbeidsfordeling og plan

## Felles
- Partikkel filter (SLAM (med resten))
- ESP interface


### RAVN 
- Lage interface for LiDAR
    - Styring av steppermotor
    - Filtrer data ?? til ein viss grad
    - kommunikasjon over UART (evt ros, i2c, noko)
    - Skal lese sensormåling og posisjon
    - Forslag; sende grader og avstand som 32bit med høgbyte som grader og avstand som lågbyte.

### ADRIAN
- Lage LiDAR
- Lage PCB for LiDAR
- Oppgåvetekst
    -  Oversette Annakvart avsnitt til nynorsk. Også kjent som ekte NORSK
- GitHub repo for oppgåva

### JØRGEN
- FÅ VIPS og LINUX på PCane
- Nettside?? blir nok simulink
- Interface igjennom matlab
    - GUI
- Oppgåvetekst
    - Annakvart avsnitt vil vere på bokmål

## Plan X; slam og lidar
Mapping av eit rom
VANSKELIG - avvent


## PLAN A; linjefølging og lokalisering med obstacle avoidance
- Hardkoda bane som vi så lagar i verkligheita
    - Lage tre baner på 1*1m kryssfiner
    - to av kvar bane
- Lager linje fast i bana
- hindringa som kan flyttast på
    - Skal alltid anta at det er ein anna linje når man kjem til ein "split"
    - ein splitt skal markerast med sort firkant på linja -> bilen må ta eit valg
- Modularitet
    - skal kunne plukke opp splittane og bytte med single baner
- GUI
    - Vise kjent kart med oppdatering av evt. hinder.

