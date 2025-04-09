function encoder_right_Callback(topic, data)
    sensorData = sensor_ref("get");
    sensorData.encoder.right = str2double(data);
    fprintf("[encoder.right] %s\n", data);
end
