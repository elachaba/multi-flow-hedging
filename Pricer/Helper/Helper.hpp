#pragma once

#include <pnl/pnl_matrix.h>
#include "pnl/pnl_vector.h"
#include <cmath>
#include <nlohmann/json.hpp>


namespace utils {

    /**
     * @brief Compares two floating-point numbers for approximate equality within a given tolerance.
     *
     * @param a The first floating-point number.
     * @param b The second floating-point number.
     * @param epsilon The tolerance value for the comparison (default is 1e-10).
     * @return true if the numbers are approximately equal, false otherwise.
     */
    bool is_close(double a, double b, double epsilon = 1e-10);


    /**
     * @brief Compares two rows from two matrices for approximate equality within a given tolerance.
     *
     * @param mat1 The first matrix.
     * @param row1 The row index in the first matrix.
     * @param mat2 The second matrix.
     * @param row2 The row index in the second matrix.
     * @param epsilon The tolerance value for the comparison (default is 1e-10).
     * @return true if the rows are approximately equal, false otherwise.
     */
    bool compare_rows(const PnlMat* mat1, int row1, const PnlMat* mat2, int row2, double epsilon = 1e-10);

}




void from_json(const nlohmann::json& j, PnlVect*& vect);
void from_json(const nlohmann::json& j, PnlMat*& mat);
