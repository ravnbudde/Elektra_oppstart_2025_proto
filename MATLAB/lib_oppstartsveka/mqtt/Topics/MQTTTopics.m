classdef MQTTTopics
%MQTTTOPICS Enum-type for gyldige MQTT-topic-suffixar brukt i Zumo-prosjektet.
%   Klassen definerer ein strengbasert enumerasjon over standard MQTT-topicar
%   som bilen kan abonnere på eller sende til.
%
%   Verdiane inkluderer:
%       - accel    : akselerometer-data
%       - gyro     : gyroskop-data
%       - mag      : magnetometer
%       - line     : linjesensor
%       - encoder  : hjul-enkoder
%
%   Bruk:
%       all = enumeration("MQTTTopics");     % gir alle verdiar som enum-objekt
%       topicStr = char(MQTTTopics.accel);   % returnerer "accel" som streng
%
%   Sjå også: mqtt_subscribe, mqtt_send

    enumeration
        accel    ('accel')
        gyro     ('gyro')
        mag      ('mag')
        line     ('line')
        encoder  ('encoder')
    end

    properties
        topic (1,1) string
    end

    methods
        function obj = MQTTTopics(val)
            obj.topic = val;
        end
        function out = char(obj)
            out = char(obj.topic);
        end
    end
end
