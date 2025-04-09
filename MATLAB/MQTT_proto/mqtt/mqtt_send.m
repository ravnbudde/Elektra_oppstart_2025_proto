function mqtt_send(topic_suffix, varargin)
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
