function send_speed(leftSpeed, rightSpeed)
    mqtt_send("speed", int16(leftSpeed), int16(rightSpeed));
end
%% Simulink MATLAB function
% function publish_speed(right, left)
%     coder.extrinsic('send_speed');  
%     send_speed(right, left)
% end
