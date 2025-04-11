function mqtt(car_id, broker, port, bn, pw, topics)
    % MQTT setup-wrapper med fleksibel argumenthandsaming
    fullpath = mfilename('fullpath');
    [projectRoot, ~, ~] = fileparts(fullpath);
    mqttPath = fullfile(projectRoot, 'mqtt');

    if ~contains(path, mqttPath)
        addpath(mqttPath);
        fprintf("📁 La til mqtt-mappe i path: %s\n", mqttPath);
    end

    % 📦 Standardverdiar
    default_car_id = 14;
    default_broker = "192.168.1.3";
    default_port   = "1883";
    default_user   = "BIAISbroker";
    default_pass   = "shinyteapot294";

    % 🔁 Kall med gradvis fleire argument
    switch nargin
        case 0
            mqtt_setup(default_car_id, default_broker, default_port, default_user, default_pass);
        case 1
            mqtt_setup(car_id, default_broker, default_port, default_user, default_pass);
        case 2
            mqtt_setup(car_id, broker, default_port, default_user, default_pass);
        case 3
            mqtt_setup(car_id, broker, port, default_user, default_pass);
        case 5
            mqtt_setup(car_id, broker, port, bn, pw); % utan topics
        otherwise
            mqtt_setup(car_id, broker, port, bn, pw, topics);
    end
end
