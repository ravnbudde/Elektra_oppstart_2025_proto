function mqttBasic(car_id)

    broker = '192.168.1.3';
    port   = '1883';
    user = 'BIAISbroker';
    pass = 'shinyteapot294';
    
    mqtt(car_id, broker, port, user, pass);
end