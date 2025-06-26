function mqtt_subscribe(basetopic, topics)
%MQTT_SUBSCRIBE Abonnerer på MQTT-topicar og binder dei til callback-funksjonar.
%   MQTT_SUBSCRIBE(BASETOPIC, TOPICS) abonnerer på ein eller fleire MQTT-topicar
%   ved å kombinere BASETOPIC og kvar suffix i TOPICS. For kvar kombinasjon
%   forventar funksjonen at det finst ein tilhøyrande callback-funksjon med namn:
%       <suffix> → <suffix>_Callback
%   der skråstrekar ('/') i suffix blir erstatta med understrek ('_').
%
%   Parameterar:
%       BASETOPIC – streng, t.d. 'zumo_car/07' (utan skråstrek til slutt)
%       TOPICS    – anten éin streng eller ein string-array med topic-suffixar
%
%   Funksjonen:
%       - Hentar MQTT-klient frå base workspace
%       - Bygg komplette topicar som BASETOPIC + "/" + SUFFIX
%       - Knyter kvar topic til ein lokal funksjon kalla <suffix>_Callback
%
%   Eksempel:
%       mqtt_subscribe("zumo_car/07", "accel/x")  
%           → krev funksjonen `accel_x_Callback`
%
%       mqtt_subscribe("zumo_car/07", ["gyro/z", "encoder/right"])
%           → krev `gyro_z_Callback` og `encoder_right_Callback`
%
%   Merk:
%       Dersom ein callback-funksjon ikkje finst, blir det gitt ei åtvaring
%       og den aktuelle topicen blir hoppa over.
%
%   Sjå også: subscribe, mqtt_initClient

    if nargin < 2 || ~isStringScalar(basetopic)
        error("Du må sende inn basetopic som ein streng utan skråstrek på slutten.");
    end

    persistent client
    if isempty(client) || ~client.Connected
        try
            client = evalin("base", "mqttClient");
        catch
            error("❌ MQTT-klient ikkje funne i base workspace. Kjør mqtt_init først.");
        end
    end

    if ischar(topics) || isStringScalar(topics)
        topics = string(topics);
    elseif ~isstring(topics)
        error("topics må vere ein string eller string-array.");
    end

    for i = 1:numel(topics)
        suffix = strtrim(topics(i));
        full_topic = basetopic + "/" + suffix;

        % Konverter suffix til gyldig funksjonsnamn
        callback_basename = strrep(suffix, "/", "_");
        callback_name = callback_basename + "_Callback";

        if exist(callback_name, 'file') ~= 2 && ...
           exist(callback_name, 'builtin') ~= 5 && ...
           exist(callback_name, 'var') ~= 1
            warning("❌ Callback-funksjon %s finst ikkje. Hopper over.", callback_name);
            continue;
        end

        callback_handle = str2func(callback_name);

        try
            subscribe(client, full_topic, 'Callback', callback_handle);
            fprintf("✅ Abonnerte på: %s → %s\n", full_topic, callback_name);
        catch ME
            warning("❌ Feil ved abonnement på %s: %s", full_topic, ME.message);
        end
    end

    disp(client.Subscriptions);
end
