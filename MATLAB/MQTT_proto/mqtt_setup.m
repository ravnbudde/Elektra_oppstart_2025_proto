function mqtt_setup(car_id, broker, port, bn, pw, topics)
    % Oppsett for systemet med MQTT-integrasjon
    % kall med mqtt_setup(14, "192.168.1.3", "1883", "BIAISbroker",  "shinyteapot294")
    if nargin < 2
        error("Du må minst spesifisere car_id og broker ('localhost', IP eller URL).");
    end

    % 🌟 1. Set opp identitet og topic
    fprintf("🚗 Setter opp system med car_id = %d\n", car_id);
    assignin("base", "car_id", uint8(car_id));

    basetopic = sprintf("zumo_car/%d", car_id);
    assignin("base", "basetopic", basetopic);
    fprintf("🔗 base topic satt til: %s\n", basetopic);

    % 2. Legg relevante mapper til path
    projectRoot = fileparts(mfilename('fullpath'));
    addpath(genpath(projectRoot));
    fprintf("📁 Alle undermapper i %s lagt til.\n", projectRoot);

    % 3. Init SensorData og referanse
    sensorData = SensorData();
    sensor_ref("set", sensorData);
    disp(sensor_ref("get"));

    % 🔌 4. MQTT-klient
    % Legg til "tcp://" om ingen protokoll er spesifisert
    if ~contains(broker, "://")
        broker = "tcp://" + broker;
    end

    if nargin < 4
        client = mqtt_initClient(broker, str2double(port));
    else
        client = mqtt_initClient(broker, str2double(port), ...
            "Username", bn, "Password", pw);
    end
    assignin("base", "mqttClient", client);


    % 5. Abonner på relevante sensor-topicar
    % Hent topics: argument > fil > default
    if nargin >= 6 && ~isempty(topics)
        % Bruk argument direkte
        topics_final = string(topics);
        fprintf("📥 Brukar topicar frå argument.\n");
    else
        topicsFile = fullfile(projectRoot, "mqtt\Topics\topics.csv");
        if isfile(topicsFile)
            raw = readlines(topicsFile);
            topics_final = strtrim(raw(raw ~= ""));
            fprintf("📄 Lasta %d topicar frå %s\n", numel(topics_final), topicsFile);
        else
            topics_final = [
                "accel", "accel/x", "accel/y", "accel/z", ...
                "gyro", "gyro/z", ...
                "line", ...
                "encoder/left", "encoder/right"
            ];
            fprintf("📦 Brukar innebygde fallback-topicliste.\n");
        end
    end

    mqtt_subscribe(basetopic,topics_final);

end
