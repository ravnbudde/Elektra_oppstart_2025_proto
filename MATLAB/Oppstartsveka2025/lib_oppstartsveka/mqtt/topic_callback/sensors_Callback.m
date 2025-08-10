function sensors_Callback(topic, data)
    sensorData = sensor_ref("get");
    
    % Forventar at data er ein komma-separert streng som "x,y,z"
    values = str2double(strsplit(data, ','));
    
    if numel(values) == 10
        sensorData.gyro.x   = values(1);
        sensorData.gyro.y   = values(2);
        sensorData.gyro.z   = values(3);
        sensorData.accel.x  = values(4);
        sensorData.accel.y  = values(5);
        sensorData.accel.z  = values(6);
        sensorData.mag.x    = values(7);
        sensorData.mag.y    = values(8);
        sensorData.mag.z    = values(9);
        sensorData.line     = values(10);
    else
        warning("Ugyldig dataformat: '%s'", data);
    end
end
