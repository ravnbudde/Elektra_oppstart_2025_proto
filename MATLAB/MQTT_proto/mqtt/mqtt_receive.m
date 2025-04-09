function value = mqtt_receive(topic)
    persistent client
    if isempty(client) || ~client.Connected
        client = evalin("base", "mqttClient");
    end

    value = NaN; % fallback
    msg = peek(client, topic);
    if ~isempty(msg)
        value = str2double(msg);
    end
end
