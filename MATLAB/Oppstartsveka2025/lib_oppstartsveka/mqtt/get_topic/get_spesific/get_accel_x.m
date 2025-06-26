function val = get_accel_x()
    sensor = sensor_ref("get");
    val = sensor.accel.x;
end
%% Simulink MATLAB function
% function val = read_accel_x()
%     coder.extrinsic('get_accel_x');
%     val = 0;
%     val = get_accel_x();
% end