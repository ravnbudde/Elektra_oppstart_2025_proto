function send_PID(kp, ki, kd)
    mqtt_send("pid", double(kp), double(ki), double(kd));
end
%% Simulink MATLAB function for sende alle PID-parametra saman
% function publish_PID(kp, ki, kd)
%     coder.extrinsic('send_PID');  % merk: dette peikar på .m-fila di
%     send_PID(kp, ki, kd)
% end