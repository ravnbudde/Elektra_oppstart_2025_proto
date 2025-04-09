function mqtt_sender_sfun(block)
    setup(block);
end

function setup(block)
    block.NumInputPorts  = 2;
    block.NumOutputPorts = 0;

    block.SetPreCompInpPortInfoToDynamic;

    block.InputPort(1).DatatypeID = 3;  % uint8 → topic som byte-array
    block.InputPort(1).Dimensions = [1 64]; % fast lengde (t.d. maks 64 byte)
    
    block.InputPort(2).DatatypeID = 0;  % double → verdi

    block.SampleTimes = [-1 0];
    block.SimStateCompliance = 'DefaultSimState';

    block.RegBlockMethod('Outputs', @Outputs);
end

function Outputs(block)
    % Konverter topic (byte-array) til tekst
    topic_bytes = block.InputPort(1).Data;
    topic = native2unicode(topic_bytes(topic_bytes > 0))'; % fjern nullar og lag string

    value = block.InputPort(2).Data;
    mqtt_send(topic, value);
end
