import sys
import json
import numpy as np
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_simplex.py <data.json>")
        sys.exit(1)

    with open(sys.argv[1], 'r') as f:
        data = json.load(f)

    fig, ax = plt.subplots(figsize=(8, 6))

    # Grid and basic setup
    max_val = 0
    for c in data['constraints']:
        # To determine bounds, check intercepts
        for i in range(2):
            if abs(c['coeffs'][i]) > 1e-6:
                max_val = max(max_val, abs(c['rhs'] / c['coeffs'][i]))
    
    max_val = max(10, max_val * 1.5)  # give some padding
    x = np.linspace(0, max_val, 400)
    
    # Shade the feasible region
    # Start with a very large region in the first quadrant
    y1 = np.minimum(1e6, np.full_like(x, 1e6))
    y2 = np.maximum(0, np.zeros_like(x))
    
    for c in data['constraints']:
        c1, c2 = c['coeffs']
        op = c['op']
        rhs = c['rhs']

        if abs(c2) < 1e-6: # vertical line
            ax.axvline(rhs / c1, color='gray', linestyle='--')
        else: # y = (rhs - c1*x) / c2
            y_line = (rhs - c1 * x) / c2
            ax.plot(x, y_line, label=f"{c1}x1 + {c2}x2 {op} {rhs}")
            
            if op == '<' or op == '<=':
                y1 = np.minimum(y1, y_line)
            elif op == '>' or op == '>=':
                y2 = np.maximum(y2, y_line)

    # Fill feasible region
    plt.fill_between(x, y2, y1, where=(y1 >= y2) & (x >= 0) & (y1 >= 0),
                     color='lightblue', alpha=0.5, label='Feasible Region')

    # Plot optimal point
    opt_x1, opt_x2 = data['optimal_point']
    ax.plot(opt_x1, opt_x2, 'r*', markersize=15, label=f'Optimal Point\n({opt_x1:.2f}, {opt_x2:.2f})')

    # Plot objective function line passing through optimal point
    obj1, obj2 = data['objective']
    opt_val = data['optimal_value']
    if abs(obj2) > 1e-6:
        y_obj = (opt_val - obj1 * x) / obj2
        ax.plot(x, y_obj, 'r--', label=f'Objective Z = {opt_val:.2f}')

    plt.xlim(0, max((opt_x1 * 1.5), 10))
    plt.ylim(0, max((opt_x2 * 1.5), 10))

    plt.xlabel('x1')
    plt.ylabel('x2')
    plt.title('Simplex Method 2D Visualization')
    plt.axhline(0, color='black', linewidth=1)
    plt.axvline(0, color='black', linewidth=1)
    plt.grid(True, linestyle=':', alpha=0.6)
    plt.legend()
    
    plt.show()

if __name__ == "__main__":
    main()
