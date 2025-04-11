function accel_x_Callback(topic, data)
    sensorData = sensor_ref("get");
    sensorData.accel.x = str2double(data);
    fprintf("[accel.x] %s\n", data);
end