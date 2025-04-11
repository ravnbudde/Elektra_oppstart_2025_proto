function mqtt_subscribe(basetopic, topics)
    % Abonner på MQTT-topic(ar) og bind mot tilhøyrande callback
    % Eksempel:
    % mqtt_subscribe("zumo_car/07","accel/x")
    % forventar funksjon accel_x_Callback

    % eksempel: mqtt_subscribe("zumo_car/07", ["gyro/z", "encoder/right", "accel"]);


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
