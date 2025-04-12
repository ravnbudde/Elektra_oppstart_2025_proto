
% --- Skaleringsfaktorar (til SI-einingar)
gyro_scale = pi / (180 * 1000);   % mdps til rad/s
accel_scale = 9.81 / 16384;       % råverdi til m/s^2 (typisk for ±2g IMU)

% --- Initiering
pos_history = zeros(size(accel_data, 1), 2);

for i = 1:size(accel_data, 1)
    gyro = gyro_data(i, :)' * gyro_scale;
    accel = accel_data(i, :)' * accel_scale;

    [x, y] = beregn_posisjon(gyro, accel);
    pos_history(i, :) = [x, y];

    fprintf('Steg %2d → x = %.3f m, y = %.3f m\n', i, x, y);
    pause(0.05);  % simuler sample rate
end

% Valfritt: Plot posisjonshistorikk
figure;
plot(pos_history(:,1), pos_history(:,2), '-o');
xlabel('x [m]');
ylabel('y [m]');
title('Estimert 2D-posisjon frå IMU');
axis equal;
grid on;

end

function [x, y] = beregn_posisjon(gyro, accel)
% gyro og accel må vere 3x1 vektorar
gyro = gyro(:);
accel = accel(:);

% Interne variablar
persistent yaw vx vy x_pos y_pos t_prev

if isempty(yaw)
    yaw = 0;
    vx = 0; vy = 0;
    x_pos = 0; y_pos = 0;
    t_prev = tic;
end

% Tid
dt = toc(t_prev);
t_prev = tic;

% Integrer yaw frå gz
yaw = yaw + gyro(3) * dt;

% Anta at accel(1) er aks i framoverretning (x i kroppsrama)
a_forward = accel(1);

% Roter akselerasjonen til verdsramma
a_x = a_forward * cos(yaw);
a_y = a_forward * sin(yaw);

% Integrer aks → fart
vx = vx + a_x * dt;
vy = vy + a_y * dt;

% Integrer fart → posisjon
x_pos = x_pos + vx * dt;
y_pos = y_pos + vy * dt;

% Returner
x = x_pos;
y = y_pos;
end