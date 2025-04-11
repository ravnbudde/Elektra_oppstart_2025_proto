function [left, right] = get_encoder()
    sensor = sensor_ref("get");
    left  = sensor.encoder.left;
    right = sensor.encoder.right;
end
