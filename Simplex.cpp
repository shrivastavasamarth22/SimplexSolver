#include "Simplex.h"
#include <iostream>
#include <cmath>

using namespace std;

Simplex::Simplex(vector<double> obj, vector<Constraint> constraints, bool isMin) {
    isMinimization = isMin;
    M = 1000000.0;
    numVars = obj.size();
    isUnbounded = false;
    isInfeasible = false;
    optimalObjective = 0.0;
    optimalVariables.assign(numVars, 0.0);

    int numVars = obj.size();
    int numConstraints = constraints.size();
    
    int slackSurplusCount = 0;
    int artificialCount = 0;
    
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

    int currentSlack = numVars;
    int currentArt = numVars + slackSurplusCount;

    for (int i = 0; i < numConstraints; i++) {
        for (int j = 0; j < numVars; j++) {
            tableau[i][j] = constraints[i].coeffs[j];
        }
        tableau[i][cols - 1] = constraints[i].rhs;

        if (constraints[i].op == "<=" || constraints[i].op == "<") {
            tableau[i][currentSlack++] = 1.0;
        } 
        else if (constraints[i].op == ">=" || constraints[i].op == ">") {
            tableau[i][currentSlack++] = -1.0;
            tableau[i][currentArt] = 1.0;
            tableau[rows - 1][currentArt] = M;
            currentArt++;
        } 
        else if (constraints[i].op == "=" || constraints[i].op == "==") {
            tableau[i][currentArt] = 1.0;
            tableau[rows - 1][currentArt] = M;
            currentArt++;
        }
    }

    for (int j = 0; j < numVars; j++) {
        tableau[rows - 1][j] = isMinimization ? obj[j] : -obj[j];
    }

    for (int i = 0; i < numConstraints; i++) {
        if (constraints[i].op == ">=" || constraints[i].op == ">" || constraints[i].op == "=" || constraints[i].op == "==") {
            for (int j = 0; j < cols; j++) {
                tableau[rows - 1][j] -= M * tableau[i][j];
            }
        }
    }
}

void Simplex::solve() {
    while (true) {
        int pivotCol = findPivotColumn();
        
        if (pivotCol == -1) {
            cout << "\n=====================================" << endl;
            cout << "      Optimal Solution Reached!      " << endl;
            cout << "=====================================" << endl;
            break;
        }

        int pivotRow = findPivotRow(pivotCol);
        
        if (pivotRow == -1) {
            cout << "\nProblem is Unbounded! No finite solution exists." << endl;
            isUnbounded = true;
            return;
        }

        pivot(pivotRow, pivotCol);
    }
    
    printResults();
}

int Simplex::findPivotColumn() {
    int pivotCol = -1;
    double minValue = -1e-6;

    for (int j = 0; j < cols - 1; j++) {
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

    if (abs(optimalValue) > M / 10) {
        isInfeasible = true;
        cout << "WARNING: Problem may be INFEASIBLE. Artificial variables could not be eliminated." << endl;
    }

    optimalObjective = optimalValue;
    cout << "Optimal Objective Value (Z): " << optimalValue << "\n\n";
    cout << "--- Optimal Variable Values ---" << endl;

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
            cout << "x" << (j + 1) << " = " << tableau[oneRowIndex][cols - 1] << endl;
        } else {
            optimalVariables[j] = 0.0;
            cout << "x" << (j + 1) << " = 0" << endl;
        }
    }
    cout << "-------------------------------" << endl;
}