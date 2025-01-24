import numpy as np
import matplotlib.pyplot as plt
import os

# Parameters
M = 30  # Grid rows
N = 10  # Grid columns
start_timestep = 0  # Starting timestep
num_timesteps = 10  # Number of timesteps to visualize
fps = 2  # Frames per second (for controlling display speed)

def read_macroscopic_file(filename):
    """Reads macroscopic quantities from a file."""
    density = np.zeros((M, N))
    velocity_x = np.zeros((M, N))
    velocity_y = np.zeros((M, N))
    
    with open(filename, "r") as file:
        for line in file:
            if not line.strip():
                continue  # Skip empty lines
            
            try:
                # Parse the line
                tokens = line.strip().split(":")
                coords = tokens[0].strip("()").split(",")
                row, col = int(coords[0]), int(coords[1])
                data = tokens[1].split(", Velocity=")
                density[row, col] = float(data[0].split("=")[1])
                velocity = data[1].strip("()").split(",")
                
                # Ensure velocity has two components
                if len(velocity) != 2:
                    raise ValueError(f"Invalid velocity format in line: {line}")
                
                velocity_x[row, col] = float(velocity[0].strip())
                velocity_y[row, col] = float(velocity[1].strip())
            
            except (IndexError, ValueError) as e:
                print(f"Error parsing line: {line}")
                print(f"Error: {e}")
                continue
    
    return density, velocity_x, velocity_y

# Loop through timesteps
for timestep in range(start_timestep, start_timestep + num_timesteps):
    filename = f"macroscopic_t{timestep}.txt"
    if not os.path.isfile(filename):
        print(f"File {filename} does not exist. Skipping...")
        continue
    
    # Read the macroscopic quantities
    density, velocity_x, velocity_y = read_macroscopic_file(filename)

    # Create a meshgrid for plotting
    x, y = np.meshgrid(range(N), range(M))

    # Display velocity field
    plt.figure(figsize=(10, 6))
    plt.quiver(x, y, velocity_x, velocity_y, color='r', angles='xy', scale_units='xy', scale=1)
    plt.title(f"Velocity Field at Timestep {timestep}", fontsize=14)
    plt.xlabel("Columns", fontsize=12)
    plt.ylabel("Rows", fontsize=12)
    plt.xlim(-1, N)
    plt.ylim(-1, M)
    plt.gca().invert_yaxis()  # Invert y-axis to match typical grid layout
    plt.grid()
    plt.tight_layout()
    plt.show(block=False)
    plt.pause(1 / fps)
    plt.close()

    # Display density map
    #plt.figure(figsize=(10, 6))
    #plt.imshow(density, cmap="jet", origin="upper")
    #plt.colorbar(label="Density")
    #plt.title(f"Density Map at Timestep {timestep}", fontsize=14)
    #plt.xlabel("Columns", fontsize=12)
    #plt.ylabel("Rows", fontsize=12)
    #plt.gca().invert_yaxis()  # Match grid layout
    #plt.tight_layout()
    #plt.show(block=False)
    #plt.pause(1 / fps)
    #plt.close()
