function client = mqtt_init()
    % MQTT-klientoppsett
    % Vel broker mode:
    % 1 = localhost, 2 = lokal broker (192.168.1.3), 3 = HiveMQ
    broker_case = 1;

    % Sett opp broker basert på val
    switch broker_case
        case 1  % Localhost, ingen brukarnamn/passord
            broker_address = "tcp://localhost";
            port = 1883;
            client = mqttclient(broker_address, ...
                Port=port, ...
                KeepAliveDuration=10000);
    
        case 2  % Lokal broker med brukarnamn/passord
            broker_address = "tcp://192.168.1.3";
            port = 1883;
            client = mqttclient(broker_address, ...
                Port=port, ...
                Username="BIAISbroker", ...
                Password="shinyteapot294", ...
                KeepAliveDuration=10000);
    
        case 3  % HiveMQ, ingen autentisering
            broker_address = "tcp://broker.hivemq.com";
            port = 1883;
            client = mqttclient(broker_address, ...
                Port=port, ...
                KeepAliveDuration=10000);
    
        otherwise
            error("Ugyldig broker_case-verdi");
    end

fprintf("Kobla til broker: %s\n", broker_address);
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
