classdef SensorData < handle
%SENSORDATA Samla datastruktur for sensorverdiar frå Zumo-bilen.
%   Denne klassen representerer siste mottatte verdiar frå ulike sensorar
%   og lagrar dei som eigenskapar i strukturar eller skalarar.
%
%   Alle eigenskapar er offentlege og kan lesast/skrivast direkte.
%
%   Innhald:
%       accel     – struct med akselerasjon i x, y, z-retning (m/s²)
%       gyro      – struct med rotasjonsfart rundt x, y, z (°/s)
%       encoder   – struct med venstre og høgre hjulposisjon (enkodarverdiar)
%       line      – linjesensor (posisjon, typisk NaN ved ingen deteksjon)
%       distance  – avstandssensor (cm eller mm, avhengig av sensor)
%       battery   – batterispenning (V)
%       temp      – temperatur (°C)
%       light     – lysnivå (vilkårleg eining, sensoravhengig)
%       mag       – magnetometer med x, y, z-komponentar (valfri)
%
%   Bruk:
%       data = SensorData();
%       data.accel.x = 3.14;
%       disp(data.encoder.right);
%
%   Sjå også: sensor_ref
    properties
        accel   = struct('x', 0, 'y', 0, 'z', 0);    % Akselerometer (3D)
        gyro    = struct('x', 0, 'y', 0, 'z', 0);    % Gyroskop (full 3D støtte)
        mag      = struct('x', 0, 'y', 0, 'z', 0);   % Magnetometer (valfri)
        encoder = struct('left', NaN, 'right', NaN);       % Hjulekodarar
        line    = NaN;                                     % Linjesensor
        distance = NaN;                                    % Avstandssensor
        battery  = NaN;                                    % Batterispenning
        temp     = NaN;                                    % Temperatur
        light    = NaN;                                    % Lysstyrke
    end
end
