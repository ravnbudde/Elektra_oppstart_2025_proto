function mqtt_send(topic, value)
    persistent client
    if isempty(client) || ~client.Connected
        client = evalin("base", "mqttClient");
    end

    % Konverter og send
    valStr = string(value);
    write(client, topic, valStr);
end
