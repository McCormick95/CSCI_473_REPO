import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import AutoMinorLocator, AutoLocator
import os
import argparse
import sys
import numpy as np

def parse_arguments():
    parser = argparse.ArgumentParser(description='Analyze performance data from stencil implementations.')
    parser.add_argument('csv_file', help='Path to the CSV file containing performance data')
    parser.add_argument('-o', '--output', default='plots', 
                        help='Directory to save output plots (default: plots)')
    return parser.parse_args()

def format_time_ticks(x, p):
    """Format time values for tick labels"""
    if x < 1:
        return f'{x:.2f}'
    elif x < 10:
        return f'{x:.1f}'
    else:
        return f'{int(x)}'

def prepare_data(csv_file):
    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: Could not find the CSV file '{csv_file}'")
        sys.exit(1)
    except Exception as e:
        print(f"Error processing the CSV file: {e}")
        sys.exit(1)
    
    df['matrix_size'] = df['ROWS'].astype(str) + 'x' + df['COLS'].astype(str)
    df['size_num'] = df['ROWS'].astype(int)
    
    results = []
    for size in df['matrix_size'].unique():
        size_data = df[df['matrix_size'] == size].copy()
        serial_time = size_data[size_data['RUN_TYPE'] == 'SERIAL']['TOTAL'].iloc[0]
        size_data['speedup'] = serial_time / size_data['TOTAL']
        size_data['efficiency'] = size_data['speedup'] / size_data['P']
        results.append(size_data)
    
    return pd.concat(results)

def setup_y_axis(plt, data_min, data_max, is_log=True):
    """Setup y-axis with appropriate tick marks and labels"""
    if is_log:
        # For log scale, generate ticks at more intermediate points
        log_min = np.floor(np.log10(data_min))
        log_max = np.ceil(np.log10(data_max))
        
        major_ticks = []
        minor_ticks = []
        
        for exp in range(int(log_min), int(log_max + 1)):
            major_ticks.append(10**exp)
            if exp < log_max:
                for i in range(2, 10):
                    minor_ticks.append(i * 10**exp)
        
        plt.gca().yaxis.set_major_locator(plt.FixedLocator(major_ticks))
        plt.gca().yaxis.set_minor_locator(plt.FixedLocator(minor_ticks))
        plt.gca().yaxis.set_major_formatter(plt.FuncFormatter(format_time_ticks))
    else:
        # For linear scale, use AutoLocator with more ticks
        plt.gca().yaxis.set_major_locator(AutoLocator())
        plt.gca().yaxis.set_minor_locator(AutoMinorLocator())
        plt.gca().yaxis.set_major_formatter(plt.FuncFormatter(format_time_ticks))

