% Visualize FHP lattice evolution across all timesteps
function visualize_fhp_evolution(config_prefix, num_timesteps)
   

    % Parameters for the grid
    rows = 500; % 
    cols = 200; % 
    directions = 6; % Number of lattice directions
    hex_directions = [
        0, 1;   % Direction 0
        -sqrt(3)/2, 0.5; % Direction 1
        -sqrt(3)/2, -0.5; % Direction 2
        0, -1; % Direction 3
        sqrt(3)/2, -0.5; % Direction 4
        sqrt(3)/2, 0.5  % Direction 5
    ];

    % Initialize visualization figure
    figure;
    hold on;
    axis equal;
    axis([0, cols, 0, rows]);
    xlabel('Columns');
    ylabel('Rows');
    title('FHP Lattice Evolution');
    quiver_plot = quiver([], [], [], [], 'b'); % Initialize quiver plot

    % Loop through all timesteps
    for t = 0:(num_timesteps - 1)
        % Construct file name
        filename = sprintf('%s%d.txt', config_prefix, t);

        % Check if the file exists
        if ~isfile(filename)
            warning('File %s not found. Skipping timestep %d...', filename, t);
            continue;
        end

        % Load data from the file
        fprintf('Visualizing timestep %d: %s\n', t, filename);
        data = read_fhp_file(filename, directions);
        if isempty(data)
            warning('Failed to read data from %s. Skipping...', filename);
            continue;
        end

        % Extract lattice states for visualization
        [x_quiver, y_quiver, u, v] = extract_lattice_vectors(data, hex_directions);

        % Update quiver plot
        set(quiver_plot, 'XData', x_quiver, 'YData', y_quiver, 'UData', u, 'VData', v);

        % Update title
        title(sprintf('FHP Lattice Evolution - Timestep %d', t));

       
        pause(0.2);
    end

    hold off;
end

% Helper function: Read FHP lattice configuration from a file
function data = read_fhp_file(filename, directions)
    % Reads FHP lattice data from the given file.
    % Arguments:
    %   filename: Path to the configuration file.
    %   directions: Number of lattice directions (default 6).
    % Returns:
    %   data: A matrix containing the parsed rows, columns, and states.

    fileID = fopen(filename, 'r');
    if fileID == -1
        data = [];
        return;
    end

    % Initialize data matrix
    data = [];
    while ~feof(fileID)
        line = fgetl(fileID);
        if isempty(line) || line(1) ~= '('
            continue;
        end
        values = sscanf(line, '(%d,%d): %d %d %d %d %d %d');
        if numel(values) == 2 + directions
            data = [data; values'];
        end
    end
    fclose(fileID);
end

% Helper function: Extract quiver vectors for visualization
function [x_quiver, y_quiver, u, v] = extract_lattice_vectors(data, hex_directions)
    % Extracts quiver plot data from the lattice configuration.
    % Arguments:
    %   data: Lattice configuration matrix.
    %   hex_directions: Hexagonal direction vectors.
    % Returns:
    %   x_quiver: X-coordinates for quiver plot.
    %   y_quiver: Y-coordinates for quiver plot.
    %   u: U-components (directions) of quiver plot vectors.
    %   v: V-components (directions) of quiver plot vectors.

    rows = data(:, 1) + 1; % Convert to 1-based indexing
    cols = data(:, 2) + 1;
    states = data(:, 3:end);

    % Initialize quiver arrays
    x_quiver = [];
    y_quiver = [];
    u = [];
    v = [];

    % Loop through all states
    for i = 1:size(states, 1)
        for d = 1:size(states, 2)
            if states(i, d) == 1 % If the direction is occupied
                x_quiver = [x_quiver; cols(i)];
                y_quiver = [y_quiver; rows(i)];
                u = [u; hex_directions(d, 1)];
                v = [v; -hex_directions(d, 2)]; % Flip Y-axis for MATLAB
            end
        end
    end
end

% Call the function to visualize the lattice evolution
% Example: Visualize from timestep 0 to timestep 99
visualize_fhp_evolution('config_timestep_', 100);
