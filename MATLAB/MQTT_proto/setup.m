function setup(car_id)
    % Få katalogen der dette skriptet ligg
    fprintf("car id %d\n\n", car_id);
    fullpath = mfilename('fullpath');
    [projectRoot, ~, ~] = fileparts(fullpath);

    % MQTT-mappe (du kan legge til fleire om nødvendig)
    mqttPath = fullfile(projectRoot, 'mqtt');

    % Legg til i path viss ikkje allereie der
    if ~contains(path, mqttPath)
        addpath(mqttPath);
        savepath;
        fprintf("La til '%s' i path.\n", mqttPath);
    else
        fprintf("ℹ️ Path allereie lagt til.\n");
    end

    % Kjør init
    try
        mqtt_init();
        disp("mqtt_init fullført");
    catch E
        warning("❌ Feil under mqtt_init: %s", E.message);
    end
end

function mqtt_init()
    % MQTT-klientoppsett
    client = mqttclient("tcp://192.168.1.3", ...
        Port=1883, ...
        Username="BIAISbroker", ...
        Password="shinyteapot294");

    assignin("base", "mqttClient", client);

    % Hent IP og bruk siste byte som ID
    ip = get_local_ip();
    id = extract_id_from_ip(ip);
    assignin("base", "my_id", id);

    disp(" MQTT og ID klart:");
    disp("   IP: " + ip + " → ID: " + id);
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
