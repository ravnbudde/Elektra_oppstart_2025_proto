# Programmvare som må installeres på VIPS-PCane

Ei oversikt over nødvendige verktøy og tillegg som må vere installert for å utvikle, teste og vise fram sensorbaserte system.

---

## Grunnleggande verktøy

| Verktøy | Forklaring |
|--------|------------|
| Git | Brukast for å laste ned dependencies i PlatformIO (t.d. via `lib_deps` i `platformio.ini`) |
| MATLAB R2024a | Skal det oppgradererast til 14b bør vi vite dette helst i forkankt |
| Simulink | med Industrial Communication Toolbox, UAV Toolbox, Aerospace Toolbox|
| Vs code | Brukast for systemmodellering og animasjonar |
| PlatformIO | ikkje kritisk. integrerer med VS Code |

---

## MATLAB Add-ons

### For MQTT-kommunikasjon
- **Industrial Communication Toolbox**  
  Nødvendig for å sende og motta MQTT-meldingar direkte frå MATLAB

### For sensorframvising og UAV-animasjon
- **UAV Toolbox**  
  Brukast til å visualisere posisjon og orientering i 3D, t.d. `UAV Animation`-blokka i Simulink  
- **Aerospace Toolbox**  
  Inneheld funksjonar som `eul2quat` for å konvertere Euler-vinklar til kvaternionar

### Generelt for sanntidstesting
- **Simulink Real-Time**  
  Støttar kommunikasjon og testing mot fysiske system i sanntid

---

## MQTT-debugging og testing - Ikkje nødvendig

- **Mosquitto (broker + klientverktøy)**  
  Installer som lokal MQTT-broker for feilsøking av kommunikasjon  
  Verktøy: `mosquitto_sub`, `mosquitto_pub`  
  Nyttig dersom ein klient ikkje får kontakt – ein kan da teste manuelt frå terminal

