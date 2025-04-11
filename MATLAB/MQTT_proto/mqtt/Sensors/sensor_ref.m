function varargout = sensor_ref(action, input)
    persistent sensorData

    switch action
        case "set"
            sensorData = input;
        case "get"
            varargout{1} = sensorData;
        otherwise
            error("Ugyldig aksjon: bruk 'set' eller 'get'");
    end
end
