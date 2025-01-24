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
    int is_dirichlet; // Flag for Dirichlet boundary
} Node;

Node **grid;
Node **temp_grid;

// Function prototypes
void allocate_grids();
void free_grids();
void initialize_grid(int rank, int cols_per_process);
void collision_phase(int rank, int cols_per_process);
void streaming_phase(int rank, int cols_per_process, int size);
void apply_boundary_conditions(int rank, int cols_per_process, int size);
void apply_slip_boundary_conditions(int rank, int size);
void apply_dirichlet_boundary_conditions(int rank, int cols_per_process);
void apply_periodic_boundary_conditions(int rank, int cols_per_process, int size);
void save_configuration_to_file(int timestep, int rank, int size);

void allocate_grids() {
    grid = malloc(M * sizeof(Node *));
    temp_grid = malloc(M * sizeof(Node *));
    if (!grid || !temp_grid) {
        perror("Failed to allocate grids");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    for (int i = 0; i < M; i++) {
        grid[i] = calloc(N, sizeof(Node));
        temp_grid[i] = calloc(N, sizeof(Node));
        if (!grid[i] || !temp_grid[i]) {
            perror("Failed to allocate grid rows");
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
    }
}



void free_grids() {
    for (int i = 0; i < M; i++) {
        free(grid[i]);
        free(temp_grid[i]);
    }
    free(grid);
    free(temp_grid);
}

void initialize_grid(int rank, int cols_per_process) {
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    for (int i = 0; i < M; i++) {
        for (int j = start_col; j < end_col; j++) {
            grid[i][j].is_dirichlet = (j == 0 || j == N - 1); // Dirichlet on left and right boundaries
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][j].state[d] = grid[i][j].is_dirichlet ? 0 : rand() % 2;
            }
        }
    }
}

void collision_phase(int rank, int cols_per_process) {
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    for (int i = 0; i < M; i++) {
        for (int j = start_col; j < end_col; j++) {
            if (grid[i][j].is_dirichlet) continue; // Skip Dirichlet nodes

            int occupied[DIRECTIONS] = {0};
            for (int d = 0; d < DIRECTIONS; d++) {
                occupied[d] = grid[i][j].state[d];
            }

            int count = 0;
            for (int d = 0; d < DIRECTIONS; d++) {
                count += occupied[d];
            }

            if (count == 2) {
                if (occupied[0] && occupied[3]) {
                    if (rand() % 2) {
                        grid[i][j].state[0] = grid[i][j].state[3] = 0;
                        grid[i][j].state[1] = grid[i][j].state[4] = 1;
                    } else {
                        grid[i][j].state[0] = grid[i][j].state[3] = 0;
                        grid[i][j].state[2] = grid[i][j].state[5] = 1;
                    }
                }
                // Handle other back-to-back directions
                else if (occupied[1] && occupied[4]) {
                    if (rand() % 2) {
                        grid[i][j].state[1] = grid[i][j].state[4] = 0;
                        grid[i][j].state[0] = grid[i][j].state[3] = 1;
                    } else {
                        grid[i][j].state[1] = grid[i][j].state[4] = 0;
                        grid[i][j].state[2] = grid[i][j].state[5] = 1;
                    }
                } else if (occupied[2] && occupied[5]) {
                    if (rand() % 2) {
                        grid[i][j].state[2] = grid[i][j].state[5] = 0;
                        grid[i][j].state[0] = grid[i][j].state[3] = 1;
                    } else {
                        grid[i][j].state[2] = grid[i][j].state[5] = 0;
                        grid[i][j].state[1] = grid[i][j].state[4] = 1;
                    }
                }
            } else if (count == 3) {
                // Handle three occupied directions
                if (occupied[0] && occupied[3] && occupied[1]) {
                    grid[i][j].state[0] = grid[i][j].state[3] = 0;
                    grid[i][j].state[2] = grid[i][j].state[5] = grid[i][j].state[1] = 1;
                } else if (occupied[1] && occupied[4] && occupied[2]) {
                    grid[i][j].state[1] = grid[i][j].state[4] = 0;
                    grid[i][j].state[0] = grid[i][j].state[3] = grid[i][j].state[2] = 1;
                } else if (occupied[2] && occupied[5] && occupied[3]) {
                    grid[i][j].state[2] = grid[i][j].state[5] = 0;
                    grid[i][j].state[1] = grid[i][j].state[4] = grid[i][j].state[3] = 1;
                } else if (occupied[0] && occupied[3] && occupied[5]) {
                    grid[i][j].state[0] = grid[i][j].state[3] = 0;
                    grid[i][j].state[1] = grid[i][j].state[4] = grid[i][j].state[5] = 1;
                }
            } else if (count == 4) {
                // Handle four occupied directions
                if (!occupied[0] && !occupied[3]) {
                    if (rand() % 2) {
                        grid[i][j].state[0] = grid[i][j].state[3] = 1;
                        grid[i][j].state[1] = grid[i][j].state[4] = 0;
                        grid[i][j].state[2] = grid[i][j].state[5] = 1;
                    } else {
                        grid[i][j].state[0] = grid[i][j].state[3] = 1;
                        grid[i][j].state[1] = grid[i][j].state[4] = 1;
                        grid[i][j].state[2] = grid[i][j].state[5] = 0;
                    }
                } else if (!occupied[1] && !occupied[4]) {
                    if (rand() % 2) {
                        grid[i][j].state[1] = grid[i][j].state[4] = 1;
                        grid[i][j].state[0] = grid[i][j].state[3] = 0;
                        grid[i][j].state[2] = grid[i][j].state[5] = 1;
                    } else {
                        grid[i][j].state[1] = grid[i][j].state[4] = 1;
                        grid[i][j].state[0] = grid[i][j].state[3] = 1;
                        grid[i][j].state[2] = grid[i][j].state[5] = 0;
                    }
                } else if (!occupied[2] && !occupied[5]) {
                    if (rand() % 2) {
                        grid[i][j].state[2] = grid[i][j].state[5] = 1;
                        grid[i][j].state[0] = grid[i][j].state[3] = 0;
                        grid[i][j].state[1] = grid[i][j].state[4] = 1;
                    } else {
                        grid[i][j].state[2] = grid[i][j].state[5] = 1;
                        grid[i][j].state[0] = grid[i][j].state[3] = 1;
                        grid[i][j].state[1] = grid[i][j].state[4] = 0;
                    }
                }
            }
        }
    }
}

