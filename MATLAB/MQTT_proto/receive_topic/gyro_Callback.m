function gyro_Callback(topic, data)
    sensorData = sensor_ref("get");
    sensorData.gyro = str2double(data);
    fprintf("[gyro] %s\n", data);
end
