#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "Simplex.h"

using namespace std;

int main() {
    string optType;
    int numVars, numConstraints;

    cout << "Do you want to 'maximise' or 'minimise'? ";
    cin >> optType;
    
    transform(optType.begin(), optType.end(), optType.begin(), ::tolower);
    bool isMinimization = (optType == "minimise" || optType == "minimize" || optType == "min");

    cout << "Enter the number of variables (e.g., 2 for x1, x2): ";
    cin >> numVars;

    cout << "Enter the number of constraints: ";
    cin >> numConstraints;
    cin.ignore();

    vector<double> objFunc(numVars);
    cout << "\nType the coefficients of the initial problem separated by spaces." << endl;
    cout << "(Example: for 3x1 + 5x2, type '3 5'): ";
    string line;
    getline(cin, line);
    stringstream ssObj(line);
    for (int i = 0; i < numVars; i++) {
        ssObj >> objFunc[i];
    }

    vector<Constraint> constraints;
    cout << "\nFor each constraint, you will be asked for the coefficients, the operator, and the RHS." << endl;
    
    for (int i = 0; i < numConstraints; i++) {
        Constraint c;
        c.coeffs.resize(numVars);
        
        cout << "\n--- Constraint " << i + 1 << " ---" << endl;
        cout << "Coefficients (separated by spaces): ";
        getline(cin, line);
        stringstream ssConst(line);
        for (int j = 0; j < numVars; j++) {
            ssConst >> c.coeffs[j];
        }

        cout << "Operator (<=, <, >=, >, =): ";
        getline(cin, c.op);

        cout << "Right-Hand Side (RHS): ";
        string rhsStr;
        getline(cin, rhsStr);
        c.rhs = stod(rhsStr);

        constraints.push_back(c);
    }

    cout << "\nStarting Big-M Simplex Solver..." << endl;
    
    Simplex solver(objFunc, constraints, isMinimization);
    solver.solve();

    return 0;
}