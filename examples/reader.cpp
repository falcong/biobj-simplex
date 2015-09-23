#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "biobj_simplex.hpp"

using namespace std;

/*
 * Reads the instance passed as parameter, solves it and displays the solutions
 */

int main (int argc, char** argv){
  cout << "Opening " << argv[1] << endl;
  int nb_objectives;
  int nb_variables;
  int nb_constraints;

  string line;

  ifstream myfile(argv[1]);
  if (myfile.is_open()) {
    getline (myfile, line);
    stringstream(line) >> nb_objectives >> nb_variables >> nb_constraints;

    Biobj_simplex solver;
    double *obj1 = new double[nb_variables]; 
    double *obj2 = new double[nb_variables];
    double *col_lb = new double[nb_variables];
    double *col_ub = new double[nb_variables];
    double *row_lb = new double[nb_constraints];
    double *row_ub = new double[nb_constraints];
    CoinPackedMatrix *matrix =  new CoinPackedMatrix(false,0,0);
    matrix->setDimensions(0, nb_variables);

    for (unsigned obj = 0; obj < nb_objectives; obj++) {
      getline (myfile, line);
      stringstream sline(line);
      for (unsigned var = 0; var < nb_variables; var++) {
        if (obj == 0)
          sline >> obj1[var];
        else
          sline >> obj2[var];
      }
    }

    for (unsigned var = 0; var < nb_variables; var++) {
      col_lb[var] = 0.0;
      col_ub[var] = solver.getInfinity();
    }

    for (unsigned constraint = 0; constraint < nb_constraints; constraint++) {
      getline (myfile, line);
      stringstream sline(line);
      CoinPackedVector row;
      for (unsigned var = 0; var < nb_variables; var++) {
        double value;
        sline >> value;
        row.insert(var, value);
      }
      row_lb[constraint] = -1.0 * solver.getInfinity();
      sline >> row_ub[constraint];
      matrix->appendRow(row);
    }


    solver.loadProblem(*matrix, col_lb, col_ub, obj1, obj2, row_lb, row_ub);
    solver.solve();

    vector<BVect> solutions = solver.getSols();
    cout << "The problem has " << solutions.size() << " non dominated points:" << endl;
    for(auto &solution: solutions) {
      cout << "\t" << solution.z1() << " " << solution.z2() << " |";
      for (auto &x: solution.x())
        cout << " " << x;
      cout << endl;
    }
  }
}
