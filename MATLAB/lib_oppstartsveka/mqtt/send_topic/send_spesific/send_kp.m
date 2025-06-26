function send_kp(kp)
    mqtt_send("pid/kp", int16(kp));
end
%% Simulink MATLAB function
% function publish_kp(kp)
%     coder.extrinsic('send_kp');  % merk: dette peikar på .m-fila di
%     send_kp(kp)
% end