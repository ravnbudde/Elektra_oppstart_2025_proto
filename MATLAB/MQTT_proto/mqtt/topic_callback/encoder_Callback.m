function encoder_Callback(topic, data)
    sensorData = sensor_ref("get");

    % Forventar at data er ein komma-separert streng som "left,right"
    values = str2double(strsplit(data, ','));

    if numel(values) == 2
        sensorData.encoder.left  = values(1);
        sensorData.encoder.right = values(2);

        fprintf("[encoder] left=%d, right=%d\n", values(1), values(2));
    else
        warning("Ugyldig encoder-dataformat: '%s'", data);
    end
end
