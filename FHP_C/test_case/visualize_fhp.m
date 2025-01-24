
files = {
    'test1_initial.txt', 
    'test1_after_one_step.txt', 
    'test2_initial.txt', 
    'test2_after_collision.txt', 
    'test3_initial.txt', 
    'test3_after_streaming.txt'
};
visualize_all_fhp(files);

function visualize_all_fhp(file_list)
    % Visualize multiple FHP lattice configurations from a list of files.
    % Arguments:
    %   file_list: A cell array of file names to visualize.

    for i = 1:length(file_list)
        filename = file_list{i};
        fprintf('Visualizing %s\n', filename);
        visualize_fhp(filename); % Call the visualization function
        pause(1); % Pause for 1 second between visualizations
    end
end



% Helper Function: Visualize a single configuration
function visualize_fhp(filename)
    % Visualize FHP lattice configuration
    % Arguments:
    %   filename: Name of the file containing the FHP configuration

    % Open the file and read the data
    fileID = fopen(filename, 'r');
    if fileID == -1
        error('Failed to open file: %s', filename);
    end

    % Initialize variables
    M = 5; % Number of rows
    N = 5; % Number of columns
    DIRECTIONS = 6; % Number of directions
    grid = zeros(M, N, DIRECTIONS);

    % Read data from file
    while ~feof(fileID)
        line = fgetl(fileID);
        if isempty(line) || line(1) ~= '('
            continue;
        end
        data = sscanf(line, '(%d,%d): %d %d %d %d %d %d');
        if numel(data) == 8
            row = data(1) + 1;
            col = data(2) + 1;
            grid(row, col, :) = data(3:end)';
        end
    end
    fclose(fileID);

    % Plot the grid
    figure('Name', ['FHP Visualization - ', filename], 'NumberTitle', 'off');
    hold on;
    axis equal;
    axis([0, N, 0, M]);
    title(['FHP Lattice Visualization: ', filename], 'Interpreter', 'none');
    xlabel('Columns');
    ylabel('Rows');

    % Hexagonal directions for visualization
    hex_directions = [
        0, 1;   % Direction 0
        -sqrt(3)/2, 0.5; % Direction 1
        -sqrt(3)/2, -0.5; % Direction 2
        0, -1; % Direction 3
        sqrt(3)/2, -0.5; % Direction 4
        sqrt(3)/2, 0.5  % Direction 5
    ];

    % Draw the particles
    for i = 1:M
        for j = 1:N
            x = j - 1; % X-coordinate
            y = M - i; % Y-coordinate
            for d = 1:DIRECTIONS
                if grid(i, j, d) == 1
                    % Draw arrow for direction d
                    quiver(x, y, hex_directions(d, 1), hex_directions(d, 2), ...
                        0.3, 'MaxHeadSize', 1, 'Color', 'b');
                end
            end
        end
    end
    hold off;
end