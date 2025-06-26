function encoder_left_Callback(topic, data)
    sensorData = sensor_ref("get");
    sensorData.encoder.left = str2double(data);
    fprintf("[encoder.left] %s\n", data);
end
