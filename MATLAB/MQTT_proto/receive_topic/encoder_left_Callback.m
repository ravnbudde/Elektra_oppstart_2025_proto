function encoder_left_Callback(topic, data)
    global encoder;
    encoder.left = str2double(data);
    fprintf("[encoder.left] %s\n", data);
end
