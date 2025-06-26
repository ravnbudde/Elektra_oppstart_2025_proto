function mag_Callback(topic, data)
    sensorData = sensor_ref("get");

    % Forventar at data er ein komma-separert streng som "x,y,z"
    values = str2double(strsplit(data, ','));

    if numel(values) == 3
        sensorData.mag.x = values(1);
        sensorData.mag.y = values(2);
        sensorData.mag.z = values(3);

        fprintf("[mag.xyz] x=%f, y=%f, z=%f\n", values(1), values(2), values(3));
    else
        warning("Ugyldig gyro-dataformat: '%s'", data);
    end
end
