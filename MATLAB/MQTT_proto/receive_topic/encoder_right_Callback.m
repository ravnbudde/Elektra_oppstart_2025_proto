function encoder_right_Callback(topic, data)
    global encoder;
    encoder.right = str2double(data);
    fprintf("[encoder.right] %s\n", data);
end
