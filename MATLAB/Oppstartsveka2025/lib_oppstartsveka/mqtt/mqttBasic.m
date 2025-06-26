function mqttBasic(car_id)
%MQTTBASIC Standard MQTT-oppsett for oppstartsveka med lokal broker.
%   MQTTBASIC(CAR_ID) set opp MQTT-kommunikasjon med førehandsdefinert broker
%   og autentisering tilpassa Raspberry Pi-baserte system brukt under oppstartsveka.
%
%   Parameter:
%       CAR_ID – heiltal som identifiserer bilen (brukast i topicar og ID)
%
%   Bruker fast oppsett:
%       Broker IP : 192.168.1.3  
%       Port      : 1883  
%       Brukarnamn: 'BIAISbroker'  
%       Passord   : 'shinyteapot294'

    broker = '192.168.1.3';
    port   = '1883';
    user = 'BIAISbroker';
    pass = 'shinyteapot294';
    
    mqtt(car_id, broker, port, user, pass);
end