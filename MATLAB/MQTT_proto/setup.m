function setup(car_id)
global basetopic;
    % Vis ID-en til bilen vi kontrollerer
    fprintf("🚗 Setter opp system med car_id = %d\n", car_id);
    % Lagre car_id i base workspace for tilgang seinare
    assignin("base", "car_id", uint8(car_id));
    % Generer basetopic og lagre
    basetopic = sprintf("zumo_car/%d", car_id);
    assignin("base", "basetopic", basetopic);
    fprintf("🔗 base topic satt til: %s\n", basetopic);
    
    % Få katalogen der dette skriptet ligg
    fullpath = mfilename('fullpath');
    [projectRoot, ~, ~] = fileparts(fullpath);

    % MQTT-mappe
    mqttPath = fullfile(projectRoot, 'mqtt');
    % Legg til i path viss ikkje allereie der
    if ~contains(path, mqttPath)
        addpath(mqttPath);
        savepath;
    end
    sendPath = fullfile(projectRoot, 'send_topic');
    if ~contains(path, sendPath)
        addpath(sendPath);
        savepath;
    end
    receivePath = fullfile(projectRoot, 'receive_topic');
    if ~contains(path, receivePath)
        addpath(receivePath);
        savepath;
    end
    
    % Kjør MQTT-init
    try
        mqtt_init();
        disp("✅ mqtt_init fullført");
    catch E
        warning("❌ Feil under mqtt_init: %s", E.message);
    end

    
    % Initialiser globale strukturar
    global accel encoder gyro line;
    accel = struct('x', NaN, 'y', NaN, 'z', NaN);
    encoder = struct('left', NaN, 'right', NaN);
    gyro = NaN;
    line = NaN;
    % Start subscriber
    pause(2);
    % f = parfeval(backgroundPool, @mqtt_subscribe, 0);
    parfeval(@mqtt_subscribe, 0);


end

function mqtt_init()
global mqttClient;
    % MQTT-klientoppsett
    client = mqttclient("tcp://192.168.1.3", ...
        Port=1883, ...
        Username="BIAISbroker", ...
        Password="shinyteapot294");

    % Lagre klient i base workspace
    assignin("base", "mqttClient", client);

    % Hent IP og bruk siste byte som ID
    ip = get_local_ip();
    id = extract_id_from_ip(ip);
    assignin("base", "my_id", uint8(id));

    % Informasjon
    fprintf("📡 MQTT og ID klart:\n");
    fprintf("   🔌 IP: %s → my_id: %d\n", ip, id);
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
