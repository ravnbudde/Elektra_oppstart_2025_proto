function val = get_accel_z()
    sensor = sensor_ref("get");
    val = sensor.accel.z;
end

%% Simulink MATLAB function
% function val = read_accel_z()
%     coder.extrinsic('get_accel_z');
%     val = 0;
%     val = get_accel_z();
% end