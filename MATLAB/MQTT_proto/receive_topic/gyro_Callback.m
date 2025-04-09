function gyro_Callback(topic, data)
    global gyro;
    gyro = str2double(data);
    fprintf("[gyro] %s\n", data);
end
