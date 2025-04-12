%%READ stuff
function publish_PID(Kp, Ki, Kd)
    coder.extrinsic('send_PID');  

    persistent last_Kp last_Ki last_Kd

    if isempty(last_Kp)
        last_Kp = NaN; % sørg for første utsending
        last_Ki = NaN;
        last_Kd = NaN;
    end

    % Sjekk om noko har endra seg
    if Kp ~= last_Kp || Ki ~= last_Ki || Kd ~= last_Kd
        send_PID(Kp, Ki, Kd); % send berre om noko endra seg
        last_Kp = Kp;
        last_Ki = Ki;
        last_Kd = Kd;
    end
end

function publish_speed(right, left)
    coder.extrinsic('send_speed');  % kallar på ekstern MATLAB-funksjon

    persistent last_right last_left

    if isempty(last_right)
        last_right = NaN;
        last_left  = NaN;
    end

    if right ~= last_right || left ~= last_left
        send_speed(right, left); % berre send om verdiar har endra seg
        last_right = right;
        last_left  = left;
    end
end

function publish_cmd(cmd)
    coder.extrinsic('send_cmd');

    persistent last_cmd
    if isempty(last_cmd)
        last_cmd = blanks(20);  % 1x20 char-array
    end

    % Konverter input til char
    cmd_char = char(cmd);
    L = length(cmd_char);

    % Sørg for at cmd_padded er ALLTID 1x20
    cmd_padded = blanks(20);      % Alloker fast lengde
    N = min(L, 20);               % maks 20 teikn
    cmd_padded(1:N) = cmd_char(1:N);  % kopier teikna

    % Samanlikn og send berre ved endring
    if ~strcmp(cmd_padded, last_cmd)
        send_cmd(strtrim(cmd_padded));  % send utan whitespace
        last_cmd = cmd_padded;          % lagre nytt for samanlikning
    end
end

%lag denne
function is_pen = publish_penalty(value, trigger_value)
    coder.extrinsic('send_penalty');

    persistent last_cmd
    if isempty(last_cmd)
        last_cmd = 0;  % tom int
    end

    

    if value > trigger_value
        is_triggered = true;
        cmd = 1;
    else
        is_triggered = false;
        cmd = 0;
    end

    if cmd ~= last_cmd
        send_penalty(cmd);
        last_cmd = cmd;
    end
end



%% Recive stuff

function [x, y, z] = read_accel()
    coder.extrinsic('get_accel');
    x = 0; y = 0; z = 0;
    [x, y, z] = get_accel();
end

function accel = read_accel()
    coder.extrinsic('get_accel');
    accel = zeros(1,3); 
    [x, y, z] = get_accel();
    accel = [x, y, z];
end

function [gx, gy, gz] = read_gyro()
    coder.extrinsic('get_gyro');
    gx = 0; gy = 0; gz = 0;
    [gx, gy, gz] = get_gyro();
end


function encoder = read_encoder()
    coder.extrinsic('get_encoder');
    encoder = zeros(1,2); % Forhåndsdefiner for Simulink sin del
    [left, right] = get_encoder();
    encoder = [left, right];
end

function [left, right]  = read_encoder()
    coder.extrinsic('get_encoder');
    left = 0; right = 0;
    [left, right] = get_encoder();
end

function gyro = read_gyro_vec()
    coder.extrinsic('get_gyro');
    gyro = zeros(1,3); % pre-alloc
    [x, y, z] = get_gyro();
    gyro = [x, y, z];
end
