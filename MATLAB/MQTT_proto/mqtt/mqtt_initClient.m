function client = mqtt_initClient(broker_address, port, varargin)
    % MQTT-klientoppsett med fleksibel brokerkonfigurasjon
    % broker_address: t.d. 'tcp://localhost' eller 'ssl://broker.hivemq.com'
    % port: heiltal, t.d. 1883 eller 8883
    % varargin: valfrie parameter som 'Username', 'Password', osv.

    % Kombiner Port og valfrie parameter
    args = [{'Port', port}, varargin];

    % Opprett MQTT-klient
    client = mqttclient(broker_address, args{:});

    % Informasjon
    fprintf("🔌 Kobla til broker: %s:%d\n", broker_address, port);

    % Hent IP og bruk siste byte som ID
    ip = get_local_ip();
    id = extract_id_from_ip(ip);
    assignin("base", "my_id", uint8(id));
    fprintf("📡 MQTT og ID klart: IP = %s → my_id = %d\n", ip, id);
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