#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

#define A 0.1  // Lattice spacing
#define M ((int)(30 / A))  // Scaled grid rows
#define N ((int)(10 / A))  // Scaled grid columns

#define DIRECTIONS 6
#define TIMESTEPS 100

int direction_vectors[DIRECTIONS][2] = {
    {0, 1}, {-1, 1}, {-1, 0}, {0, -1}, {1, -1}, {1, 0}
};

typedef struct {
    int state[DIRECTIONS];
} Node;


Node grid[M][N];
Node temp_grid[M][N];

// Function prototypes
// Function prototypes
void initialize_grid(int rank, int cols_per_process);
void collision_phase(int start_col, int end_col);
void streaming_phase(int start_col, int end_col, int rank, int size);
void save_configuration_to_file(int timestep, int rank, int size);
void apply_obstacle_no_slip(int start_col, int end_col);
void apply_slip_boundary_conditions(int rank, int size);
void apply_dirichlet_boundary_conditions(int rank, int size);
void apply_periodic_boundary_conditions(int rank, int size);

int main(int argc, char* argv[]) {
    int rank, size;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check if N is divisible by size
    if (N % size != 0) {
        if (rank == 0) {
            printf("Error: Grid columns must be divisible by the number of processes.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int cols_per_process = N / size;
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process - 1;

    // Initialize the grid
    initialize_grid(rank, cols_per_process);

    // Main simulation loop
    for (int timestep = 0; timestep < TIMESTEPS; timestep++) {
        printf("Rank %d: Timestep %d\n", rank, timestep);

        collision_phase(start_col, end_col);
        streaming_phase(start_col, end_col, rank, size);

        // Apply boundaries and obstacle
        apply_slip_boundary_conditions(rank, size);
        apply_dirichlet_boundary_conditions(rank, size);
        apply_obstacle_no_slip(start_col, end_col);

        // Save configuration
        save_configuration_to_file(timestep, rank, size);
    }

    MPI_Finalize();
    return 0;
}

//initialize the grid
void initialize_grid(int rank, int cols_per_process){
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    //initializing the grid
    for(int i = 0; i < M; i++){
        for (int j = start_col; j < end_col; j++){
            for (int d = 0; d < DIRECTIONS; d++){
                grid[i][j].state[d] = rand() % 2;
            }
        }
    }
}

//set up the collision phase 
//local particle interaction
// Apply collision rules
void collision_phase(int rank, int cols_per_process) {
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;
  

    for (int i = 0; i < M; i++) {
        for (int j = start_col; j < end_col; j++) {
            int occupied[DIRECTIONS] = {0};
            for (int d = 0; d < DIRECTIONS; d++) {
                occupied[d] = grid[i][j].state[d];
            }

            int count = 0;
            for (int d = 0; d < DIRECTIONS; d++) {
                count += occupied[d];
            }

            if (count == 2) {
                // Handle two occupied back-to-back directions
                if (occupied[0] && occupied[3]) {
                    if (rand() % 2 == 0) {
                        grid[i][j].state[0] = 0;
                        grid[i][j].state[3] = 0;
                        grid[i][j].state[1] = 1;
                        grid[i][j].state[4] = 1;
                    } else {
                        grid[i][j].state[0] = 0;
                        grid[i][j].state[3] = 0;
                        grid[i][j].state[2] = 1;
                        grid[i][j].state[5] = 1;
                    }
                } else if (occupied[1] && occupied[4]) {
                    if (rand() % 2 == 0) {
                        grid[i][j].state[1] = 0;
                        grid[i][j].state[4] = 0;
                        grid[i][j].state[0] = 1;
                        grid[i][j].state[3] = 1;
                    } else {
                        grid[i][j].state[1] = 0;
                        grid[i][j].state[4] = 0;
                        grid[i][j].state[2] = 1;
                        grid[i][j].state[5] = 1;
                    }
                } else if (occupied[2] && occupied[5]) {
                    if (rand() % 2 == 0) {
                        grid[i][j].state[2] = 0;
                        grid[i][j].state[5] = 0;
                        grid[i][j].state[0] = 1;
                        grid[i][j].state[3] = 1;
                    } else {
                        grid[i][j].state[2] = 0;
                        grid[i][j].state[5] = 0;
                        grid[i][j].state[1] = 1;
                        grid[i][j].state[4] = 1;
                    }
                }
            } else if (count == 3) {
                // Handle three occupied directions
                // Case: Two back-to-back directions and one spectator
                if (occupied[0] && occupied[3] && occupied[1]) {
                    grid[i][j].state[0] = 0;
                    grid[i][j].state[3] = 0;
                    grid[i][j].state[2] = 1;
                    grid[i][j].state[5] = 1;
                    grid[i][j].state[1] = 1;
                } else if (occupied[1] && occupied[4] && occupied[2]) {
                    grid[i][j].state[1] = 0;
                    grid[i][j].state[4] = 0;
                    grid[i][j].state[0] = 1;
                    grid[i][j].state[3] = 1;
                    grid[i][j].state[2] = 1;
                } else if (occupied[2] && occupied[5] && occupied[3]) {
                    grid[i][j].state[2] = 0;
                    grid[i][j].state[5] = 0;
                    grid[i][j].state[1] = 1;
                    grid[i][j].state[4] = 1;
                    grid[i][j].state[3] = 1;
                } else if (occupied[0] && occupied[3] && occupied[5]) {
                    grid[i][j].state[0] = 0;
                    grid[i][j].state[3] = 0;
                    grid[i][j].state[1] = 1;
                    grid[i][j].state[4] = 1;
                    grid[i][j].state[5] = 1;
                }
            } else if (count == 4) {
                // Handle four occupied directions
                if (!occupied[0] && !occupied[3]) {
                    if (rand() % 2 == 0) {
                        grid[i][j].state[0] = 1;
                        grid[i][j].state[3] = 1;
                        grid[i][j].state[1] = 0;
                        grid[i][j].state[4] = 0;
                        grid[i][j].state[2] = 1;
                        grid[i][j].state[5] = 1;
                    } else {
                        grid[i][j].state[0] = 1;
                        grid[i][j].state[3] = 1;
                        grid[i][j].state[1] = 1;
                        grid[i][j].state[4] = 1;
                        grid[i][j].state[2] = 0;
                        grid[i][j].state[5] = 0;
                    }
                } else if (!occupied[1] && !occupied[4]) {
                    if (rand() % 2 == 0) {
                        grid[i][j].state[1] = 1;
                        grid[i][j].state[4] = 1;
                        grid[i][j].state[0] = 0;
                        grid[i][j].state[3] = 0;
                        grid[i][j].state[2] = 1;
                        grid[i][j].state[5] = 1;
                    } else {
                        grid[i][j].state[1] = 1;
                        grid[i][j].state[4] = 1;
                        grid[i][j].state[0] = 1;
                        grid[i][j].state[3] = 1;
                        grid[i][j].state[2] = 0;
                        grid[i][j].state[5] = 0;
                    }
                } else if (!occupied[2] && !occupied[5]) {
                    if (rand() % 2 == 0) {
                        grid[i][j].state[2] = 1;
                        grid[i][j].state[5] = 1;
                        grid[i][j].state[0] = 0;
                        grid[i][j].state[3] = 0;
                        grid[i][j].state[1] = 1;
                        grid[i][j].state[4] = 1;
                    } else {
                        grid[i][j].state[2] = 1;
                        grid[i][j].state[5] = 1;
                        grid[i][j].state[0] = 1;
                        grid[i][j].state[3] = 1;
                        grid[i][j].state[1] = 0;
                        grid[i][j].state[4] = 0;
                    }
                }
            }
        }
    }
}

// Streaming phase: move particles to neighbors 
void streaming_phase(int start_col, int end_col, int rank, int size) {
    // Ensure boundary columns are handled correctly by exchanging with neighboring processes first
    if (rank > 0) {
        MPI_Sendrecv(&grid[0][start_col], M * sizeof(Node), MPI_BYTE, rank - 1, 0,
                     &grid[0][start_col - 1], M * sizeof(Node), MPI_BYTE, rank - 1, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    if (rank < size - 1) {
        MPI_Sendrecv(&grid[0][end_col - 1], M * sizeof(Node), MPI_BYTE, rank + 1, 0,
                     &grid[0][end_col], M * sizeof(Node), MPI_BYTE, rank + 1, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Perform streaming using the updated grid and write to temp_grid
    for (int i = 0; i < M; i++) {
        for (int j = start_col; j < end_col; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                if (grid[i][j].state[d] == 1) {
                    int new_i = (i + direction_vectors[d][0] + M) % M;
                    int new_j = (j + direction_vectors[d][1] + N) % N;

                    // Check if the new column belongs to this process
                    if (new_j >= start_col && new_j < end_col) {
                        temp_grid[new_i][new_j].state[d] = 1;
                    }
                    temp_grid[i][j].state[d] = 0;
                }
            }
        }
    }


    // Copy updated temp_grid back to grid
    for (int i = 0; i < M; i++) {
        for (int j = start_col; j < end_col; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][j].state[d] = temp_grid[i][j].state[d];
            }
        }
    }
}

//save configuration
void save_configuration_to_file(int timestep, int rank, int size) {
    // Create a filename using the timestep and rank
    char filename[50];
    sprintf(filename, "config_t%d_rank%d.txt", timestep, rank);

    // Open the file for writing
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    // Calculate the column range for this rank
    int cols_per_process = N / size;
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process - 1;

    // Iterate through rows and columns in this process's range
    for (int i = 0; i < M; i++) {
        for (int j = start_col; j <= end_col; j++) {
            fprintf(file, "%d,%d: ", i, j);

            // Check if the cell is part of the obstacle
            int dx = i - 8;
            int dy = j - 4;
            if (dx * dx + dy * dy <= 1) {
                fprintf(file, "Obstacle\n");
            } else {
                // Write the states for each direction
                for (int d = 0; d < DIRECTIONS; d++) {
                    fprintf(file, "%d ", grid[i][j].state[d]);
                }
                fprintf(file, "\n");
            }
        }
    }

    // Close the file
    fclose(file);
}




//top and bottom nboundary conditions - long edges
void apply_slip_boundary_conditions(int rank, int size) {
    // Top boundary
    if (rank == 0) {
        for (int j = 0; j < N; j++) {
            grid[0][j].state[2] = grid[1][j].state[5];  // Flip downward direction to upward
            grid[0][j].state[5] = grid[1][j].state[2];
        }
    }

    // Bottom boundary
    if (rank == size - 1) {
        for (int j = 0; j < N; j++) {
            grid[M - 1][j].state[2] = grid[M - 2][j].state[5];  // Flip upward direction to downward
            grid[M - 1][j].state[5] = grid[M - 2][j].state[2];
        }
    }
}

//short edges
void apply_dirichlet_boundary_conditions(int rank, int size) {
    int cols_per_process = N / size;
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    // Inflow (left column)
    if (start_col == 0) {
        for (int i = 0; i < M; i++) {
            grid[i][0].state[0] = 1;  // Fixed inflow direction
            grid[i][0].state[3] = 0;  // Ensure no opposite flow
        }
    }

    // Outflow (right column)
    if (end_col == N) {
        for (int i = 0; i < M; i++) {
            grid[i][N - 1].state[3] = 1; // Fixed outflow direction
            grid[i][N - 1].state[0] = 0; // Ensure no opposite flow
        }
    }
}

//periodic boundary conditions
void apply_periodic_boundary_conditions(int rank, int size) {
    int cols_per_process = N / size;
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    // Left column wraps to the right
    if (start_col == 0) {
        for (int i = 0; i < M; i++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][0].state[d] = grid[i][N - 1].state[d];
            }
        }
    }

    // Right column wraps to the left
    if (end_col == N) {
        for (int i = 0; i < M; i++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][N - 1].state[d] = grid[i][0].state[d];
            }
        }
    }
}


//obstacle
void apply_obstacle_no_slip(int rank, int cols_per_process) {
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    for (int i = 0; i < M; i++) {
        for (int j = start_col; j < end_col; j++) {
            int dx = i - 8;
            int dy = j - 4;
            if (dx * dx + dy * dy <= 1) {
                // Apply no-slip boundary condition
                for (int d = 0; d < DIRECTIONS; d++) {
                    grid[i][j].state[d] = 0;
                }
            }
        }
    }
}