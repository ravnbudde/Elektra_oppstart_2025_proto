# Arduino som ISP
Følg tutorial her: https://www.instructables.com/How-to-Program-an-Attiny85-From-an-Arduino-Uno/
Bør være straight forward

# Arduino ISP gjennom Atmel Studio
Her begynner morroa.  
Først last ned AVRDUDE herfra: https://download.savannah.gnu.org/releases/avrdude/ (nyeste zip fila per skrivende tid: https://download.savannah.gnu.org/releases/avrdude/avrdude-6.3-mingw32.zip)  

Lagre og pakk ut zipfila et sted på PCen (bør ikke ha noe å si hvor, ikke bruk mellomrom og sånt)  

I ATmel studio: 
- Tools -> external tools  
- Title: &Deploy
- Command: C:\"path_til"\avrdude.exe
- Arguments: -e -v -pATtiny85 -carduino -PCOM3 -b19200 -D -Uflash:w:"$(ProjectDir)Debug\$(ItemFileName).hex":i -C"C:\"path_til"\avrdude.conf"

**NB** Bytt ut COM3 til COM arduinoen er koblet i. Butt ut "path_til" med path til avrdude.exe/conf  

For å laste over: Tools -> Deploy


