function encoder_right_Callback(topic, data)
    sensorData = SensorStorage.getSensor();
    sensorData.encoder.right = str2double(data);
    fprintf("[encoder.right] %s\n", data);
end
