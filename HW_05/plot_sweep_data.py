import sys
import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

def create_3d_plot(H, A, Z, title, filename):
    fig = plt.figure(figsize=(12, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    surf = ax.plot_surface(H, A, Z, cmap='viridis', alpha=0.8)
    
    ax.set_xlabel('H', fontsize=12)
    ax.set_ylabel('A', fontsize=12)
    ax.set_zlabel('Rate (n/N)', fontsize=12)
    ax.set_title(title, fontsize=16)
    
    fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5, label='Rate')
    
    plt.savefig(filename, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"3D graph saved as {filename}")

def create_2d_combined_plot(data, title, filename):
    plt.figure(figsize=(12, 8))
    
    plt.plot(data['H'], data['t_n'], label='Transmitted', linestyle='-', color='blue')
    plt.plot(data['H'], data['a_n'], label='Absorbed', linestyle='-', color='green')
    plt.plot(data['H'], data['r_n'], label='Reflected', linestyle='-', color='magenta')
    
    plt.xlabel('H', fontsize=12)
    plt.ylabel('Fraction of n', fontsize=12)
    plt.title(title, fontsize=16)
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig(filename, bbox_inches='tight', dpi=300)
    plt.close()
    print(f"2D graph saved as {filename}")

def generate_graphs(input_filename):
    # Read the data
    try:
        data = pd.read_csv(input_filename)
    except FileNotFoundError:
        print(f"Error: The file '{input_filename}' was not found.")
        sys.exit(1)
    except pd.errors.EmptyDataError:
        print(f"Error: The file '{input_filename}' is empty.")
        sys.exit(1)
    except pd.errors.ParserError:
        print(f"Error: Unable to parse '{input_filename}'. Make sure it's a valid CSV file.")
        sys.exit(1)

    print("\nFirst few rows of raw data:")
    print(data.head())
    print("\nColumn names:")
    print(data.columns)

    # Calculate fractions
    data['t_n'] = data['tran'] / data['n']
    data['a_n'] = data['abs'] / data['n']
    data['r_n'] = data['refl'] / data['n']

    # Create a mesh grid for H and A for 3D plots
    H = np.unique(data['H'])
    A = np.unique(data['A'])
    H_mesh, A_mesh = np.meshgrid(H, A)

    # Reshape the data for the 3D surface plots
    Z_t = data.pivot(index='A', columns='H', values='t_n').values
    Z_a = data.pivot(index='A', columns='H', values='a_n').values
    Z_r = data.pivot(index='A', columns='H', values='r_n').values

    # Create and save 3D plots
    create_3d_plot(H_mesh, A_mesh, Z_t, 'Transmission Rate vs H and A', 'transmission_3d.png')
    create_3d_plot(H_mesh, A_mesh, Z_a, 'Absorption Rate vs H and A', 'absorption_3d.png')
    create_3d_plot(H_mesh, A_mesh, Z_r, 'Reflection Rate vs H and A', 'reflection_3d.png')

    # Create and save 2D combined plot
    # For the 2D plot, we'll use the data for a specific A value (e.g., the first A value)
    first_A = data['A'].iloc[0]
    data_for_2d = data[data['A'] == first_A].sort_values('H')
    create_2d_combined_plot(data_for_2d, f'Neutron Transport Fractions vs H (A = {first_A})', 'neutron_fractions_2d.png')

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python plot_sweep_data.py <input_filename>")
        sys.exit(1)
    
    input_filename = sys.argv[1]
    generate_graphs(input_filename)