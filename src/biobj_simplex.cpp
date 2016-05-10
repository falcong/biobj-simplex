#include "biobj_simplex.hpp"

using namespace std;

Biobj_simplex::Biobj_simplex() {
  model = new OsiClpSolverInterface();
}


Biobj_simplex::~Biobj_simplex() {
//  delete model;
  delete[] cost1;
  delete[] cost2;
//  delete[] work_col;
//  delete[] basics;
}


double Biobj_simplex::getInfinity() {
  return model->getInfinity();
}


void Biobj_simplex::loadProblem(const CoinPackedMatrix &matrix,
    const double *col_lb,
    const double *col_ub,
    const double *objective1,
    const double *objective2,
    const double *row_lb,
    const double *row_ub) {
  num_cols = matrix.getNumCols();
  num_rows = matrix.getNumRows();

  // load the problem as a single objective one
  model->loadProblem(matrix, col_lb, col_ub, objective1, row_lb, row_ub);
  // artificially add the second objective function as a dummy ubounded constraint
  CoinPackedVector obj2(num_cols, objective2);
  model->addRow(obj2, -1.0 * model->getInfinity(), model->getInfinity());

  // allocate memory for working arrays
  cost1 = new double[num_cols + num_rows];
  cost2 = new double[num_cols + num_rows];
  work_col = new double[num_cols];
  basics = new int[num_rows];

  // tell Osi and Clp to be quiet
  model->messageHandler()->setLogLevel(0);
}


void Biobj_simplex::updateCosts() {
  // the first array of reduced costs
  //TODO copy all array at once
  for (int i = 0; i < num_cols; i++) {
    cost1[i] = model->getReducedCost()[i];
  }
  for (int i = 0; i < num_rows; i++) {
    cost1[num_cols + i] = -model->getRowPrice()[i];
  }

  // get the second array of reduced costs from the dummy constraint
  model->getBInvARow(num_rows, cost2, (double*)(cost2 + num_cols));
}


void Biobj_simplex::solve() {
  // phases 1 and 2: check whether the problem is feasible and compute
  // the first efficient solution
  model->initialSolve();

  // tell model to allow us to do extra things
  model->enableSimplexInterface(true);

  // gather data to update reduced costs
  const double* upper = model->getRowUpper();
  const double* solution = model->getColSolution();
  const double* activity = model->getRowActivity();
  updateCosts();

  model->getBasics(basics);

  // manually compute b values of the simplex tableau
  vector<double> b;
  for ( int i = 0; i < num_rows; i++ ) {
    // the value depends on whether i is in the basis the index of a
    // regular variable ( < num_cols ) or a slack variable ( >= )
    if(basics[i] < num_cols)
      b.push_back(solution[basics[i]]);
    else
      b.push_back(upper[i] - activity[i]);
  }
  // gather in s the values in decision space of the current solution
  vector<double> s (solution, solution + num_cols);

  // create the corresponding solution with its objective values
  BVect sol(/*-1.0 **/ model->getObjValue(), /*-1.0 **/ activity[num_rows], s);
  solutions.push_back(sol);

  // compute candidate variables to enter the basis
  while (computeVarIn()){
    // while at least one such variables does exist, 
    // compute the corresponding leaving variable
    computeVarOut();

    // perform pivot
    int outstatus = 1;
    model->pivot(var_in, basics[var_out], outstatus);

    // update data used to compute reduced costs
    upper = model->getRowUpper();
    solution = model->getColSolution();
    activity = model->getRowActivity();
    model->getBasics(basics);
    for ( int i = 0; i < num_rows; i++ ) {
      if(basics[i] < num_cols)
        b.push_back(solution[basics[i]]);
      else
        b.push_back(upper[i] - activity[i]);
    }
    // create the newly obtained solution
    vector<double> s (solution, solution + num_cols);

    BVect sol2(/*-1.0 **/ model->getObjValue(), /*-1.0 **/ activity[num_rows], s);
    solutions.push_back(sol2);
    updateCosts();
  }
}


vector<BVect> Biobj_simplex::getSols() {
  return solutions;
}


bool Biobj_simplex::computeVarIn() {
  lambda = 0;
  bool var_in_found = false;
  for( int i = 0; i < num_cols + num_rows; i++ ) {
    if ( (cost1[i] >= 0) && (cost2[i] < 0) ) {
      // the current variable is eligible to enter the basis
      double candidateLambda = (-cost2[i]/(cost1[i]-cost2[i]));
      var_in_found = true;
      if (candidateLambda > lambda) {
        // the entering variable to select has the greatest lambda value
        lambda = candidateLambda;
        var_in = i;
      }
    }
  }
  return var_in_found;
}


void Biobj_simplex::computeVarOut() {
  // update work_col to represent the column of var_in
  model->getBInvACol(var_in, work_col);

  double current_ratio;
  double min_ratio;
  bool one_out_found = false;

  // update data (a little redundant, isn't it?)
  const double* upper = model->getRowUpper();
  const double* solution = model->getColSolution();
  const double* activity = model->getRowActivity();
  vector<double> b;
  model->getBasics(basics);
  for ( int i = 0; i < num_rows; i++ ) {
    if(basics[i] < num_cols)
      b.push_back(solution[basics[i]]);
    else
      b.push_back(upper[i] - activity[i]);
  }

  for( int i = 0; i < num_rows; i++ ){
    if( work_col[i] > 0){
      // the current variable is eligible to leave the basis
      current_ratio = b[basics[i]]/work_col[i];
      if( !one_out_found ){
        one_out_found = true;
        var_out = i;
        min_ratio = current_ratio;
      } else if( current_ratio < min_ratio ) {
        // the leaving variable to select has the lowest ratio
        var_out = i;
        min_ratio = current_ratio;
      }
    }
  }
}
