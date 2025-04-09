function mqtt_send(topic_suffix, value)
    % Hent MQTT-klient frå base (persistent cache)
    persistent client
    if isempty(client) || ~client.Connected
        client = evalin("base", "mqttClient");
    end

    % Hent basetopic og my_id frå workspace
    basetopic = evalin("base", "basetopic");
    my_id = evalin("base", "my_id");

    % Lag full topic
    topic = basetopic + "/" + topic_suffix;

    % Lag melding: "my_id,verdi"
    message = sprintf("%d,%f", my_id, value);

    % Send
    write(client, topic, message);
end
