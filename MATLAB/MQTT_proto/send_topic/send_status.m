function send_status(value)
    mqtt_send('status', value);
end

%%
% function publish_anle(value)
%     coder.extrinsic('send_angle');  % merk: dette peikar på .m-fila di
%     send_angle(value)
% end
