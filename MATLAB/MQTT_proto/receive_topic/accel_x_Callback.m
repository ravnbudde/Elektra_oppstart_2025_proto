function accel_x_Callback(topic, data)
    global accel;
    accel.x = str2double(data);
    fprintf("[accel.x] %s\n", data);
end
