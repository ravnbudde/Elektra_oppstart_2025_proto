function accel_z_Callback(topic, data)
    sensorData = sensor_ref("get");
    sensorData.accel.z = str2double(data);
    fprintf("[accel.z] %s\n", data);
end
