% Parameters
rows = 10; % Adjust to your grid size
cols = 15; % Adjust to your grid size
latticeSpacing = 1; % Distance between hexagonal nodes
angles = [0, pi/3, 2*pi/3, pi, 4*pi/3, 5*pi/3]; % Directions in radians

% Generate Hexagonal Grid
x = [];
y = [];
for i = 0:rows-1
    for j = 0:cols-1
        xPos = j * latticeSpacing + (mod(i, 2) * latticeSpacing / 2);
        yPos = i * (sqrt(3)/2) * latticeSpacing;
        x = [x, xPos];
        y = [y, yPos];
    end
end

% Animation loop for timesteps
numTimesteps = 30; % Total number of timesteps
for t = 0:numTimesteps-1
    % Load configuration file
    filename = sprintf('config_timestep_%d.txt', t);
    fileID = fopen(filename, 'r');
    rawData = textscan(fileID, '%s %d %d %d %d %d %d %d', 'Delimiter', ':');
    fclose(fileID);

    % Extract data
    directions = cell2mat(rawData(2:end)); % 6 direction states
    indices = rawData{1}; % Node indices as strings

    % Map indices to positions
    positions = sscanf([indices{:}], '(%d,%d)', [2, Inf])';
    xPositions = positions(:, 2); % Columns
    yPositions = positions(:, 1); % Rows

    % Plot the lattice
    figure(1);
    clf;
    scatter(x, y, 'k'); % Hexagonal nodes
    hold on;

    % Plot particle directions as arrows
    for k = 1:size(positions, 1)
        for d = 1:6
            if directions(k, d) == 1
                dx = cos(angles(d)) * 0.5; % Arrow length scaling
                dy = sin(angles(d)) * 0.5;
                quiver(x(xPositions(k)), y(yPositions(k)), dx, dy, 0, 'r'); % Disable scaling
            end
        end
    end

    axis equal;
    title(sprintf('FHP-1 Simulation - Timestep %d', t));
    xlabel('X');
    ylabel('Y');
    pause(0.1); % Adjust pause duration for animation speed
end
