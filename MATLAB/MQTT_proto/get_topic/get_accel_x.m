function val = get_accel_x()
    sensor = sensor_ref("get");
    val = sensor.accel.x;
end
