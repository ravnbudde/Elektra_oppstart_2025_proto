classdef SensorStorage
%SENSORSTORAGE Statisk lagring og henting av SensorData-instans.
%   SENSORSTORAGE gir tilgang til ein delt SensorData-instans gjennom
%   to statiske metodar:
%
%       SensorStorage.setSensor(obj)  – lagrar eit SensorData-objekt globalt
%       SensorStorage.getSensor()     – returnerer det lagra objektet
%
%   Bruk:
%       SensorStorage.setSensor(SensorData());
%       data = SensorStorage.getSensor();
%
%   Merk:
%       Dersom getSensor() blir kalla før setSensor(), returnerer den []
%       og gir ei åtvaring i konsollen.
%
%   Typisk brukt for å dele sensordata globalt mellom funksjonar
%   utan å bruke `assignin` eller `global`.
%
%   Sjå også: SensorData

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
