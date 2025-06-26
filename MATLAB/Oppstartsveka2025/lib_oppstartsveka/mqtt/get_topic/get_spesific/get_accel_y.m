function val = get_accel_y()
    sensor = sensor_ref("get");
    val = sensor.accel.y;
end
%% Simulink MATLAB function
% function val = read_accel_y()
%     coder.extrinsic('get_accel_y');  % merk: dette peikar på .m-fila di
%     val = 0;                         % initier output
%     val = get_accel_y();            % kall den eksterne funksjonen
% end
