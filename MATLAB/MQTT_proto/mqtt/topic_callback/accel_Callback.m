function accel_Callback(topic, data)
    sensorData = sensor_ref("get");
    
    % Forventar at data er ein komma-separert streng som "x,y,z"
    values = str2double(strsplit(data, ','));
    
    if numel(values) == 3
        sensorData.accel.x = values(1);
        sensorData.accel.y = values(2);
        sensorData.accel.z = values(3);
        
        fprintf("[accel.xyz] x=%f, y=%f, z=%f\n", values(1), values(2), values(3));
    else
        warning("Ugyldig dataformat: '%s'", data);
    end
end
