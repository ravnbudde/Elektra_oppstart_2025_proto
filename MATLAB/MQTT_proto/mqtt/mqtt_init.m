client = mqttclient("tcp://192.168.1.3", ...
    Port=1883, ...
    Username="BIAISbroker", ...
    Password="shinyteapot294");

assignin("base", "mqttClient", client);
%% TODO:: oprett topics og
% ---- Definer topic som byte-array ----
assignin("base", "topic_test", uint8('test/topic'));
assignin("base", "topic_sensor", uint8('sensor/data'));
assignin("base", "topic_actuator", uint8('actuator/cmd'));

