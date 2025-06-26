function gyro_Callback(topic, data)
    sensorData = sensor_ref("get");

    % Forventar at data er ein komma-separert streng som "x,y,z"
    values = str2double(strsplit(data, ','));

    if numel(values) == 3
        sensorData.gyro.x = values(1);
        sensorData.gyro.y = values(2);
        sensorData.gyro.z = values(3);

        fprintf("[gyro.xyz] x=%f, y=%f, z=%f\n", values(1), values(2), values(3));
    else
        warning("Ugyldig gyro-dataformat: '%s'", data);
    end
end
