% MATLAB script to visualize the hexagonal grid states

clc;
clear;
close all;

% Parameters
M = 30; % Number of rows
N = 10; % Number of columns
DIRECTIONS = 6; % Number of directions in the hexagonal grid
TIMESTEPS = 100; % Total timesteps to visualize

% Hexagonal grid offsets for plotting
theta = (0:5) * pi / 3; % Angles for hexagon vertices
hex_x = cos(theta);
hex_y = sin(theta);

% Load and visualize each timestep
for timestep = 0:TIMESTEPS-1
    % Load the configuration file
    filename = sprintf("config_t%d.txt", timestep);
    if ~isfile(filename)
        warning("File %s does not exist. Skipping...", filename);
        continue;
    end
    data = readlines(filename);

    % Initialize the figure
    figure(1);
    clf;
    hold on;
    axis equal;
    title(sprintf("Hexagonal Grid at Timestep %d", timestep));
    xlabel("Columns");
    ylabel("Rows");

    % Process the grid data
    for i = 1:numel(data)
        line = data(i);
        if line == ""
            continue;
        end

        % Extract the row, column, and state
        tokens = regexp(line, "(\d+),(\d+): (.*)", "tokens");
        if isempty(tokens)
            continue;
        end

        tokens = tokens{1};
        row = str2double(tokens{1});
        col = str2double(tokens{2});
        state = str2num(tokens{3}); %#ok<ST2NM> % Extract state as array

        % Convert grid coordinates to hexagonal positions
        x_center = col * 1.5;
        y_center = row * sqrt(3);

        % Adjust hexagon position for staggered rows
        if mod(row, 2) == 1
            x_center = x_center + 0.75;
        end

        % Draw the hexagon
        fill(hex_x + x_center, hex_y + y_center, [0.9 0.9 0.9], 'EdgeColor', 'k');

        % Add arrows for each direction
        for d = 1:DIRECTIONS
            if state(d) == 1
                arrow_x = [x_center, x_center + 0.3 * cos((d-1) * pi / 3)];
                arrow_y = [y_center, y_center + 0.3 * sin((d-1) * pi / 3)];
                plot(arrow_x, arrow_y, 'r-', 'LineWidth', 1.5);
            end
        end
    end

    % Configure plot settings
    axis off;
    drawnow;

    % Pause for visualization
    pause(0.1);
end

disp("Visualization complete.");
