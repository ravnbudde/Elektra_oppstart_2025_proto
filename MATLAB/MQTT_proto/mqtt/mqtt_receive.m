function value = mqtt_receive(topic_suffix)
    persistent client subscribed_topics
    if isempty(client) || ~client.Connected
        client = evalin("base", "mqttClient");
        subscribed_topics = strings(0);  % bruk string-array, ikkje cell
    end

    basetopic = evalin("base", "basetopic");
    full_topic = basetopic + "/" + topic_suffix;  % string + string = string

    % Abonner viss ikkje abonnert frå før
    if ~any(subscribed_topics == full_topic)
        subscribe(client, char(full_topic));
        subscribed_topics(end+1) = full_topic;
    end

    value = NaN;  % fallback dersom ingen melding kjem

    try
        data = read(client, 'Topic', full_topic);

        if ~isempty(data)
            value = str2double(data.Payload{end});
        end
    catch err
        warning("❌ Feil ved lesing frå '%s': %s", full_topic, err.message);
    end
end
