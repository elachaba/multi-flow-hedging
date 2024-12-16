#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>
#include "pnl/pnl_random.h"

namespace models {

    BlackScholesModel::BlackScholesModel(double sigma, double r)
        : sigma_(sigma), r_(r) {}

    void BlackScholesModel::simulate_path_from_zero(PnlMat* path, double final_time, int nb_time_steps, const PnlVect* spots)  const override {
        // TODO: implement path simulation from zero
    }

    void BlackScholesModel::simulate_path_from_t(PnlMat* path, double t, double final_time, int nb_time_steps, const PnlMat* past) const override {
        // TODO: implement path simulation from time t > 0
    }

    void BlackScholesModel::printParameters() const {
        std::cout << "Black-Scholes Model Parameters:" << std::endl;
        std::cout << "Volatility (sigma): " << sigma_ << std::endl;
        std::cout << "Risk free rate (r): " << r_ << std::endl;
    }

    double BlackScholesModel::getSigma() const {
        return sigma_;
    }

    double BlackScholesModel::getRiskFreeRate() const {
        return r_;
    }

}
