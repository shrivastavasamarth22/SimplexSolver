#include "Simplex.h"
#include <iostream>
#include <cmath>

using namespace std;

Simplex::Simplex(vector<double> obj, vector<Constraint> constraints, bool isMin, bool quietFlag) {
    isMinimization = isMin;
    quiet = quietFlag;
    M = 0; // Big-M is no longer used, we use Two-Phase
    numVars = obj.size();
    isUnbounded = false;
    isInfeasible = false;
    optimalObjective = 0.0;
    optimalVariables.assign(numVars, 0.0);
    originalObjective = obj;

    int numConstraints = constraints.size();
    
    int slackSurplusCount = 0;
    artificialCount = 0;
    
    for(const auto& c : constraints) {
        if(c.op == "<=" || c.op == "<") {
            slackSurplusCount++;
        } else if(c.op == ">=" || c.op == ">") {
            slackSurplusCount++;
            artificialCount++;
        } else if(c.op == "=" || c.op == "==") {
            artificialCount++;
        }
    }

    cols = numVars + slackSurplusCount + artificialCount + 1;
    rows = numConstraints + 1;
    tableau.assign(rows, vector<double>(cols, 0.0));
    constraintSlackCols.assign(numConstraints, -1);

    int currentSlack = numVars;
    artificialStartCol = numVars + slackSurplusCount;
    int currentArt = artificialStartCol;

    for (int i = 0; i < numConstraints; i++) {
        for (int j = 0; j < numVars; j++) {
            tableau[i][j] = constraints[i].coeffs[j];
        }
        tableau[i][cols - 1] = constraints[i].rhs;

        if (constraints[i].op == "<=" || constraints[i].op == "<") {
            constraintSlackCols[i] = currentSlack;
            tableau[i][currentSlack++] = 1.0;
        } 
        else if (constraints[i].op == ">=" || constraints[i].op == ">") {
            constraintSlackCols[i] = currentSlack;
            tableau[i][currentSlack++] = -1.0;
            tableau[i][currentArt] = 1.0;
            currentArt++;
        } 
        else if (constraints[i].op == "=" || constraints[i].op == "==") {
            tableau[i][currentArt] = 1.0;
            currentArt++;
        }
    }
}

bool Simplex::runSimplexPhase() {
    while (true) {
        int pivotCol = findPivotColumn();
        
        if (pivotCol == -1) {
            return true; // Optimal reached for this phase
        }

        int pivotRow = findPivotRow(pivotCol);
        
        if (pivotRow == -1) {
            isUnbounded = true;
            return false;
        }

        pivot(pivotRow, pivotCol);
    }
}

void Simplex::solve() {
    if (artificialCount > 0) {
        if (!quiet) cout << "\nStarting Phase I..." << endl;
        // Phase 1 objective: Minimize sum of artificial variables -> Maximize -sum(art_vars)
        for (int j = 0; j < cols; j++) {
            tableau[rows - 1][j] = 0.0;
        }
        for (int j = artificialStartCol; j < artificialStartCol + artificialCount; j++) {
            tableau[rows - 1][j] = 1.0; // minimization cost
        }

        // Zero out the costs of basic variables (artificials) in the objective row
        for (int i = 0; i < rows - 1; i++) {
            bool hasArt = false;
            for (int j = artificialStartCol; j < artificialStartCol + artificialCount; j++) {
                if (tableau[i][j] == 1.0) hasArt = true;
            }
            if (hasArt) {
                for (int j = 0; j < cols; j++) {
                    tableau[rows - 1][j] -= tableau[i][j];
                }
            }
        }

        if (!runSimplexPhase()) {
            if (!quiet) cout << "\nProblem is Unbounded in Phase I! (Should not happen)" << endl;
            return;
        }

        double phase1Obj = tableau[rows - 1][cols - 1];
        if (abs(phase1Obj) > 1e-6) {
            if (!quiet) cout << "\nProblem is INFEASIBLE. Artificial variables could not be eliminated." << endl;
            isInfeasible = true;
            return;
        }
        
        if (!quiet) cout << "Phase I Complete. Artificial variables minimized." << endl;
    }

    if (!quiet) cout << "\nStarting Phase II..." << endl;
    // Set up original objective
    for (int j = 0; j < cols; j++) {
        tableau[rows - 1][j] = 0.0;
    }
    
    // Ignore artificial columns by setting their cost to 0 and preventing entry
    // By convention, we can just not iterate through them when finding pivot
    // We update findPivotColumn to ignore artificial variables in phase 2

    for (int j = 0; j < numVars; j++) {
        tableau[rows - 1][j] = isMinimization ? originalObjective[j] : -originalObjective[j];
    }

    // Restore zero-cost for current basic variables
    for (int i = 0; i < rows - 1; i++) {
        int basicCol = -1;
        for (int j = 0; j < artificialStartCol; j++) { // Only look at valid vars
            if (abs(tableau[i][j] - 1.0) < 1e-6) {
                // Check if it's the ONLY 1 in the column
                bool isBasic = true;
                for (int k = 0; k < rows - 1; k++) {
                    if (k != i && abs(tableau[k][j]) > 1e-6) {
                        isBasic = false;
                        break;
                    }
                }
                if (isBasic) {
                    basicCol = j;
                    break;
                }
            }
        }

        if (basicCol != -1) {
            double cost = tableau[rows - 1][basicCol];
            if (abs(cost) > 1e-6) {
                for (int j = 0; j < cols; j++) {
                    tableau[rows - 1][j] -= cost * tableau[i][j];
                }
            }
        }
    }

    if (!runSimplexPhase()) {
        if (!quiet) cout << "\nProblem is Unbounded! No finite solution exists." << endl;
        return;
    }
    
    if (!quiet) {
        cout << "\n=====================================" << endl;
        cout << "      Optimal Solution Reached!      " << endl;
        cout << "=====================================" << endl;
    }
    printResults();
}

