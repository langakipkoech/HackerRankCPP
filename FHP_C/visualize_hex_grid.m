% MATLAB script to 
clc;
clear;
close all;

% Parameters
M = 30; % Number of rows
N = 10; % Number of columns
DIRECTIONS = 6; % Number of directions in the hexagonal grid
start_timestep = 0; % Starting timestep
num_timesteps = 5; % Number of timesteps to compare

% Obstacle parameters
obstacle_center = [8, 4]; % Center of the obstacle (row, column)
obstacle_radius = 1; % Radius of the obstacle

% Arrow directions for the hexagonal grid
arrow_directions = [0, pi/3, 2*pi/3, pi, 4*pi/3, 5*pi/3];

% Initialize the figure
figure;
hold on;
set(gcf, 'Position', [100, 100, 1600, 900]); % Adjust figure size

% Iterate through the timesteps
for timestep = start_timestep:(start_timestep + num_timesteps - 1)
    % Load the configuration file
    filename = sprintf("config_t%d.txt", timestep);
    if ~isfile(filename)
        warning("File %s does not exist. Skipping...", filename);
        continue;
    end
    data = readlines(filename);

    % Create a subplot for the current timestep
    subplot(1, num_timesteps, timestep - start_timestep + 1);
    hold on;
    axis equal;
    title(sprintf("Timestep %d", timestep), 'FontSize', 12);
    xlabel("Columns", 'FontSize', 10);
    ylabel("Rows", 'FontSize', 10);

    % Process the grid data
    for i = 1:numel(data)
        line = strtrim(data(i));
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
        state = str2num(tokens{3}); %#ok<ST2NM>
        if isempty(state) || numel(state) < DIRECTIONS
            warning("Invalid state data at row %d, col %d. Skipping...", row, col);
            continue;
        end

        % Convert grid coordinates to hexagonal positions
        x_center = col * 1.5; % Adjust column spacing
        y_center = row * sqrt(3); % Adjust row spacing

        % Adjust arrow position for staggered rows
        if mod(row, 2) == 1
            x_center = x_center + 0.75;
        end

        % Check if the cell is inside the obstacle
        dx = row - obstacle_center(1);
        dy = col - obstacle_center(2);
        if dx^2 + dy^2 <= obstacle_radius^2
            % Mark obstacle as a black circle
            plot(x_center, y_center, 'ko', 'MarkerSize', 10, 'MarkerFaceColor', 'k');
            continue;
        end

        % Add arrows for each direction
        for d = 1:DIRECTIONS
            if state(d) == 1
                arrow_x = [x_center, x_center + 0.5 * cos(arrow_directions(d))];
                arrow_y = [y_center, y_center + 0.5 * sin(arrow_directions(d))];
                plot(arrow_x, arrow_y, 'r-', 'LineWidth', 1.5); % Red arrows for directions
            end
        end
    end

    % Configure plot settings for the subplot
    xlim([-1, N+1] * 1.5); % Full column range
    ylim([-1, M+1] * sqrt(3)); % Full row range
    axis off;
end

disp("Full grid comparison plots complete.");
