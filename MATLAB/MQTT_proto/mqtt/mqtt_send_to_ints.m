function mqtt_send_to_ints(topic_suffix, value1, value2)
    % Hent MQTT-klient
    persistent client
    if isempty(client) || ~client.Connected
        client = evalin("base", "mqttClient");
    end

    % Hent topic og ID
    basetopic = evalin("base", "basetopic");
    my_id     = evalin("base", "my_id");
    topic     = basetopic + "/" + topic_suffix;

    % Sørg for at alle er int32
    my_id  = int16(my_id);
    value1 = int16(value1);
    value2 = int16(value2);

    % Pakk til byte array (12 byte)
    payload = typecast([my_id, value1, value2], 'uint8');  % 3 x 4 byte

    % Konverter til char for sending
    char_payload = char(payload);  % Dette lagar 1 char per byte (kan vere uleseleg)

    % Send som string (MQTT godtar dette)
    write(client, topic, char_payload);
end
