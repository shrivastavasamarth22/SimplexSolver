#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <regex>
#include "Simplex.h"

using namespace std;

int main() {
    cout << "--- Simplex Solver ---" << endl;
    cout << "Enter the objective function (e.g., 'max 3x1 - 4x2 + 5x3' or 'min x1 + 2x2'):\n> ";
    string objLine;
    getline(cin, objLine);

    bool isMinimization = false;
    string lowerObj = objLine;
    transform(lowerObj.begin(), lowerObj.end(), lowerObj.begin(), ::tolower);
    if (lowerObj.find("min") != string::npos) {
        isMinimization = true;
    }

    int maxVarIdx = 0;
    
    auto parseExpression = [&](const string& expr, vector<double>& coeffs) {
        regex termRegex(R"(([+-]?\s*\d*\.?\d*)\s*[xX](\d+))");
        sregex_iterator words_begin(expr.begin(), expr.end(), termRegex);
        sregex_iterator words_end;

        for (sregex_iterator i = words_begin; i != words_end; ++i) {
            smatch match = *i;
            string coeffStr = match[1].str();
            int varIdx = stoi(match[2].str()) - 1; // 1-based to 0-based
            
            coeffStr.erase(remove_if(coeffStr.begin(), coeffStr.end(), ::isspace), coeffStr.end());
            
            double coeff = 1.0;
            if (coeffStr == "-" || coeffStr == "+-") coeff = -1.0;
            else if (coeffStr == "+" || coeffStr == "") coeff = 1.0;
            else coeff = stod(coeffStr);
            
            maxVarIdx = max(maxVarIdx, varIdx + 1);
            if (varIdx >= coeffs.size()) coeffs.resize(varIdx + 1, 0.0);
            coeffs[varIdx] = coeff;
        }
    };

    vector<double> objFunc;
    parseExpression(objLine, objFunc);

    vector<Constraint> constraints;
    cout << "\nEnter constraints one by one (e.g., '3x1 + x2 <= 10')." << endl;
    cout << "Type 'done' or press Enter on an empty line to start solving:\n";
    
    while (true) {
        cout << "> ";
        string constLine;
        getline(cin, constLine);
        
        string lowerConst = constLine;
        transform(lowerConst.begin(), lowerConst.end(), lowerConst.begin(), ::tolower);
        
        // Trim whitespace from string
        lowerConst.erase(0, lowerConst.find_first_not_of(" \t\r\n"));
        if (lowerConst == "done" || lowerConst.empty()) break;

        regex opRegex(R"((<=|>=|<|>|=))");
        smatch opMatch;
        if (regex_search(constLine, opMatch, opRegex)) {
            Constraint c;
            c.op = opMatch[1].str();
            
            string lhs = constLine.substr(0, opMatch.position());
            string rhsStr = constLine.substr(opMatch.position() + opMatch.length());
            
            c.rhs = stod(rhsStr);
            parseExpression(lhs, c.coeffs);
            constraints.push_back(c);
        } else {
            cout << "Invalid constraint format. Expected an operator (<=, >=, <, >, =).\n";
        }
    }

    int numVars = maxVarIdx;
    
    // Ensure all vectors evaluate up to numVars
    objFunc.resize(numVars, 0.0);
    for (auto& c : constraints) {
        c.coeffs.resize(numVars, 0.0);
    }

    cout << "\nStarting Big-M Simplex Solver..." << endl;
    
    Simplex solver(objFunc, constraints, isMinimization);
    solver.solve();

    if (numVars == 2 && !solver.getIsUnbounded() && !solver.getIsInfeasible()) {
        cout << "\nExporting 2D data for visualization..." << endl;
        ofstream outFile("data.json");
        outFile << "{\n";
        outFile << "  \"isMinimization\": " << (isMinimization ? "true" : "false") << ",\n";
        
        outFile << "  \"objective\": [" << objFunc[0] << ", " << objFunc[1] << "],\n";
        
        outFile << "  \"constraints\": [\n";
        for (size_t i = 0; i < constraints.size(); ++i) {
            outFile << "    {\"coeffs\": [" << constraints[i].coeffs[0] << ", " << constraints[i].coeffs[1] << "], ";
            outFile << "\"op\": \"" << constraints[i].op << "\", ";
            outFile << "\"rhs\": " << constraints[i].rhs << "}";
            if (i < constraints.size() - 1) outFile << ",";
            outFile << "\n";
        }
        outFile << "  ],\n";

        vector<double> optVars = solver.getOptimalVariables();
        outFile << "  \"optimal_point\": [" << optVars[0] << ", " << optVars[1] << "],\n";
        outFile << "  \"optimal_value\": " << solver.getOptimalObjective() << "\n";
        outFile << "}\n";
        outFile.close();

        cout << "Launching Python visualizer..." << endl;
        system("python plot_simplex.py data.json");
    }

    return 0;
}