function [mx, my, mz] = get_mag()
    sensor = sensor_ref("get");
    mx = sensor.mag.x;
    my = sensor.mag.y;
    mz = sensor.mag.z;
end