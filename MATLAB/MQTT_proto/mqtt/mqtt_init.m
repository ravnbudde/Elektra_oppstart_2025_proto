function client = mqtt_init()
    % MQTT-klientoppsett
    client = mqttclient("tcp://192.168.1.3", ...
        Port=1883, ...
        Username="BIAISbroker", ...
        Password="shinyteapot294", ...
        KeepAliveDuration= 10000);
    % Lagre klient i base workspace
    % assignin("base", "mqttClient", client);

    % Hent IP og bruk siste byte som ID
    ip = get_local_ip();
    id = extract_id_from_ip(ip);
    assignin("base", "my_id", uint8(id));

    % Informasjon
    fprintf("📡 MQTT og ID klart:\n");
    fprintf("🔌 IP: %s → my_id: %d\n", ip, id);
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
