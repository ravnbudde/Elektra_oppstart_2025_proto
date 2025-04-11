function [x, y, z] = get_accel()
    sensor = sensor_ref("get");
    x = sensor.accel.x;
    y = sensor.accel.y;
    z = sensor.accel.z;
end
% Simulink MATLAB function
% function [x, y, z] = read_accel_xyz()
%     coder.extrinsic('get_accel_xyz');
%     x = 0; y = 0; z = 0;
%     [x, y, z] = get_accel_xyz();
% end