void streaming_phase(int rank, int cols_per_process, int size) {
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    // Exchange ghost columns with neighbors
    if (rank > 0) {
        MPI_Sendrecv(
            &grid[0][start_col], M * sizeof(Node), MPI_BYTE, rank - 1, 0,
            &grid[0][start_col - 1], M * sizeof(Node), MPI_BYTE, rank - 1, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );
    }
    if (rank < size - 1) {
        MPI_Sendrecv(
            &grid[0][end_col - 1], M * sizeof(Node), MPI_BYTE, rank + 1, 0,
            &grid[0][end_col], M * sizeof(Node), MPI_BYTE, rank + 1, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );
    }

    // Clear temp_grid
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                temp_grid[i][j].state[d] = 0;
            }
        }
    }

    // Perform streaming
    for (int i = 0; i < M; i++) {
        for (int j = start_col; j < end_col; j++) {
            if (grid[i][j].is_dirichlet) {
                for (int d = 0; d < DIRECTIONS; d++) {
                    temp_grid[i][j].state[d] = grid[i][j].state[d];
                }
                continue;
            }

            for (int d = 0; d < DIRECTIONS; d++) {
                if (grid[i][j].state[d]) {
                    int new_i = (i + direction_vectors[d][0] + M) % M;
                    int new_j = (j + direction_vectors[d][1] + N) % N;
                    temp_grid[new_i][new_j].state[d] = 1;
                }
            }
        }
    }

    // Copy temp_grid back to grid
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][j].state[d] = temp_grid[i][j].state[d];
            }
        }
    }
}


void save_configuration_to_file(int timestep, int rank, int size) {
    char filename[50];
    sprintf(filename, "config_timestep_%d_rank_%d.txt", timestep, rank);
    FILE *file = fopen(filename, "w");

    int cols_per_process = N / size;
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    for (int i = 0; i < M; i++) {
        for (int j = start_col; j < end_col; j++) {
            fprintf(file, "(%d,%d): ", i, j);
            for (int d = 0; d < DIRECTIONS; d++) {
                fprintf(file, "%d ", grid[i][j].state[d]);
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

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

void apply_boundary_conditions(int rank, int cols_per_process, int size) {
    apply_slip_boundary_conditions(rank, size);
    apply_dirichlet_boundary_conditions(rank, cols_per_process);
    apply_periodic_boundary_conditions(rank, cols_per_process, size);
}

void apply_slip_boundary_conditions(int rank, int size) {
    // Top boundary (rank 0)
    if (rank == 0) {
        for (int j = 0; j < N; j++) {
            grid[0][j].state[2] = grid[1][j].state[5];  // Flip downward direction to upward
            grid[0][j].state[5] = grid[1][j].state[2];
        }
    }

    // Bottom boundary (last rank)
    if (rank == size - 1) {
        for (int j = 0; j < N; j++) {
            grid[M - 1][j].state[2] = grid[M - 2][j].state[5];  // Flip upward direction to downward
            grid[M - 1][j].state[5] = grid[M - 2][j].state[2];
        }
    }
}

void apply_dirichlet_boundary_conditions(int rank, int cols_per_process) {
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    for (int i = 0; i < M; i++) {
        if (start_col == 0) {  // Left column (inflow)
            grid[i][0].state[0] = 1;  // Fixed inflow direction
            grid[i][0].state[3] = 0;  // Ensure no opposite flow
        }

        if (end_col == N) {  // Right column (outflow)
            grid[i][N - 1].state[3] = 1; // Fixed outflow direction
            grid[i][N - 1].state[0] = 0; // Ensure no opposite flow
        }
    }
}

void apply_periodic_boundary_conditions(int rank, int cols_per_process, int size) {
    int start_col = rank * cols_per_process;
    int end_col = start_col + cols_per_process;

    // Periodic boundaries between the left and right edges
    if (rank == 0) {  // First process handles left-to-right wrapping
        for (int i = 0; i < M; i++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][0].state[d] = grid[i][N - 1].state[d];
            }
        }
    }

    if (rank == size - 1) {  // Last process handles right-to-left wrapping
        for (int i = 0; i < M; i++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][N - 1].state[d] = grid[i][0].state[d];
            }
        }
    }
}



int main(int argc, char* argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) {
            printf("Error: Grid columns must be divisible by the number of processes.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    allocate_grids();

    int cols_per_process = N / size;
    initialize_grid(rank, cols_per_process);

    for (int timestep = 0; timestep < TIMESTEPS; timestep++) {
        collision_phase(rank, cols_per_process);
        apply_obstacle_no_slip(rank, cols_per_process);
        streaming_phase(rank, cols_per_process, size);
        apply_boundary_conditions(rank, cols_per_process, size);
        save_configuration_to_file(timestep, rank, size);
    }

    free_grids();
    MPI_Finalize();
    return 0;
}
