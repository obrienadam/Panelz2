#ifndef MATRIX_H
#define MATRIX_H

// Eigen thin alias — the solver uses Eigen types directly.
// This header exists to centralise the Eigen include and expose
// convenient type aliases throughout the codebase.

#include <Eigen/Dense>

using DenseMatrix = Eigen::MatrixXd;
using DenseVector = Eigen::VectorXd;

#endif // MATRIX_H
