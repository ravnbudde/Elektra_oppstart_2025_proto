classdef SensorData < handle
    properties
        accel   = struct('x', NaN, 'y', NaN, 'z', NaN);    % Akselerometer (3D)
        gyro    = struct('x', NaN, 'y', NaN, 'z', NaN);    % Gyroskop (full 3D støtte)
        encoder = struct('left', NaN, 'right', NaN);       % Hjulekodarar
        line    = NaN;                                     % Linjesensor
        distance = NaN;                                    % Avstandssensor
        battery  = NaN;                                    % Batterispenning
        temp     = NaN;                                    % Temperatur
        light    = NaN;                                    % Lysstyrke
        mag      = struct('x', NaN, 'y', NaN, 'z', NaN);   % Magnetometer (valfri)
    end
end
