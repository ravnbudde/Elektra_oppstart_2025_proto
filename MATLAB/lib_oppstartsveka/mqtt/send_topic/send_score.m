function send_score(task, value)
    group_id = evalin("base", "group_id");
    topic = "score/del" + task +"/" + group_id;
    mqtt_send(topic, value);
end

%%
% function publish_score(task, value)
%     coder.extrinsic('send_angle');  % merk: dette peikar på .m-fila di
%     send_angle(value)
% end