def create_metric_plots(df, metric, ylabel, title_prefix, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    
    plt.style.use('default')
    colors = ['blue', 'green', 'red', 'purple', 'orange', 'brown']
    markers = ['o', 's', '^', 'D', 'v', 'p']
    
    for run_type in ['OMP', 'PTH']:
        plt.figure(figsize=(12, 8))
        
        impl_data = df[df['RUN_TYPE'] == run_type]
        
        max_threads = max(impl_data['P'])
        if metric == 'speedup':
            x_ideal = range(1, max_threads + 1)
            plt.plot(x_ideal, x_ideal, 'k--', label='Ideal Speedup', alpha=0.7)
        elif metric == 'efficiency':
            plt.axhline(y=1, color='k', linestyle='--', label='Ideal Efficiency', alpha=0.7)
        
        for (size, color, marker) in zip(sorted(df['matrix_size'].unique()), colors, markers):
            size_data = impl_data[impl_data['matrix_size'] == size]
            
            if not size_data.empty:
                plt.plot(size_data['P'], size_data[metric], 
                        marker=marker, 
                        color=color,
                        label=f'Matrix Size {size}',
                        linewidth=2,
                        markersize=8)
        
        if metric == 'TOTAL':
            plt.yscale('log')
            setup_y_axis(plt, impl_data[metric].min(), impl_data[metric].max(), True)
            
            for size, color in zip(sorted(df['matrix_size'].unique()), colors):
                serial_data = df[(df['RUN_TYPE'] == 'SERIAL') & (df['matrix_size'] == size)]
                if not serial_data.empty:
                    serial_time = serial_data['TOTAL'].iloc[0]
                    plt.axhline(y=serial_time, color=color, linestyle='--', 
                              alpha=0.5, linewidth=1)
        else:
            setup_y_axis(plt, impl_data[metric].min(), impl_data[metric].max(), False)
        
        plt.title(f"{title_prefix} - {run_type} Implementation")
        plt.xlabel("Number of Threads/Processes")
        plt.ylabel(ylabel)
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        
        plt.xticks(sorted(df['P'].unique()))
        
        if metric in ['speedup', 'efficiency']:
            plt.ylim(bottom=0)
        
        plt.tight_layout()
        plt.savefig(f"{output_dir}/{metric.lower()}_{run_type}.png", 
                   dpi=300, bbox_inches='tight')
        plt.close()

def create_component_time_plots(df, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    
    # Create separate plots for OMP and PTH
    for run_type in ['OMP', 'PTH']:
        plt.figure(figsize=(15, 8))
        
        impl_data = df[df['RUN_TYPE'] == run_type]
        matrix_sizes = sorted(impl_data['matrix_size'].unique())
        thread_counts = sorted(impl_data['P'].unique())
        
        num_sizes = len(matrix_sizes)
        bar_width = 0.35
        
        for i, size in enumerate(matrix_sizes):
            size_data = impl_data[impl_data['matrix_size'] == size].sort_values('P')
            x = range(len(thread_counts))
            
            offset = i * bar_width
            plt.bar([p + offset for p in x], 
                   size_data['WORK'], 
                   bar_width/num_sizes,
                   label=f'{size} Work',
                   alpha=0.8)
            
            plt.bar([p + offset for p in x], 
                   size_data['OTHER'],
                   bar_width/num_sizes,
                   bottom=size_data['WORK'],
                   label=f'{size} Other',
                   alpha=0.8)
        
        plt.xlabel('Number of Threads/Processes')
        plt.ylabel('Time (seconds)')
        plt.title(f'Time Components Analysis - {run_type} Implementation')
        plt.xticks([p + bar_width/2 for p in x], thread_counts)
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(True, linestyle='--', alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(f'{output_dir}/time_components_{run_type}.png', 
                   dpi=300, bbox_inches='tight')
        plt.close()

def print_summary(df):
    print("\nPerformance Summary:")
    for run_type in ['OMP', 'PTH']:
        print(f"\n{run_type} Implementation:")
        for size in sorted(df['matrix_size'].unique()):
            impl_data = df[(df['RUN_TYPE'] == run_type) & (df['matrix_size'] == size)]
            if not impl_data.empty:
                best_time = impl_data['TOTAL'].min()
                best_threads = impl_data.loc[impl_data['TOTAL'].idxmin(), 'P']
                best_speedup = impl_data['speedup'].max()
                
                print(f"\nMatrix Size: {size}")
                print(f"  Best time: {best_time:.3f} seconds (with {best_threads} threads)")
                print(f"  Max speedup: {best_speedup:.2f}x")

def create_timing_breakdown_plots(df, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    
    # Colors and line styles for different timing components
    time_components = {
        'TOTAL': ('blue', 'o', 'Total Time'),
        'WORK': ('green', 's', 'Work Time'),
        'OTHER': ('red', '^', 'Other Time')
    }
    
    # Create separate plots for each implementation and matrix size
    for run_type in ['OMP', 'PTH']:
        for size in sorted(df['matrix_size'].unique()):
            plt.figure(figsize=(12, 8))
            
            # Filter data for this implementation and matrix size
            plot_data = df[(df['RUN_TYPE'] == run_type) & (df['matrix_size'] == size)]
            
            if not plot_data.empty:
                # Plot each timing component
                for component, (color, marker, label) in time_components.items():
                    plt.plot(plot_data['P'], plot_data[component], 
                            marker=marker, 
                            color=color,
                            label=label,
                            linewidth=2,
                            markersize=8)
                
                plt.title(f'Timing Breakdown - {run_type} Implementation\nMatrix Size {size}')
                plt.xlabel('Number of Threads/Processes')
                plt.ylabel('Time (seconds)')
                plt.grid(True, linestyle='--', alpha=0.7)
                plt.legend()
                
                # Use log scale for better visibility of different time scales
                plt.yscale('log')
                
                # Set x-axis to show all thread counts
                plt.xticks(sorted(plot_data['P'].unique()))
                
                plt.tight_layout()
                plt.savefig(f"{output_dir}/timing_breakdown_{run_type}_{size}.png", 
                           dpi=300, bbox_inches='tight')
                plt.close()

def calculate_e_values(df):
    """Calculate e_overall and e_computation for each implementation, matrix size, and thread count"""
    e_values = []
    
    for run_type in ['OMP', 'PTH']:
        for size in sorted(df['matrix_size'].unique()):
            data = df[(df['RUN_TYPE'] == run_type) & (df['matrix_size'] == size)]
            if not data.empty:
                # Calculate for each thread count > 1
                for _, row in data[data['P'] > 1].iterrows():
                    p = row['P']
                    speedup = row['speedup']
                    
                    # Calculate work speedup
                    serial_work = data[data['P'] == 1]['WORK'].iloc[0]
                    work_speedup = serial_work / row['WORK']
                    
                    # Calculate e values
                    e_overall = (1/speedup - 1/p)/(1 - 1/p)
                    e_computation = (1/work_speedup - 1/p)/(1 - 1/p)
                    
                    e_values.append({
                        'Implementation': run_type,
                        'Matrix_Size': size,
                        'Threads': p,
                        'e_overall': e_overall,
                        'e_computation': e_computation
                    })
    
    return pd.DataFrame(e_values)

def create_e_value_plots(df, e_df, output_dir):
    """Create plots for e_overall and e_computation trends"""
    os.makedirs(output_dir, exist_ok=True)
    
    # Colors and markers for different matrix sizes
    colors = ['blue', 'green', 'red', 'purple']
    markers = ['o', 's', '^', 'D']
    
    # Create plots for each implementation and metric type
    for run_type in ['OMP', 'PTH']:
        for metric in ['e_overall', 'e_computation']:
            plt.figure(figsize=(12, 8))
            
            # Plot trend for each matrix size
            impl_data = e_df[e_df['Implementation'] == run_type]
            
            for (size, color, marker) in zip(sorted(df['matrix_size'].unique()), colors, markers):
                size_data = impl_data[impl_data['Matrix_Size'] == size]
                if not size_data.empty:
                    plt.plot(size_data['Threads'], size_data[metric],
                            marker=marker,
                            color=color,
                            label=f'Matrix Size {size}',
                            linewidth=2,
                            markersize=8)
            
            plt.title(f'{metric} Values - {run_type} Implementation')
            plt.xlabel('Number of Threads')
            plt.ylabel(f'{metric} Value')
            plt.grid(True, linestyle='--', alpha=0.7)
            plt.legend()
            
            # Set x-ticks to thread counts
            plt.xticks(sorted(impl_data['Threads'].unique()))
            
            plt.tight_layout()
            plt.savefig(f"{output_dir}/{metric}_{run_type}.png", dpi=300, bbox_inches='tight')
            plt.close()
            
            # Print values
            print(f"\n{metric} Values for {run_type}:")
            print("=" * 80)
            for size in sorted(df['matrix_size'].unique()):
                size_data = impl_data[impl_data['Matrix_Size'] == size]
                print(f"\nMatrix Size: {size}")
                print("Threads\t{metric}")
                print("-" * 40)
                for _, row in size_data.iterrows():
                    print(f"{row['Threads']}\t{row[metric]:.3f}")

def create_size_comparison_plot(df, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    
    plt.figure(figsize=(12, 8))
    
    implementations = {
        'SERIAL': ('blue', 'o', 'Serial Implementation'),
        'OMP': ('green', 's', 'OpenMP Implementation'),
        'PTH': ('red', '^', 'Pthreads Implementation')
    }
    
    all_times = []
    
    for run_type, (color, marker, label) in implementations.items():
        if run_type in ['OMP', 'PTH']:
            best_times = df[df['RUN_TYPE'] == run_type].groupby('size_num')['TOTAL'].min()
            plt.plot(best_times.index, best_times.values,
                    marker=marker,
                    color=color,
                    label=f'{label} (Best Parallel)',
                    linewidth=2,
                    markersize=8)
            all_times.extend(best_times.values)
            
            single_thread = df[(df['RUN_TYPE'] == run_type) & (df['P'] == 1)]
            plt.plot(single_thread['size_num'], single_thread['TOTAL'],
                    marker=marker,
                    color=color,
                    linestyle='--',
                    label=f'{label} (Single Thread)',
                    linewidth=2,
                    markersize=8)
            all_times.extend(single_thread['TOTAL'].values)
        else:
            serial_data = df[df['RUN_TYPE'] == run_type]
            plt.plot(serial_data['size_num'], serial_data['TOTAL'],
                    marker=marker,
                    color=color,
                    label=label,
                    linewidth=2,
                    markersize=8)
            all_times.extend(serial_data['TOTAL'].values)
    
    plt.title('Execution Time vs Matrix Size Comparison')
    plt.xlabel('Matrix Size (N for NxN matrix)')
    plt.ylabel('Execution Time (seconds)')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    
    plt.xscale('log')
    plt.yscale('log')
    
    # Setup enhanced y-axis ticks
    setup_y_axis(plt, min(all_times), max(all_times), True)
    
    sizes = sorted(df['size_num'].unique())
    plt.xticks(sizes, sizes)
    
    plt.tight_layout()
    plt.savefig(f"{output_dir}/size_comparison.png", dpi=300, bbox_inches='tight')
    plt.close()

def main():
    args = parse_arguments()
    
    print(f"Reading data from: {args.csv_file}")
    df = prepare_data(args.csv_file)
    
    print(f"Generating plots in directory: {args.output}")

    e_df = calculate_e_values(df)
    
    create_metric_plots(df, 'TOTAL', 'Execution Time (seconds)', 'Execution Time', args.output)
    create_metric_plots(df, 'speedup', 'Speedup (T₁/Tₚ)', 'Speedup', args.output)
    create_metric_plots(df, 'efficiency', 'Efficiency (Speedup/P)', 'Efficiency', args.output)
    create_component_time_plots(df, args.output)
    create_timing_breakdown_plots(df, args.output)
    create_e_value_plots(df, e_df, args.output)
    create_size_comparison_plot(df, args.output)
    
    print_summary(df)
    
    print(f"\nPlots have been saved in the '{args.output}' directory")

if __name__ == "__main__":
    main()