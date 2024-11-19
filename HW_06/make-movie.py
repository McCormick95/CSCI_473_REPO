import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import argparse
import numpy as np
import seaborn as sns
import struct
from matplotlib.animation import FuncAnimation, PillowWriter


parser = argparse.ArgumentParser(description='get the name of the input and output file names')
parser.add_argument('input_file', type=str, help='the name of the input file')
parser.add_argument('output_file', type=str, help='the name of the output file')
args = parser.parse_args()

# Open the binary file for reading
with open(str(args.input_file), 'rb') as file:
    # Read the number of rows and columns from the file
    rows = struct.unpack('<i', file.read(4))[0]
    columns = struct.unpack('<i', file.read(4))[0]
    frames = struct.unpack('<i', file.read(4))[0]

    # Create a 3D array to store the data
    matrix = []

    # Read the matrix data from the file
    for frame in range(frames):
        matrix_frame = []
        for i in range(rows):
            row = []
            for j in range(columns):
                row.append(struct.unpack('<d', file.read(8))[0])
            matrix_frame.append(row)
        matrix.append(matrix_frame)

# Create a figure and axes for the animation
fig, ax = plt.subplots()
plt.subplots_adjust(bottom=0.2)
plt.xticks(np.arange(0, columns, 10), labels=np.arange(10, columns+1, 10))
plt.yticks(np.arange(0, rows, 10), labels=np.arange(10, rows+1, 10))

img = ax.imshow(matrix[0], cmap='coolwarm', vmin=0, vmax=1)

# The 'animate' function updates the heatmap for each frame.
def animate(frame):
    ax.clear()
    ax.set_title('Heatmap GIF')
    sns.heatmap(matrix[frame], ax=ax, cmap='coolwarm', vmin=0, vmax=1, cbar=False)

print("Creating animation...")

# Create the animation.
ani = FuncAnimation(fig, animate, frames=frames, interval=10, blit= False, repeat=True, 
                    repeat_delay=1000)

# Save the animation as a GIF.
# ani.save(args.output_file, writer=PillowWriter(fps=30))

ani.save(args.output_file, writer="ffmpeg", fps=30)

print("Animation created and saved as", args.output_file)


