function accel_z_Callback(topic, data)
    global accel;
    accel.z = str2double(data);
    fprintf("[accel.z] %s\n", data);
end
