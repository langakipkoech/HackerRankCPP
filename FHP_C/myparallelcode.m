% MATLAB Visualization Script for Lattice Gas Automaton Configurations
% Visualizes `config_timestep_<timestep>_rank_<rank>.txt` files.

% Parameters
clc;
clear;
close all;

% Parameters
M = 30; % Total rows
N = 10; % Total columns
DIRECTIONS = 6; % Number of directions
num_timesteps = 5; % Number of timesteps to visualize
num_ranks = 4; % Number of ranks in the MPI program

% Arrow directions for hexagonal grid
arrow_directions = [0, pi/3, 2*pi/3, pi, 4*pi/3, 5*pi/3];

% Figure settings
figure;
set(gcf, 'Position', [100, 100, 1600, 900]); % Adjust figure size
hold on;

% Iterate through timesteps
for timestep = 0:(num_timesteps - 1)
    % Combine configurations from all ranks
    global_config = cell(M, N); % Placeholder for full configuration

    for rank = 0:(num_ranks - 1)
        % Load configuration file for the current rank
        filename = sprintf('config_t%d.txt', timestep, rank);
        if ~isfile(filename)
            warning('File %s does not exist. Skipping...', filename);
            continue;
        end
        data = readlines(filename);

        % Process rank's data
        for i = 1:numel(data)
            line = strtrim(data(i));
            if line == ""
                continue;
            end

            % Parse data: (row, col): state
            tokens = regexp(line, '\((\d+),(\d+)\): (.*)', 'tokens');
            if isempty(tokens)
                continue;
            end
            tokens = tokens{1};
            row = str2double(tokens{1});
            col = str2double(tokens{2});
            state = str2num(tokens{3}); %#ok<ST2NM>

            global_config{row + 1, col + 1} = state; % Save state (MATLAB index starts at 1)
        end
    end

    % Create subplot for the current timestep
    subplot(1, num_timesteps, timestep + 1);
    hold on;
    axis equal;
    title(sprintf('Timestep %d', timestep), 'FontSize', 12);
    xlabel('Columns', 'FontSize', 10);
    ylabel('Rows', 'FontSize', 10);

    % Visualize the global configuration
    for row = 1:M
        for col = 1:N
            state = global_config{row, col};
            if isempty(state)
                continue;
            end

            % Convert grid coordinates to hexagonal positions
            x_center = col * 1.5; % Adjust column spacing
            y_center = row * sqrt(3); % Adjust row spacing

            % Adjust for staggered rows
            if mod(row, 2) == 1
                x_center = x_center + 0.75;
            end

            % Plot arrows for active directions
            for d = 1:DIRECTIONS
                if state(d) == 1
                    arrow_x = [x_center, x_center + 0.5 * cos(arrow_directions(d))];
                    arrow_y = [y_center, y_center + 0.5 * sin(arrow_directions(d))];
                    plot(arrow_x, arrow_y, 'r-', 'LineWidth', 1.5); % Red arrows
                end
            end
        end
    end

    % Configure subplot settings
    xlim([-1, N+1] * 1.5); % Full column range
    ylim([-1, M+1] * sqrt(3)); % Full row range
    axis off;
end

disp('Visualization complete.');
