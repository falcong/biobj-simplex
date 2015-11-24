#include <cppunit/extensions/HelperMacros.h>
#include "biobj_simplex.hpp"

class Biobj_simplex_test : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(Biobj_simplex_test);
  CPPUNIT_TEST(testSimpleSolution);
  CPPUNIT_TEST_SUITE_END();
 private:
  Biobj_simplex* solver;
 public:
  void setUp() {
    solver = new Biobj_simplex;
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
    col_ub[0] = solver->getInfinity();
    col_ub[1] = solver->getInfinity();
     
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
    row_lb[0] = -1.0 * solver->getInfinity();
    row_ub[0] = 3.0;
    matrix->appendRow(row1);

    //3 x0 - 1 x1 <= 6  =>  -infinity <= 3 x0 - 1 x1 <= 6
    CoinPackedVector row2;
    row2.insert(0, 3.0);
    row2.insert(1, -1.0);
    row_lb[1] = -1.0 * solver->getInfinity();
    row_ub[1] = 6.0;
    matrix->appendRow(row2);

    solver->loadProblem(*matrix, col_lb, col_ub, objective1, objective2, row_lb, row_ub);
  }
  void tearDown() {
    delete solver;
  }
  void testSimpleSolution() {
    solver->solve();
    std::vector<BVect> solutions = solver->getSols();

    CPPUNIT_ASSERT(solutions.size() == 3);
    CPPUNIT_ASSERT(solutions[0].z1() ==  0);
    CPPUNIT_ASSERT(solutions[0].z2() ==  0);
    CPPUNIT_ASSERT(solutions[1].z1() ==  3);
    CPPUNIT_ASSERT(solutions[1].z2() == -6);
    CPPUNIT_ASSERT(solutions[2].z1() == 12);
    CPPUNIT_ASSERT(solutions[2].z2() == -9);
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION(Biobj_simplex_test);
