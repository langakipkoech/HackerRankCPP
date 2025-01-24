% MATLAB Script to visualize lattice gas configurations
clc;
clear;
close all;

% Parameters
M = 30; % Number of rows
N = 10; % Number of columns
DIRECTIONS = 6; % Number of directions in the hexagonal grid
total_timesteps = 100; % Total number of timesteps
num_processes = 4; % Number of parallel processes (adjust based on MPI program)
arrow_directions = [0, pi/3, 2*pi/3, pi, 4*pi/3, 5*pi/3]; % Directions in radians

% Create figure
figure;
set(gcf, 'Position', [100, 100, 1600, 900]);
colormap(gray);

% Loop over timesteps
for timestep = 0:total_timesteps - 1
    clf; % Clear the figure for each timestep
    hold on;

    % Combine data from all processes
    full_grid = zeros(M, N, DIRECTIONS);
    for rank = 0:num_processes - 1
        filename = sprintf("config_timestep_%d_rank_%d.txt", timestep, rank);
        if ~isfile(filename)
            warning("File %s does not exist. Skipping...", filename);
            continue;
        end
        file_data = readlines(filename);

        for line = 1:numel(file_data)
            text_line = strtrim(file_data(line));
            if isempty(text_line)
                continue;
            end
            % Extract coordinates and states
            tokens = regexp(text_line, "\(?(?<row>\d+),(?<col>\d+)\): (?<states>.+)", "names");
            if isempty(tokens)
                continue;
            end
            row = str2double(tokens.row) + 1;
            col = str2double(tokens.col) + 1;
            states = str2num(tokens.states); %#ok<ST2NM>
            full_grid(row, col, :) = states;
        end
    end

    % Plot the grid
    for row = 1:M
        for col = 1:N
            x_center = col * 1.5; % X position
            y_center = row * sqrt(3); % Y position
            if mod(row, 2) == 1
                x_center = x_center + 0.75;
            end

            % Plot the arrows for the current cell
            for d = 1:DIRECTIONS
                if full_grid(row, col, d) == 1
                    arrow_x = [x_center, x_center + 0.5 * cos(arrow_directions(d))];
                    arrow_y = [y_center, y_center + 0.5 * sin(arrow_directions(d))];
                    plot(arrow_x, arrow_y, 'r-', 'LineWidth', 1.5);
                end
            end
        end
    end

    % Set plot title and settings
    title(sprintf("Timestep %d", timestep), 'FontSize', 16);
    xlabel("Columns", 'FontSize', 14);
    ylabel("Rows", 'FontSize', 14);
    xlim([-1, N + 1] * 1.5);
    ylim([-1, M + 1] * sqrt(3));
    axis off;

    % Pause to visualize the current timestep
    pause(0.05);
end

disp("Visualization complete.");
