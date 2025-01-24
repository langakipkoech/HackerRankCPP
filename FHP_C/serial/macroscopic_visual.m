% Parameters
M = 30; % Grid rows
N = 10; % Grid columns
start_timestep = 0; % Starting timestep
num_timesteps = 100; % Number of timesteps to visualize

% Loop through each timestep
for timestep = start_timestep:(start_timestep + num_timesteps - 1)
    % Load macroscopic data
    filename = sprintf('macroscopic_t%d.txt', timestep);
    if ~isfile(filename)
        warning('File %s does not exist. Skipping...', filename);
        continue;
    end
    data = readlines(filename);

    % Initialize storage for density and velocity
    density = zeros(M, N);
    velocity_x = zeros(M, N);
    velocity_y = zeros(M, N);

    % Parse data from the file
    for i = 1:numel(data)
        line = strtrim(data(i));
        if line == ""
            continue;
        end

        % Extract row, column, density, and velocity
        tokens = regexp(line, '\((\d+),(\d+)\): Density=(.*), Velocity=\((.*),(.*)\)', 'tokens');
        if isempty(tokens)
            continue;
        end
        tokens = tokens{1};

        % Parse values
        row = str2double(tokens{1}) + 1; % MATLAB indexing starts from 1
        col = str2double(tokens{2}) + 1;
        density(row, col) = str2double(tokens{3});
        velocity_x(row, col) = str2double(tokens{4});
        velocity_y(row, col) = str2double(tokens{5});
    end

    % Create a meshgrid for plotting
    [X, Y] = meshgrid(1:N, 1:M);

    % Plot the velocity field as a quiver plot
    figure(1); % Use a single figure for all timesteps
    clf; % Clear the figure for the next frame
    quiver(X, Y, velocity_x, velocity_y, 'r', 'LineWidth', 1.5);
    title(sprintf('Macroscopic Flow at Timestep %d', timestep), 'FontSize', 14);
    xlabel('Columns', 'FontSize', 12);
    ylabel('Rows', 'FontSize', 12);
    axis equal;
    xlim([0, N + 1]);
    ylim([0, M + 1]);
    grid on;

    

    % Pause to control frame rate
    pause(0.5); % Adjust pause time for desired speed
end
