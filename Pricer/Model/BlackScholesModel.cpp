#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>
#include "pnl/pnl_random.h"

namespace models {

    BlackScholesModel::BlackScholesModel(double sigma, double r)
        : sigma_(sigma), r_(r) {}

    PnlMat* BlackScholesModel::simulate_path_from_zero(const PnlVect* spots)  const {
        // TODO: implement path simulation from zero
    }

    PnlMat* BlackScholesModel::simulate_path_from_t(double t, const PnlMat* past) const {
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
