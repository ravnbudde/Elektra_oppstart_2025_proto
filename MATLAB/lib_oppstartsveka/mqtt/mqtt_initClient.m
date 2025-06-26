function client = mqtt_initClient(broker_address, port, varargin)
%MQTT_INITCLIENT Oppretter og returnerer ein MQTT-klient med dynamisk ID.
%   CLIENT = MQTT_INITCLIENT(BROKER_ADDRESS, PORT) koplar til MQTT-brokeren
%   spesifisert av BROKER_ADDRESS (t.d. 'tcp://localhost') og heiltalsverdien PORT
%   (t.d. 1883 eller 8883).
%
%   CLIENT = MQTT_INITCLIENT(BROKER_ADDRESS, PORT, Name, Value, ...)
%   lar deg spesifisere tilleggskonfigurasjon som namn-verdi-par, t.d.
%   'Username', 'Password', 'ClientID', osv. sjå dokumentasjon for
%   mqttclient(broker_address, args{:})
%
%   Funksjonen loggar til konsoll at klienten er tilkopla, og tildeler
%   einheits-ID basert på siste byte i lokal IPv4-adresse.
%   Denne ID-en blir lagra i workspace som variabelen `my_id` (uint8).
%
%   Output:
%       client  - MQTT-klientobjekt oppretta med mqttclient()
%
%   Sjå også: mqttclient, assignin

    % Kombiner Port og valfrie parameter
    args = [{'Port', port}, varargin];

    % Opprett MQTT-klient
    client = mqttclient(broker_address, args{:});

    % Informasjon
    fprintf("🔌 Kobla til broker: %s:%d\n", broker_address, port);

    % Hent IP og bruk siste byte som ID
    ip = get_local_ip();
    id = extract_id_from_ip(ip);
    assignin("base", "my_id", uint8(id));
    fprintf("📡 MQTT og ID klart: IP = %s → my_id = %d\n", ip, id);
end

function ip = get_local_ip()
    [~, out] = system('ipconfig');
    matches = regexp(out, 'IPv4 Address[^\d]*(\d+\.\d+\.\d+\.\d+)', 'tokens');
    ip = matches{1}{1};  % første IPv4
end

function id = extract_id_from_ip(ip)
    parts = split(ip, '.');
    id = str2double(parts{end});
end