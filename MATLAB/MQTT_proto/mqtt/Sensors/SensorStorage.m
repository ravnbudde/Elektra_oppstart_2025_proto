classdef SensorStorage
    methods (Static)
        function setSensor(obj)
            persistent shared
            shared = obj;
        end
        function out = getSensor()
            persistent shared
            if isempty(shared)
                warning("SensorStorage er tom! Har du kalla setSensor()?")
                out = [];
            else
                out = shared;
            end
        end
    end
end
