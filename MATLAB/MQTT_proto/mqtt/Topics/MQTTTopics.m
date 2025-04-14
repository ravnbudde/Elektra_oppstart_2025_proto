classdef MQTTTopics
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
