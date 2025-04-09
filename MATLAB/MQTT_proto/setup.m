function setup(car_id)
    global basetopic
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
    getPath = fullfile(projectRoot, 'get_topic');
    if ~contains(path, getPath)
        addpath(getPath);
        savepath;
    end
    receivePath = fullfile(projectRoot, 'receive_topic');
    if ~contains(path, receivePath)
        addpath(receivePath);
        savepath;
    end
    sensorPath = fullfile(projectRoot, 'Sensors');
    if ~contains(path, sensorPath)
        addpath(sensorPath);
        savepath;
    end
    sensorData = SensorData();
    sensor_ref("set", sensorData);
    disp(sensor_ref("get"));
    
    % lag ein mqtt client som skal brukast til subscribe (callback) og
    % sending
    client = mqtt_init();  % Bruk funksjonen din her, som ønskja
    assignin("base", "mqttClient", client);
    % Start subscriber
    pause(2);
    
    mqtt_subscribe();
    send_angle(69);
    
    % sim("mqtt_simulink.slx")
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
