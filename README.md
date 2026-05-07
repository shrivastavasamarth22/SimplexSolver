# 🧮 Simplex & ILP Solver

![C++](https://img.shields.io/badge/C++-11%2B-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

A lightweight, terminal-based C++ application that solves complex continuous and integer linear programming problems using the **Two-Phase Simplex Method** and **Branch & Bound** algorithms. 

## ✨ Features

- **Natural Math Input**: Just type equations like `max 3x1 - 4x2` and `2x1 + x2 <= 20` directly.
- **Two-Phase Simplex Method**: Eliminates the numerical instability of Big-M by perfectly tracking and minimizing artificial variables in a dedicated first phase.
- **Integer Linear Programming (ILP)**: Includes a Branch & Bound solver to restrict your decision variables to whole integers automatically.
- **Sensitivity Analysis**: Reports Shadow Prices (Dual Values) for all constraints, determining the marginal utility of relaxing them.
- **2D Visualization**: Automatically generates beautiful graphs of the feasible region and optimal point for 2-variable problems using Python and Matplotlib.
- **Maximization & Minimization**: Supports both optimization types dynamically derived from your input.
- **Flexible Constraints**: Handles `<=`, `>=`, and `=` constraints seamlessly.
- **Robust Verification**: Computes exactly if a problem is mathematically unbounded or strictly infeasible.

## 🚀 Getting Started

### Prerequisites
- A standard C++ compiler (e.g., GCC, Clang, MSVC) supporting C++11 or higher.
- *(Optional)* Python 3 and `matplotlib` if you want to use the 2D visualizer for 2-variable problems. `pip install numpy matplotlib`

### Compilation
Run the following command in the project directory to build the executable:

```bash
g++ -o SimplexSolver main.cpp Simplex.cpp
```

### Usage
Execute the compiled binary:

```bash
./SimplexSolver
```

Follow the interactive prompts to define your objective function, variables, and constraints:
```text
--- Simplex Solver ---
Enter the objective function (e.g., 'max 3x1 - 4x2 + 5x3' or 'min x1 + 2x2'):
> max 3x1 + 4.5x2

Enter constraints one by one (e.g., '3x1 + x2 <= 10').
Type 'done' or press Enter on an empty line to start solving:
> 2x1 + x2 <= 20
> x1 - x2 >= 5
> 

Do you want to restrict variables to integers? (ILP Branch & Bound) [y/n]: y

--- Starting Branch & Bound ILP Solver ---
Branch and Bound search completed!
Nodes explored: 3
...
```

## 📝 License

This project is licensed under the [MIT License](LICENSE).
