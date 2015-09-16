#include "../src/biobj_simplex.hpp"

using namespace std;

int main() {
  Biobj_simplex solver;

  /*
   * Problem from Ehrgott (2005), page 162
   *  
   *  minimize  3 x0 + 1 x1
   *           -1 x0 - 2 x1
   *  s.t                x1 <= 3
   *            3 x0 - 1 x1 <= 6
   *              x0        >= 0
   *              x1        >= 0
   *
   *  extreme efficient solutions are:
   *  - a = ( 0,  0) with x0 = 0 and x1 = 0
   *  - b = ( 3, -6) with x0 = 0 and x1 = 3
   *  - c = (12, -9) with x0 = 3 and x1 = 3
   */

   int n_cols = 2;
   double *objective1   = new double[n_cols];//first objective coefficients
   double *objective2   = new double[n_cols];//second objective coefficients
   double *col_lb       = new double[n_cols];//the column lower bounds
   double *col_ub       = new double[n_cols];//the column upper bounds

   //Define the objective coefficients.
   //minimize  3 x0 + 1 x1
   objective1[0] = 3.0;
   objective1[1] = 1.0;
   //minimize -1 x0 - 2 x1
   objective2[0] = -1.0;
   objective2[1] = -2.0;

   //Define the variable lower/upper bounds.
   // x0 >= 0   =>  0 <= x0 <= infinity
   // x1 >= 0   =>  0 <= x1 <= infinity
   col_lb[0] = 0.0;
   col_lb[1] = 0.0;
   col_ub[0] = solver.getInfinity();
   col_ub[1] = solver.getInfinity();
     
   int n_rows = 2;
   double *row_lb = new double[n_rows]; //the row lower bounds
   double *row_ub = new double[n_rows]; //the row upper bounds
     
   //Define the constraint matrix.
   CoinPackedMatrix *matrix =  new CoinPackedMatrix(false,0,0);
   matrix->setDimensions(0, n_cols);

   //1 x1 <= 3  =>  -infinity <= 1 x1 <= 3
   CoinPackedVector row1;
   row1.insert(0, 0.0);
   row1.insert(1, 1.0);
   row_lb[0] = -1.0 * solver.getInfinity();
   row_ub[0] = 3.0;
   matrix->appendRow(row1);

   //3 x0 - 1 x1 <= 6  =>  -infinity <= 3 x0 - 1 x1 <= 6
   CoinPackedVector row2;
   row2.insert(0, 3.0);
   row2.insert(1, -1.0);
   row_lb[1] = -1.0 * solver.getInfinity();
   row_ub[1] = 6.0;
   matrix->appendRow(row2);

   solver.loadProblem(*matrix, col_lb, col_ub, objective1, objective2, row_lb, row_ub);
   solver.solve();

   vector<BVect> solutions = solver.getSols();
   cout << "The problem has " << solutions.size() << " non dominated points:" << endl;
   for(auto& solution : solutions) {
    cout << "\t" << solution.z1() << " " << solution.z2() << " | "
      << solution.x().at(0) << " " << solution.x().at(1) << endl;
   }

   return 0;
}
