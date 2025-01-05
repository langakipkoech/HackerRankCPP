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

int main() {
    srand(time(NULL)); 

    initialize_grid();

    for (int timestep = 0; timestep < TIMESTEPS; timestep++) {
        printf("Timestep %d\n", timestep);
        collision_phase();
        streaming_phase();
        save_configuration_to_file(timestep);
    }

    printf("Simulation complete.\n");
    return 0;
}
