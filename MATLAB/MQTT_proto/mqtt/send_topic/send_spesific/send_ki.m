function send_ki(ki)
    mqtt_send("pid/ki", int16(ki));
end
%% Simulink MATLAB function
% function publish_ki(ki)
%     coder.extrinsic('send_ki');  
%     send_ki(ki)
% end