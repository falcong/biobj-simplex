#include "biobj_simplex.hpp"

using namespace std;

Biobj_simplex::Biobj_simplex() {
  model = new OsiClpSolverInterface;
}


Biobj_simplex::~Biobj_simplex() {
  delete model;
  delete[] cost1;
  delete[] cost2;
  delete[] work_col;
  delete[] basics;
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

  cost1 = new double[num_cols + num_rows];
  cost2 = new double[num_cols + num_rows];
  work_col = new double[num_cols];
  basics = new int[num_rows];

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

  const double* upper = model->getRowUpper();
  const double* solution = model->getColSolution();
  const double* activity = model->getRowActivity();
  updateCosts();

  model->getBasics(basics);

  vector<double> b;
  for ( int i = 0; i < num_rows; i++ ) {
    if(basics[i] < num_cols)
      b.push_back(solution[basics[i]]);
    else
      b.push_back(upper[i] - activity[i]);
  }
  vector<double> s (solution, solution + num_cols);

  BVect sol(/*-1.0 **/ model->getObjValue(), /*-1.0 **/ activity[num_rows], s);
  solutions.push_back(sol);
  //cout << sol._y1 << " " << sol._y2 << endl;

  //compute var_in candidates
  //cout << cost1[0] << " " << cost1[1] << " " << cost1[2] << " " << cost1[3] << endl;
  //cout << cost2[0] << " " << cost2[1] << " " << cost2[2] << " " << cost2[3] << endl;

  while (computeVarIn()){
    computeVarOut();
    //cout << var_in << " doit pivoter avec " << basics[var_out] << endl;

    //perform pivot
    int outstatus = 1;
    model->pivot(var_in, basics[var_out], outstatus);
    //cout << "----------------" << endl;
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
    vector<double> s (solution, solution + num_cols);

    BVect sol2(/*-1.0 **/ model->getObjValue(), /*-1.0 **/ activity[num_rows], s);
    solutions.push_back(sol2);
    //cout << sol2._y1 << " " << sol2._y2 << endl;
    updateCosts();

    //cout << cost1[0] << " " << cost1[1] << " " << cost1[2] << " " << cost1[3] << endl;
    //cout << cost2[0] << " " << cost2[1] << " " << cost2[2] << " " << cost2[3] << endl;
  }
}


vector<BVect> Biobj_simplex::getSols() {
  return solutions;
}


bool Biobj_simplex::computeVarIn() {
  lambda = 0;
  bool var_in_found = false;
  for( int i = 0; i < num_cols; i++ ) {
    if ( (cost1[i] > 0) && (cost2[i] < 0) ) {
      double candidateLambda = (-cost2[i]/(cost1[i]-cost2[i]));
      //cout << i << " : " << candidateLambda << endl;
      var_in_found = true;
      if (candidateLambda > lambda) {
        lambda = candidateLambda;
        var_in = i;
      }
    }
  }
  return var_in_found;
}


void Biobj_simplex::computeVarOut() {
  model->getBInvACol(var_in, work_col);
  double current_ratio;
  double min_ratio;
  bool one_out_found = false;
  const double* upper = model->getRowUpper();
  const double* solution = model->getColSolution();
  const double* activity = model->getRowActivity();
  vector<double> b;
  for ( int i = 0; i < num_rows; i++ ) {
    if(basics[i] < num_cols)
      b.push_back(solution[basics[i]]);
    else
      b.push_back(upper[i] - activity[i]);
  }

    for( int i = 0; i < num_rows; i++ ){
      if( work_col[i] > 0){
        current_ratio = b[basics[i]]/work_col[i];
        if( !one_out_found ){
          one_out_found = true;
          var_out = i;
          min_ratio = current_ratio;
        } else if( current_ratio < min_ratio ) {
          var_out = i;
          min_ratio = current_ratio;
        }
      }
    }
}
