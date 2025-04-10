function send_kd(kd)
    mqtt_send("pid/kd", int16(kd));
end
%% Simulink MATLAB function
% function publish_kd(kd)
%     coder.extrinsic('send_kd');  
%     send_kd(kd)
% end