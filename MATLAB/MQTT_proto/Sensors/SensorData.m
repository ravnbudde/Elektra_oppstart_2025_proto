classdef SensorData < handle
    properties
        accel = struct('x', NaN, 'y', NaN, 'z', NaN);
        encoder = struct('left', NaN, 'right', NaN);
        gyro = NaN;
        line = NaN;
    end
end
