function accel_z_Callback(topic, data)
    sensorData = SensorStorage.getSensor();
    sensorData.accel.z = str2double(data);
    fprintf("[accel.z] %s\n", data);
end
