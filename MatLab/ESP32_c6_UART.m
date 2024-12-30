serialObj = serialport("COM12", 115200); % Endre COM-port etter behov
fig = figure; % Opprett ein ny figur
pax = polaraxes(fig); % Opprett polarakse i figuren
hold(pax, 'on'); % Aktiver 'hold' på polaraksen

while true
    rawData = read(serialObj, 2, "uint8"); % Les 2 bytes
    if numel(rawData) == 2
        combined_data = bitor(bitshift(uint16(rawData(1)), 8), uint16(rawData(2)));
        angle = double(bitshift(combined_data, -8)) * (360 / 255);
        distance = double(bitand(combined_data, 255)) * (100 / 255);
        polarplot(pax, deg2rad(angle), distance, 'o'); % Bruk den spesifikke aksen
        drawnow;
    end
end
