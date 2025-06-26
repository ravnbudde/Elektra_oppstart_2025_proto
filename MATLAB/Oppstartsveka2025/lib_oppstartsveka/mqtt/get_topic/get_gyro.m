function [gx, gy, gz] = get_gyro()
    sensor = sensor_ref("get");
    gx = sensor.gyro.x;
    gy = sensor.gyro.y;
    gz = sensor.gyro.z;
end

%% Simulink MATLAB function
% function [gx, gy, gz] = read_gyro()
%     coder.extrinsic('get_gyro');
%     gx = 0; gy = 0; gz = 0;
%     [gx, gy, gz] = get_gyro();
% end
