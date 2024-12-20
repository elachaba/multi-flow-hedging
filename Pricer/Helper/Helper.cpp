#include "Helper.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>




namespace utils {

    bool is_close(double a, double b, double epsilon) {
        return std::fabs(a - b) < epsilon;
    }

    bool compare_rows(const PnlMat* mat1, int row1, const PnlMat* mat2, int row2, double epsilon) {
        if (row1 >= mat1->m || row2 >= mat2->m || mat1->n != mat2->n) {
            return false;
        }

        for (int j = 0; j < mat1->n; j++) {
            double val1 = MGET(mat1, row1, j);
            double val2 = MGET(mat2, row2, j);
            if (std::fabs(val1 - val2) >= epsilon) {
                return false; 
            }
        }
        return true;
    }
}


void from_json(const nlohmann::json& j, PnlVect*& vect) {
    std::vector<double> stl_v = j.get<std::vector<double>>();
    vect = pnl_vect_create_from_ptr(stl_v.size(), stl_v.data());
}

void from_json(const nlohmann::json& j, PnlMat*& mat) {
    std::vector<std::vector<double>> stl_m = j.get<std::vector<std::vector<double>>>();
    int nLines = stl_m.size();
    if (nLines == 0) {
        mat = pnl_mat_create(0, 0);
        return;
    }
    int nColumns = stl_m[0].size();
    for (auto it : stl_m) {
        if (nColumns != it.size()) {
            std::cerr << "Matrix is not regular" << std::endl;
            mat = NULL;
            return;
        }
    }
    mat = pnl_mat_create(nLines, nColumns);
    int m = 0;
    for (auto row : stl_m) {
        pnl_mat_set_row_from_ptr(mat, row.data(), m);
        m++;
    }
}