int Simplex::findPivotColumn() {
    int pivotCol = -1;
    double minValue = -1e-6;

    // Only search through non-artificial columns if we are in Phase II
    // We infer we are in phase II if artificial cost in objective is wiped, but 
    // to be totally safe we can just limit the search bounds.
    int limitCol = (tableau[rows-1][artificialStartCol] == 1.0) ? (cols - 1) : artificialStartCol;
    
    // If no artificial variables remain or we are in Phase II, limit to artificialStartCol
    // Actually, setting limit to cols - 1 during Phase I and artificialStartCol in Phase II is correct.
    for (int j = 0; j < limitCol; j++) {
        if (tableau[rows - 1][j] < minValue) {
            minValue = tableau[rows - 1][j];
            pivotCol = j;
        }
    }
    return pivotCol;
}

int Simplex::findPivotRow(int pivotCol) {
    int pivotRow = -1;
    double minRatio = -1.0;

    for (int i = 0; i < rows - 1; i++) {
        if (tableau[i][pivotCol] > 1e-6) {
            double ratio = tableau[i][cols - 1] / tableau[i][pivotCol];
            if (minRatio == -1.0 || ratio < minRatio) {
                minRatio = ratio;
                pivotRow = i;
            }
        }
    }
    return pivotRow;
}

void Simplex::pivot(int pivotRow, int pivotCol) {
    double pivotValue = tableau[pivotRow][pivotCol];

    for (int j = 0; j < cols; j++) {
        tableau[pivotRow][j] /= pivotValue;
    }

    for (int i = 0; i < rows; i++) {
        if (i != pivotRow) {
            double factor = tableau[i][pivotCol];
            for (int j = 0; j < cols; j++) {
                tableau[i][j] -= factor * tableau[pivotRow][j];
            }
        }
    }
}

void Simplex::printResults() {
    double optimalValue = tableau[rows - 1][cols - 1];
    
    if (isMinimization) optimalValue *= -1;

    optimalObjective = optimalValue;
    if (!quiet) cout << "Optimal Objective Value (Z): " << optimalValue << "\n\n";
    if (!quiet) cout << "--- Optimal Variable Values ---" << endl;

    for (int j = 0; j < numVars; j++) {
        int oneCount = 0;
        int zeroCount = 0;
        int oneRowIndex = -1;

        for (int i = 0; i < rows - 1; i++) { 
            if (abs(tableau[i][j] - 1.0) < 1e-6) {
                oneCount++;
                oneRowIndex = i;
            } else if (abs(tableau[i][j]) < 1e-6) {
                zeroCount++;
            }
        }

        if (oneCount == 1 && zeroCount == (rows - 2)) {
            optimalVariables[j] = tableau[oneRowIndex][cols - 1];
            if (!quiet) cout << "x" << (j + 1) << " = " << tableau[oneRowIndex][cols - 1] << endl;
        } else {
            optimalVariables[j] = 0.0;
            if (!quiet) cout << "x" << (j + 1) << " = 0" << endl;
        }
    }

    shadowPrices.assign(rows - 1, 0.0);
    if (!quiet) cout << "\n--- Shadow Prices (Sensitivity Analysis) ---" << endl;
    for (int i = 0; i < rows - 1; i++) {
        if (constraintSlackCols[i] != -1) {
            double sp = tableau[rows - 1][constraintSlackCols[i]];
            // We want shadow prices generally to be positive for relaxed inequalities
            // Some formulas vary depending on the minimization vs maximization
            if (isMinimization) sp = -sp; 
            if (abs(sp) < 1e-6) sp = 0.0; // avoid -0.0
            
            shadowPrices[i] = sp;
            if (!quiet) cout << "Constraint " << (i + 1) << " Shadow Price: " << sp << endl;
        } else {
            if (!quiet) cout << "Constraint " << (i + 1) << " Shadow Price: N/A (Equality Constraint)" << endl;
        }
    }
    if (!quiet) cout << "--------------------------------------------" << endl;
}