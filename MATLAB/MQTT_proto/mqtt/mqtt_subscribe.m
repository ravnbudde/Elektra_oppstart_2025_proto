function mqtt_subscribe()
    global basetopic
    persistent client  % Held på klienten mellom kall
    
    % 🔍 Opprett MQTT-klient viss naudsynt
    if isempty(client) || ~client.Connected
        client = mqtt_init();  % Bruk funksjonen din her, som ønskja
    end

    % Liste over topic-suffixar og tilhøyrande callbacks
    topics_callbacks = {
        "sensors/accel/x",       @accel_x_Callback;
        "sensors/accel/y",       @accel_y_Callback;
        "sensors/accel/z",       @accel_z_Callback;
        "sensors/encoder/left",  @encoder_left_Callback;
        "sensors/encoder/right", @encoder_right_Callback;
        "sensors/gyro",          @gyro_Callback;
        "sensors/line",          @line_Callback;
    };

    % Abonner på alle
    for i = 1:size(topics_callbacks, 1)
        topic = basetopic + "/" + topics_callbacks{i, 1};
        callback = topics_callbacks{i, 2};

        try
            subscribe(client, topic, 'Callback', callback);
            fprintf("✅ Abonnerte på: %s\n", topic);
        catch ME
            warning("❌ Feil ved abonnement på %s: %s", topic, ME.message);
        end
    end

    disp(client.Subscriptions);
    disp('📡 Lyttar etter sensorverdier.');
    while true
        pause(10);
        flush(client);
end
