function mqtt_subscribe()
    global basetopic
    
    persistent client
    if isempty(client) || ~client.Connected
        client = evalin("base", "mqttClient");
    end

    % Liste over topic-suffixar og tilhøyrande callbacks
    topics_callbacks = {
        "accel",         @accel_Callback;
        "accel/x",       @accel_x_Callback;
        "accel/y",       @accel_y_Callback;
        "accel/z",       @accel_z_Callback;
        "encoder/left",  @encoder_left_Callback;
        "encoder/right", @encoder_right_Callback;
        "gyro",          @gyro_Callback;
        "line",          @line_Callback;
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

end
