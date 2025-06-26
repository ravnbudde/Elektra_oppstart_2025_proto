function mqtt_send(topic_suffix, varargin)
%MQTT_SEND Send MQTT-melding med ID-prefiks til eit gitt topic-suffix.
%   MQTT_SEND(TOPIC_SUFFIX, VALUE1, VALUE2, ...) sender ein melding over MQTT
%   til topic 'zumo_car/<my_id>/<TOPIC_SUFFIX>', der `my_id` er henta frå workspace.
%
%   Meldinga blir bygd som ein kommaseparert streng med:
%       <my_id>,VALUE1,VALUE2,...
%       dette for atbilen skal kunne merke om fleire forsøker å sende
%       melding til hen.
%
%   Parameterar:
%       TOPIC_SUFFIX  – streng som blir lagt til etter `basetopic/`
%       VALUE1, ...   – vilkårleg mange verdiar (tal, strengar, osv.) som skal sendast
%
%   Funksjonen hentar MQTT-klienten og basetopic frå workspace
%   (`mqttClient`, `basetopic`, `my_id`), og opprettar dei ved behov.
%
%   Eksempel:
%       mqtt_send("status", 42)
%       mqtt_send("sensor", 3.14, "OK")
%
%   Sjå også: write, mqtt_setup, mqtt_initClient

    % Hent MQTT-klient
    persistent client
    if isempty(client) || ~client.Connected
        client = evalin("base", "mqttClient");
    end

    % Hent topic og ID
    basetopic = evalin("base", "basetopic");
    my_id = evalin("base", "my_id");
    topic = basetopic + "/" + topic_suffix;

    % Bygg meldinga: start med ID, legg til alle verdiar
    message = sprintf("%d", my_id);
    for k = 1:numel(varargin)
        message = message + "," + string(varargin{k});
    end

    % Send meldinga
    write(client, topic, message);
end
