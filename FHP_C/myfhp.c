#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define M 30  // Grid rows
#define N 10  // Grid columns
#define DIRECTIONS 6
#define TIMESTEPS 100

// Direction vectors for hexagonal grid
int direction_vectors[DIRECTIONS][2] = {
    {0, 1},   // Right
    {-1, 1},  // Top-right
    {-1, 0},  // Top-left
    {0, -1},  // Left
    {1, -1},  // Bottom-left
    {1, 0}    // Bottom-right
};

// Node structure
typedef struct {
    int state[DIRECTIONS]; // Occupancy of each direction
} Node;

Node grid[M][N];
Node temp_grid[M][N]; // Temporary grid for updates

// Initialize the grid with random states
void initialize_grid() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][j].state[d] = rand() % 2; // Random initialization (0 or 1)
            }
        }
    }
}

// Apply collision rules
void collision_phase() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
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
void streaming_phase() {
    memcpy(temp_grid, grid, sizeof(grid));

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                if (grid[i][j].state[d] == 1) {
                    int new_i = (i + direction_vectors[d][0] + M) % M;
                    int new_j = (j + direction_vectors[d][1] + N) % N;
                    temp_grid[new_i][new_j].state[d] = 1;
                    temp_grid[i][j].state[d] = 0;
                }
            }
        }
    }

    memcpy(grid, temp_grid, sizeof(grid));
}
/*


// Save configuration to file for MATLAB plotting
void save_configuration_to_file(int timestep) {
    char filename[50];
    sprintf(filename, "config_t%d.txt", timestep);
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, "%d,%d: ", i, j);
            for (int d = 0; d < DIRECTIONS; d++) {
                fprintf(file, "%d ", grid[i][j].state[d]);
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
}
*/
//slip boundary conditions
//this is applied to top and bottom rows
void apply_slip_boundary_conditions() {
    // Top 
    for (int j = 0; j < N; j++) {
        grid[0][j].state[2] = grid[1][j].state[5];  // Flip downward direction to upward
        grid[0][j].state[5] = grid[1][j].state[2];
    }

    // Bottom (ro
    for (int j = 0; j < N; j++) {
        grid[M - 1][j].state[2] = grid[M - 2][j].state[5];  // Flip upward direction to downward
        grid[M - 1][j].state[5] = grid[M - 2][j].state[2];
    }
}

//Dirichlet boundary condition
//This sets fixed inflow on the left and outflow on the right:
void apply_dirichlet_boundary_conditions() {
    // Inflow (left column)
    for (int i = 0; i < M; i++) {
        grid[i][0].state[0] = 1;  // Fixed inflow direction
        grid[i][0].state[3] = 0;  // Ensure no opposite flow
    }

    // Outflow (right column)
    for (int i = 0; i < M; i++) {
        grid[i][N - 1].state[3] = 1; // Fixed outflow direction
        grid[i][N - 1].state[0] = 0; // Ensure no opposite flow
    }
}

//Periodic boundary
void apply_periodic_boundary_conditions() {
    for (int i = 0; i < M; i++) {
        for (int d = 0; d < DIRECTIONS; d++) {
            // Left column wraps to the right
            grid[i][0].state[d] = grid[i][N - 1].state[d];
            // Right column wraps to the left
            grid[i][N - 1].state[d] = grid[i][0].state[d];
        }
    }
}

void apply_obstacle_no_slip() {
    int obstacle_center_x = 8;
    int obstacle_center_y = 4;
    int radius = 1;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            // Calculate distance from the obstacle center
            int dx = i - obstacle_center_x;
            int dy = j - obstacle_center_y;
            if (dx * dx + dy * dy <= radius * radius) {
                // Reflect velocities for all directions
                for (int d = 0; d < DIRECTIONS; d++) {
                    grid[i][j].state[d] = grid[i][j].state[(d + 3) % DIRECTIONS];
                }
            }
        }
    }
}






//outputing the config file
void save_configuration_to_file(int timestep) {
    char filename[50];
    sprintf(filename, "config_t%d.txt", timestep);
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, "%d,%d: ", i, j);
            int dx = i - 8;
            int dy = j - 4;
            if (dx * dx + dy * dy <= 1) {
                fprintf(file, "Obstacle\n");
            } else {
                for (int d = 0; d < DIRECTIONS; d++) {
                    fprintf(file, "%d ", grid[i][j].state[d]);
                }
                fprintf(file, "\n");
            }
        }
    }

    fclose(file);
}




//main function

int main() {
    srand(time(NULL)); 

    initialize_grid();

    for (int timestep = 0; timestep < TIMESTEPS; timestep++) {
        printf("Timestep %d\n", timestep);
        collision_phase();
        streaming_phase();

        //adopt the obstacle
        apply_obstacle_no_slip();

    //apply the boundary conditions
        apply_slip_boundary_conditions(); //long direction
        apply_dirichlet_boundary_conditions();
        //apply_periodic_boundary_conditions();

        save_configuration_to_file(timestep);
    }

    printf("Simulation complete.\n");
    return 0;
}
