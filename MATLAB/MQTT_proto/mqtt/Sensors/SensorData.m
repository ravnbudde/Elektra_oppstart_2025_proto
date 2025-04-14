classdef SensorData < handle
    properties
        accel   = struct('x', 0, 'y', 0, 'z', 0);    % Akselerometer (3D)
        gyro    = struct('x', 0, 'y', 0, 'z', 0);    % Gyroskop (full 3D støtte)
        encoder = struct('left', NaN, 'right', NaN);       % Hjulekodarar
        line    = NaN;                                     % Linjesensor
        distance = NaN;                                    % Avstandssensor
        battery  = NaN;                                    % Batterispenning
        temp     = NaN;                                    % Temperatur
        light    = NaN;                                    % Lysstyrke
        mag      = struct('x', 0, 'y', 0, 'z', 0);   % Magnetometer (valfri)
    end
end
