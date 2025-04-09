function val = get_gyro()
    sensor = sensor_ref("get");
    val = sensor.gyro;
end

%% Simulink MATLAB function
% function val = read_gyro()
%     coder.extrinsic('get_gyro');
%     val = 0;
%     val = get_gyro();
% end
