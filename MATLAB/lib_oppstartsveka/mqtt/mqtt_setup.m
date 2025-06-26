function mqtt_setup(car_id, broker, port, bn, pw, topics)
%MQTT_SETUP Initialiserer og konfigurerer MQTT-kommunikasjon for ein Zumo-bil.
%   MQTT_SETUP(CAR_ID, BROKER, PORT, USERNAME, PASSWORD, TOPICS) set opp ein
%   MQTT-klient for ein gjeven bil-ID, med tilhøyrande broker-informasjon
%   og abonnerer på relevante topicar.
%   NB! støtter kun TCP
%
%   Obligatoriske parameter:
%       CAR_ID     – heiltal brukt som bilens-ID og filter av topics
%       BROKER     – adresse til MQTT-broker, t.d. 'localhost' eller 'broker.io'
%
%   Valfrie parameter:
%       PORT       – portnummer som streng (standard: "1883")
%       USERNAME   – brukarnamn for autentisering (standard: tom streng)
%       PASSWORD   – passord for autentisering (standard: tom streng)
%       TOPICS     – streng-array av MQTT-topicar. Dersom ikkje oppgitt,
%                    blir alle verdiar frå enumen `MQTTTopics` brukt.
%
%   Ved køyretid blir desse variablane sett i base-workspace:
%       - car_id     : einheits-ID som uint8
%       - basetopic  : MQTT-topic base, 'zumo_car/<car_id>'
%       - mqttClient : MQTT-klientobjekt (frå mqttclient())
%
%   Funksjonen:
%       - Legg til alle undermapper til prosjektets path
%       - Initialiserer sensor-data via sensor_ref()
%       - Kallar mqtt_initClient() for å opprette klient
%       - Abonnerer på oppgjevne eller alle standardtopicar
%   Eksempel på bruk:
%       mqtt_setup(1, "localhost")                                  % brukar port 1883 utan brukarnamn/passord
%       mqtt_setup(2, "localhost", "8883")                          % brukar spesifikk port
%       mqtt_setup(3, "localhost", "1883", "user")                  % med brukarnamn, tomt passord
%       mqtt_setup(4, "localhost", "1883", "user", "pw")            % med brukarnamn og passord
%       mqtt_setup(5, "localhost", "1883", "", "", ["cmd", "pid"])  % med eigne topicar - ellers bruker den default
%
%   Sjå også: mqtt_initClient, mqttclient, mqtt_subscribe, sensor_ref

    % 📦 Standardverdiar
    default_port   = "1883";
    default_user   = "";
    default_pass   = "";

    % 1. Verdiar basert på input
    if nargin < 3 || isempty(port)
        port = default_port;
    end
    if nargin < 4 || isempty(bn)
        bn = default_user;
    end
    if nargin < 5 || isempty(pw)
        pw = default_pass;
    end

    % 🔧 Fortset som før...
    fprintf("🚗 Setter opp system med car_id = %d\n", car_id);
    assignin("base", "car_id", uint8(car_id));

    basetopic = sprintf("zumo_car/%d", car_id);
    assignin("base", "basetopic", basetopic);
    fprintf("🔗 base topic satt til: %s\n", basetopic);

    % 2. Legg relevante mapper til path
    projectRoot = fileparts(mfilename('fullpath'));
    addpath(genpath(projectRoot));
    fprintf("📁 Alle undermapper i %s lagt til.\n", projectRoot);

    % 3. Init SensorData
    sensor_ref("set", SensorData());
    disp(sensor_ref("get"));

    % 4. Bygg broker-adresse
    if ~contains(broker, "://")
        broker = "tcp://" + broker;
    end

    % 5. Opprett MQTT-klient
    if bn == ""
        client = mqtt_initClient(broker, str2double(port));
    else
        client = mqtt_initClient(broker, str2double(port), "Username", bn, "Password", pw);
    end
    assignin("base", "mqttClient", client);

    % 6. Hent topicar
    if nargin < 6 || isempty(topics)
        allTopics = enumeration("MQTTTopics");
        topics_final = string(arrayfun(@char, allTopics, "UniformOutput", false));
        fprintf("📚 Brukar %d topicar frå enum.\n", numel(topics_final));
    else
        topics_final = string(topics);
        fprintf("📥 Brukar %d topicar frå input.\n", numel(topics_final));
    end

    mqtt_subscribe(basetopic, topics_final);
end
