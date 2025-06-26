function varargout = sensor_ref(action, input)
%SENSOR_REF Global referanse til SensorData-instans via persistent variabel.
%   SENSOR_REF('set', obj) lagrar SensorData-objektet obj i ein persistent variabel.
%   SENSOR_REF('get') returnerer det lagra SensorData-objektet.
%
%   Bruk:
%       sensor_ref("set", SensorData());     % lagre objekt
%       data = sensor_ref("get");            % hente objekt
%
%   Input:
%       action – 'set' eller 'get' (streng, påkravd)
%       input  – SensorData-instans (berre ved 'set')
%
%   Output:
%       data   – returnert SensorData-instans (berre ved 'get')
%
%   Merk:
%       Denne funksjonen gir enklare og tryggare tilgang til delte
%       sensorverdiar utan å bruke globale variablar eller assignin().
%
%   Sjå også: SensorData, SensorStorage
    persistent sensorData

    switch action
        case "set"
            sensorData = input;
        case "get"
            varargout{1} = sensorData;
        otherwise
            error("Ugyldig aksjon: bruk 'set' eller 'get'");
    end
end
