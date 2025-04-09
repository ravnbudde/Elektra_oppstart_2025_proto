% Initialiser globale strukturar
global accel encoder gyro line;
accel = struct('x', NaN, 'y', NaN, 'z', NaN);
encoder = struct('left', NaN, 'right', NaN);
gyro = NaN;
line = NaN;

% Liste over topic-suffixar og tilhøyrande callbacks
topics_callbacks = {
    "sensors/accel/x",     @accel_x_Callback;
    "sensors/accel/y",     @accel_y_Callback;
    "sensors/accel/z",     @accel_z_Callback;
    "sensors/encoder/left",  @encoder_left_Callback;
    "sensors/encoder/right", @encoder_right_Callback;
    "sensors/gyro",        @gyro_Callback;
    "sensors/line",        @line_Callback;
};

% Abonner på alle
for i = 1:size(topics_callbacks, 1)
    topic = "zumo_car/14/" + topics_callbacks{i, 1};
    callback = topics_callbacks{i, 2};
    subscribe(mqttClient, topic, 'Callback', callback);
    fprintf("✅ Abonnerte på: %s\n", topic);
end

disp(mqttClient.Subscriptions);

% Hald skriptet køyrande
disp('📡 Lyttar etter sensorverdier. Trykk Ctrl+C for å avslutte.');
while true
    pause(1);
end
