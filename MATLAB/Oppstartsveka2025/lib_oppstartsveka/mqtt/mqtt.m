function mqtt(car_id, broker, port, bn, pw, topics)
%MQTT Wrapper for MQTT-oppstart med fleksibel argumenthandsaming.
%   MQTT(...) kallar mqtt_setup() med ulike gradar av spesifikasjon,
%   og legg automatisk til 'mqtt/'-mappa i søkjestien dersom ho ikkje er der frå før.
%
%   Funksjonen støttar 0 til 6 inputargument, og fyller inn manglande verdiar
%   med standardkonfigurasjon brukt i oppstartsveka.
%   
%  Viktig! alt som ikkje er tall må sendast inn med fnutter '<var>'.
%  Visst man skal koble til ein broker som har anna prefix enn tcp, så
%  MÅ det spesifiserast med å skrive  <prefix>://<broker-domain>.
%  - f.eks: mqtt(14,'mqtt://broker.hivemq.com')
%
%   Gyldige kall:
%       mqtt()
%       mqtt(car_id)
%       mqtt(car_id, 'broker')
%       mqtt(car_id, 'broker', 'port')
%       mqtt(car_id, 'broker', 'port', 'username', 'password')
%       mqtt(car_id, 'broker', 'port', 'username', 'password', ['topics'])
%
%   Standardverdiar brukt:
%       CAR_ID     : 14
%       BROKER     : '192.168.1.3'
%       PORT       : '1883'
%       USERNAME   : 'BIAISbroker'
%       PASSWORD   : 'shinyteapot294'
%
%   Eksempel:
%       mqtt();                          % brukar alle standardverdiar
%       mqtt(3);                         % brukar bil-ID 3, resten standard
%       mqtt(3, "test.broker.io");       % spesifiserer broker
%       mqtt(3, "broker.io", "1884", "user", "pw", ["cmd", "speed"]);
%
%   Sjå også: mqtt_setup, mqttDefault
    % MQTT setup-wrapper med fleksibel argumenthandsaming
    fullpath = mfilename('fullpath');
    [projectRoot, ~, ~] = fileparts(fullpath);
    mqttPath = fullfile(projectRoot, 'mqtt');

    if ~contains(path, mqttPath)
        addpath(mqttPath);        fprintf("📁 La til mqtt-mappe i path: %s\n", mqttPath);
    end

    % Standardverdiar
    default_car_id = 14;
    default_broker = "192.168.1.3";
    default_port   = "1883";
    default_user   = "BIAISbroker";
    default_pass   = "shinyteapot294";

    % Kall med gradvis fleire argument
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
