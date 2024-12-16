#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>
#include "pnl/pnl_random.h"

namespace model {

    BlackScholesModel::BlackScholesModel(double sigma, double r)
        : sigma_(sigma), r_(r) {}

    void BlackScholesModel::simulate(PnlMat* path, double T, int N, double S0) const {
        // Initialisation du premier élément de la matrice avec S0
        pnl_mat_set_double(path, 0, 0, S0);

        double dt = T / N;
        double drift = (r_ - 0.5 * sigma_ * sigma_) * dt;
        double diffusion = sigma_ * std::sqrt(dt);

        // Initialisation de la séquence aléatoire
        PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
        pnl_rng_sseed(rng, time(NULL));

        for (int i = 1; i < N + 1; ++i) {
            double Z = pnl_rng_normal(rng);
            double S_prev = pnl_mat_get_double(path, i - 1, 0);
            double S = S_prev * std::exp(drift + diffusion * Z);
            pnl_mat_set_double(path, i, 0, S);
        }

        pnl_rng_free(&rng);
    }

    void BlackScholesModel::printParameters() const {
        std::cout << "Modèle de Black-Scholes Parameters:" << std::endl;
        std::cout << "Volatilité (sigma): " << sigma_ << std::endl;
        std::cout << "Taux sans risque (r): " << r_ << std::endl;
    }

    double BlackScholesModel::getSigma() const {
        return sigma_;
    }

    double BlackScholesModel::getRiskFreeRate() const {
        return r_;
    }

}
