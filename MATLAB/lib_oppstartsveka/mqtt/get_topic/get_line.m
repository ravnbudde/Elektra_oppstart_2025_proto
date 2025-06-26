function val = get_line()
    sensor = sensor_ref("get");
    val = sensor.line;
end
