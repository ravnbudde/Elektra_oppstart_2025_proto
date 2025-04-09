function mqtt_subscribe()
    global basetopic accel encoder gyro line mqttClient;

    % 🚩 Start logg
    log("📥 Startar mqtt_subscribe");

    topics_callbacks = {
        "sensors/accel/x",     @accel_x_Callback;
        "sensors/accel/y",     @accel_y_Callback;
        "sensors/accel/z",     @accel_z_Callback;
        "sensors/encoder/left",  @encoder_left_Callback;
        "sensors/encoder/right", @encoder_right_Callback;
        "sensors/gyro",        @gyro_Callback;
        "sensors/line",        @line_Callback;
    };

    for i = 1:size(topics_callbacks, 1)
        topic = basetopic + "/" + topics_callbacks{i, 1};
        callback = topics_callbacks{i, 2};
        try
            subscribe(mqttClient, topic, 'Callback', callback);
            msg = sprintf("✅ Abonnerte på: %s med callback %s", topic, func2str(callback));
            log(msg);
        catch ME
            msg = sprintf("❌ Klarte ikkje abonnere på %s: %s", topic, ME.message);
            log(msg);
        end
    end

    % Skriv alle aktive abonnement til logg
    subs = mqttClient.Subscriptions;
    if isempty(subs)
        log("⚠️ Ingen aktive abonnement etter oppsett.");
    else
        log("📋 Aktive abonnement:");
        for i = 1:height(subs)
            log(sprintf(" - %s → %s", subs.Topic(i), subs.Callback{i}));
        end
    end
    
    disp(mqttClient.Subscriptions);
    
    % Hald skriptet køyrande
    disp('📡 Lyttar etter sensorverdier. Trykk Ctrl+C for å avslutte.');
    log('📡 Lyttar etter sensorverdier...');

    % Loop for å halde prosessen i live
    while true
        pause(1);
    end
end

function log(msg)
    timestamp = datestr(now, 'yyyy-mm-dd HH:MM:SS');
    fullmsg = sprintf("[%s] %s\n", timestamp, msg);
    fid = fopen("mqtt_log.txt", "a");
    if fid ~= -1
        fprintf(fid, fullmsg);
        fclose(fid);
    end
end
