// test_matrix.cpp — Tests for Eigen-backed DenseMatrix / DenseVector

#include <cmath>
#include <gtest/gtest.h>
#include "Matrix.h"

static constexpr double EPS = 1.0e-10;

// ── 2×2 system ────────────────────────────────────────────────────────────────
// [2  1] [x]   [5]       x = 2, y = 1
// [1  3] [y] = [5]

TEST(DenseMatrix, Solve2x2)
{
    DenseMatrix A(2, 2);
    A(0, 0) = 2.0; A(0, 1) = 1.0;
    A(1, 0) = 1.0; A(1, 1) = 3.0;

    DenseVector b(2);
    b(0) = 5.0;
    b(1) = 5.0;

    DenseVector x = A.fullPivLu().solve(b);

    EXPECT_NEAR(x(0), 2.0, EPS);
    EXPECT_NEAR(x(1), 1.0, EPS);
}

// ── 3×3 system ────────────────────────────────────────────────────────────────
// [1 0 0] [x]   [3]       x=3, y=2, z=1
// [0 2 0] [y] = [4]
// [0 0 3] [z]   [3]

TEST(DenseMatrix, Solve3x3Diagonal)
{
    DenseMatrix A = DenseMatrix::Zero(3, 3);
    A(0, 0) = 1.0;
    A(1, 1) = 2.0;
    A(2, 2) = 3.0;

    DenseVector b(3);
    b(0) = 3.0;
    b(1) = 4.0;
    b(2) = 3.0;

    DenseVector x = A.fullPivLu().solve(b);

    EXPECT_NEAR(x(0), 3.0, EPS);
    EXPECT_NEAR(x(1), 2.0, EPS);
    EXPECT_NEAR(x(2), 1.0, EPS);
}

// ── Dense general 3×3 ─────────────────────────────────────────────────────────
// Solution verified against wolfram alpha

TEST(DenseMatrix, Solve3x3General)
{
    DenseMatrix A(3, 3);
    A << 2,  1, -1,
        -3, -1,  2,
        -2,  1,  2;

    DenseVector b(3);
    b << 8, -11, -3;

    DenseVector x = A.fullPivLu().solve(b);

    // Expected: x=2, y=3, z=-1
    EXPECT_NEAR(x(0),  2.0, EPS);
    EXPECT_NEAR(x(1),  3.0, EPS);
    EXPECT_NEAR(x(2), -1.0, EPS);
}

// ── Identity matrix ───────────────────────────────────────────────────────────

TEST(DenseMatrix, SolveIdentity)
{
    DenseMatrix A = DenseMatrix::Identity(4, 4);
    DenseVector b(4);
    b << 1.0, 2.0, 3.0, 4.0;

    DenseVector x = A.fullPivLu().solve(b);

    for (int i = 0; i < 4; ++i)
        EXPECT_NEAR(x(i), b(i), EPS);
}

// ── Residual check: A·x ≈ b ──────────────────────────────────────────────────

TEST(DenseMatrix, ResidualSmall)
{
    // Random-ish 5×5 matrix
    DenseMatrix A(5, 5);
    A << 4, 3, 0, 0, 0,
         3, 4, -1, 0, 0,
         0,-1, 4,  1, 0,
         0, 0, 1,  3, 2,
         0, 0, 0,  2, 4;

    DenseVector b(5);
    b << 24, 30, -24, 12, 16;

    DenseVector x = A.fullPivLu().solve(b);
    DenseVector residual = A * x - b;

    EXPECT_LT(residual.norm(), 1.0e-8);
}

// ── Type aliases ──────────────────────────────────────────────────────────────

TEST(DenseMatrix, TypeAliasesAreEigen)
{
    DenseMatrix M = DenseMatrix::Zero(3, 3);
    DenseVector v = DenseVector::Ones(3);

    EXPECT_EQ(M.rows(), 3);
    EXPECT_EQ(M.cols(), 3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_DOUBLE_EQ(v.sum(), 3.0);
}
