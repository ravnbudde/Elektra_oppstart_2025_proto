function accel_x_Callback(topic, data)
    sensorData = sensor_ref("get");
    sensorData.accel.x = str2double(data);
    fprintf("[accel.x] %s\n", data);
    assignin("base", "accel_x_temp", str2double(data));  % oppdater workspace-variabel

    % Oppdater Data Store Memory-blokk
    evalin("base", "accel_x = accel_x_temp;");
end