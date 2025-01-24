#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIRECTIONS 6
#define M 5 // Grid rows
#define N 5 // Grid columns

// Direction vectors for a hexagonal grid
int direction_vectors[DIRECTIONS][2] = {
    {0, 1}, {-1, 1}, {-1, 0}, {0, -1}, {1, -1}, {1, 0}
};

typedef struct {
    int state[DIRECTIONS];
} Node;

Node grid[M][N];

void initialize_grid_single_particle(int x, int y, int direction) {
    // Initialize grid with one particle
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][j].state[d] = 0;
            }
        }
    }
    grid[x][y].state[direction] = 1;
}

void initialize_grid_two_particles(int x, int y, int d1, int d2) {
    // Initialize grid with two particles at back-to-back directions
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][j].state[d] = 0;
            }
        }
    }
    grid[x][y].state[d1] = 1;
    grid[x][y].state[d2] = 1;
}

void apply_periodic_boundary() {
    for (int i = 0; i < M; i++) {
        grid[i][0] = grid[i][N - 1];
        grid[i][N - 1] = grid[i][0];
    }
}

void streaming_phase() {
    Node temp_grid[M][N] = {0};

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                if (grid[i][j].state[d] == 1) {
                    int new_i = (i + direction_vectors[d][0] + M) % M;
                    int new_j = (j + direction_vectors[d][1] + N) % N;
                    temp_grid[new_i][new_j].state[d] = 1;
                }
            }
        }
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int d = 0; d < DIRECTIONS; d++) {
                grid[i][j].state[d] = temp_grid[i][j].state[d];
            }
        }
    }
}

void collision_phase() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int count = 0;
            int occupied[DIRECTIONS] = {0};

            for (int d = 0; d < DIRECTIONS; d++) {
                occupied[d] = grid[i][j].state[d];
                count += occupied[d];
            }

            if (count == 2 && occupied[0] && occupied[3]) {
                if (rand() % 2) {
                    grid[i][j].state[0] = grid[i][j].state[3] = 0;
                    grid[i][j].state[1] = grid[i][j].state[4] = 1;
                } else {
                    grid[i][j].state[0] = grid[i][j].state[3] = 0;
                    grid[i][j].state[2] = grid[i][j].state[5] = 1;
                }
            }
        }
    }
}

void save_grid_to_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, "(%d,%d): ", i, j);
            for (int d = 0; d < DIRECTIONS; d++) {
                fprintf(file, "%d ", grid[i][j].state[d]);
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
    printf("Configuration saved to %s\n", filename);
}

void print_grid() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("(%d,%d): ", i, j);
            for (int d = 0; d < DIRECTIONS; d++) {
                printf("%d ", grid[i][j].state[d]);
            }
            printf("\n");
        }
    }
}

void test_single_particle_motion() {
    printf("\nTest 1: Single Particle Motion\n");
    initialize_grid_single_particle(2, 2, 0);
    printf("Initial state:\n");
    print_grid();
    save_grid_to_file("test1_initial.txt");

    streaming_phase();
    printf("After one step:\n");
    print_grid();
    save_grid_to_file("test1_after_one_step.txt");
}

void test_two_particles_collision() {
    printf("\nTest 2: Two Particles Collision\n");
    initialize_grid_two_particles(2, 2, 0, 3);
    printf("Initial state:\n");
    print_grid();
    save_grid_to_file("test2_initial.txt");

    collision_phase();
    printf("After collision:\n");
    print_grid();
    save_grid_to_file("test2_after_collision.txt");
}

void test_periodic_boundaries() {
    printf("\nTest 3: Periodic Boundaries\n");
    initialize_grid_single_particle(2, N - 1, 0);
    printf("Initial state:\n");
    print_grid();
    save_grid_to_file("test3_initial.txt");

    streaming_phase();
    printf("After streaming (with periodic boundaries):\n");
    print_grid();
    save_grid_to_file("test3_after_streaming.txt");
}

int main() {
    srand(time(NULL));

    test_single_particle_motion();
    test_two_particles_collision();
    test_periodic_boundaries();

    return 0;
}
