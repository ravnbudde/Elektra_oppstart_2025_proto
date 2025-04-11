function mqtt_setup(car_id, broker, port, bn, pw, topics)
    % 📦 Standardverdiar
    default_port   = "1883";
    default_user   = "";
    default_pass   = "";

    % 1. Verdiar basert på input
    if nargin < 3 || isempty(port)
        port = default_port;
    end
    if nargin < 4 || isempty(bn)
        bn = default_user;
    end
    if nargin < 5 || isempty(pw)
        pw = default_pass;
    end

    % 🔧 Fortset som før...
    fprintf("🚗 Setter opp system med car_id = %d\n", car_id);
    assignin("base", "car_id", uint8(car_id));

    basetopic = sprintf("zumo_car/%d", car_id);
    assignin("base", "basetopic", basetopic);
    fprintf("🔗 base topic satt til: %s\n", basetopic);

    % 2. Legg relevante mapper til path
    projectRoot = fileparts(mfilename('fullpath'));
    addpath(genpath(projectRoot));
    fprintf("📁 Alle undermapper i %s lagt til.\n", projectRoot);

    % 3. Init SensorData
    sensor_ref("set", SensorData());
    disp(sensor_ref("get"));

    % 4. Bygg broker-adresse
    if ~contains(broker, "://")
        broker = "tcp://" + broker;
    end

    % 5. Opprett MQTT-klient
    if bn == ""
        client = mqtt_initClient(broker, str2double(port));
    else
        client = mqtt_initClient(broker, str2double(port), "Username", bn, "Password", pw);
    end
    assignin("base", "mqttClient", client);

    % 6. Hent topicar
    if nargin < 6 || isempty(topics)
        allTopics = enumeration("MQTTTopics");
        topics_final = string(arrayfun(@char, allTopics, "UniformOutput", false));
        fprintf("📚 Brukar %d topicar frå enum.\n", numel(topics_final));
    else
        topics_final = string(topics);
        fprintf("📥 Brukar %d topicar frå input.\n", numel(topics_final));
    end

    mqtt_subscribe(basetopic, topics_final);
end
