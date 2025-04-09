function mqtt_receiver_sfun(block)
    setup(block);
end

function setup(block)
    block.NumInputPorts  = 1;
    block.NumOutputPorts = 1;

    block.SetPreCompInpPortInfoToDynamic;
    block.SetPreCompOutPortInfoToDynamic;

    block.InputPort(1).DatatypeID = 10; % string
    block.OutputPort(1).DatatypeID = 0; % double

    block.SampleTimes = [-1 0];

    block.SimStateCompliance = 'DefaultSimState';

    block.RegBlockMethod('Outputs', @Outputs);
end

function Outputs(block)
    topic = block.InputPort(1).Data;
    value = mqtt_receive(topic);
    block.OutputPort(1).Data = value;
end
