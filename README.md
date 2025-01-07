
# Hexagonal Grid Cellular Automaton Simulation

This project simulates a cellular automaton on a hexagonal grid. It uses a collision and streaming model to update the states of particles at each node of the grid over a series of timesteps. The simulation is implemented in C and outputs the grid configuration at each timestep for further analysis or visualization.

## Features

- **Hexagonal Grid**: The grid is represented with `M x N` nodes, each with six possible directions (corresponding to the six neighbors in a hexagonal layout).
- **Collision Phase**: Handles interactions between particles at a node based on predefined rules.
- **Streaming Phase**: Moves particles to neighboring nodes in their respective directions.
- **Data Output**: Saves the state of the grid at each timestep to files for external visualization, e.g., in MATLAB.

## How It Works

1. **Initialization**: The grid is initialized with random binary states (0 or 1) for each direction at every node.
2. **Collision Phase**: Applies rules to resolve particle interactions at each node. 
   - Handles cases with two, three, or four particles in specific patterns.
3. **Streaming Phase**: Particles move to neighboring nodes in the direction they are facing.
4. **Data Output**: Saves the state of the grid at each timestep to a file in the format `config_t<TIMESTEP>.txt`.
5. **Iteration**: The process repeats for a user-defined number of timesteps (`TIMESTEPS`).

## Grid and Directions

The hexagonal grid is represented with nodes having six directions:
1. Right
2. Top-right
3. Top-left
4. Left
5. Bottom-left
6. Bottom-right

Each direction is represented as an array index in the `state` array of a node.

## File Structure

- **`config_t<TIMESTEP>.txt`**: The grid configuration at each timestep is saved in these files. Each line contains the coordinates of a node and the states of its six directions.

## Compilation and Execution

### Prerequisites

- GCC (GNU Compiler Collection) or any standard C compiler.
- A UNIX-like terminal or development environment.

### Compile

Run the following command to compile the code:

```bash
gcc -o hex_sim hex_simulation.c
