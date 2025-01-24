function visualize_fhp_lattice()
    % Grid dimensions
    M = 30; % Rows
    N = 10; % Columns
    directions = [0, 1; -1, 1; -1, 0; 0, -1; 1, -1; 1, 0]; % Hexagonal directions

    % Get list of configuration files
    config_files = dir('config_timestep_*.txt');
    if isempty(config_files)
        error('No configuration files found in the current directory.');
    end

    % Initialize figure
    figure;
    colormap jet;

    % Loop through configuration files
    for file_idx = 1:length(config_files)
        % Read lattice configuration
        lattice = load_lattice(config_files(file_idx).name, M, N);

        % Clear figure
        clf;
        hold on;
        axis equal;
        axis([0, N, 0, M]);
        title(['FHP Lattice Visualization: ', config_files(file_idx).name], 'Interpreter', 'none');
        xlabel('Columns');
        ylabel('Rows');

        % Plot particle directions using quiver
        for i = 1:M
            for j = 1:N
                for d = 1:size(directions, 1)
                    if lattice(i, j, d) == 1
                        quiver(j - 0.5, M - i + 0.5, directions(d, 2), -directions(d, 1), ...
                            'MaxHeadSize', 0.2, 'AutoScale', 'off', 'Color', 'b');
                    end
                end
            end
        end

        % Pause for visualization
        pause(0.5); % Adjust this to control the speed
    end
end

function lattice = load_lattice(filename, M, N)
    % Load lattice configuration from a file
    lattice = zeros(M, N, 6); % Initialize for 6 directions
    fid = fopen(filename, 'r');
    if fid == -1
        error('Cannot open file: %s', filename);
    end

    while ~feof(fid)
        line = fgetl(fid);
        if isempty(line) || line(1) ~= '('
            continue;
        end

        % Parse the line for lattice state
        tokens = regexp(line, '\((\d+),(\d+)\): (.+)', 'tokens');
        if ~isempty(tokens)
            i = str2double(tokens{1}{1}) + 1; % MATLAB is 1-based
            j = str2double(tokens{1}{2}) + 1;
            states = str2num(tokens{1}{3}); %#ok<ST2NM>
            lattice(i, j, :) = states;
        end
    end

    fclose(fid);
end
