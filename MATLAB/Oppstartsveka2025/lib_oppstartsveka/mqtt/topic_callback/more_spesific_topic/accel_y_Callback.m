function accel_y_Callback(topic, data)
    sensorData = sensor_ref("get");
    sensorData.accel.y = str2double(data);
    fprintf("[accel.y] %s\n", data);
end
