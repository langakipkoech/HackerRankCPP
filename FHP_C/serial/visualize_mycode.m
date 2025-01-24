clc;
clear;
close all;

% Parameters
M = 40; % Number of rows
N = 15; % Number of columns
DIRECTIONS = 6; % Number of directions in the hexagonal grid
start_timestep = 0; % Starting timestep
num_timesteps = 30; % Number of timesteps to animate

% Obstacle parameters
obstacle_center = [8, 4]; % Center of the obstacle (row, column)
obstacle_radius = 1; % Radius of the obstacle

% Arrow directions for the hexagonal grid
arrow_directions = [0, pi/3, 2*pi/3, pi, 4*pi/3, 5*pi/3];

% Initialize the figure
figure;
hold on;
axis equal;
title("FHP Simulation Animation", 'FontSize', 14);
xlabel("Columns", 'FontSize', 12);
ylabel("Rows", 'FontSize', 12);
set(gca, 'XLim', [-1, N+1] * 1.5, 'YLim', [-1, M+1] * sqrt(3)); % Set axis limits
set(gca, 'Color', 'white'); % Background color

% Create animation
frames(num_timesteps) = struct('cdata', [], 'colormap', []);
for timestep = start_timestep:(start_timestep + num_timesteps - 1)
    % Clear the previous timestep's arrows
    cla;
    
    % Load the configuration file
    filename = sprintf("config_t%d.txt", timestep);
    if ~isfile(filename)
        warning("File %s does not exist. Skipping...", filename);
        continue;
    end
    data = readlines(filename);

    % Display the timestep
    title(sprintf("Timestep %d", timestep), 'FontSize', 14);

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

    % Capture the frame for animation
    frames(timestep - start_timestep + 1) = getframe(gcf);
end

% Save animation as a video
video_filename = "fhp_simulation_animation.mp4";
v = VideoWriter(video_filename, 'MPEG-4');
v.FrameRate = 2; % Adjust frame rate for speed
open(v);
writeVideo(v, frames);
close(v);

disp("Animation saved as fhp_simulation_animation.mp4");
