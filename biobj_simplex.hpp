#ifndef BIOBJ_SIMPLEX
#define BIOBJ_SIMPLEX

#include <coin/OsiClpSolverInterface.hpp>
#include <coin/ClpSimplex.hpp>
#include <coin/CoinPackedVector.hpp>
#include <coin/CoinPackedMatrix.hpp>
#include <vector>
#include "../mo-utils/BVect.hpp"

class Biobj_simplex{
  private:
    OsiSolverInterface *model;
    int num_rows;
    int num_cols;
    double *obj1;
    double *obj2;
    //vector<double> b;

    double *cost1;
    double *cost2;
    int var_in;
    int var_out;
    double lambda;
    double *work_row;
    double *work_col;
    int* basics;

    vector<BVect> solutions;

    void updateCosts();

  public:
    Biobj_simplex();
    ~Biobj_simplex();
    double getInfinity();
    void loadProblem(const CoinPackedMatrix &matrix, const double *col_lb, const double *col_ub, const double *objective1, const double *objective2, const double *row_lb, const double *row_ub);
    void solve();
    vector<BVect> getSols();
    bool computeVarIn();
    void computeVarOut();

};

#endif
