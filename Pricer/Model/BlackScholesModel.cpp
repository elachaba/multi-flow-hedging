#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>


namespace models {

    BlackScholesModel::BlackScholesModel(PnlMat* volchol, double r, double maturity, int nbSteps, PnlRng* rng)
        : volchol_(volchol), r_(r), maturity_(maturity), nbSteps_(nbSteps), rng_(rng) {}

    PnlMat* BlackScholesModel::simulate_path_from_zero(const PnlVect* spots)  const {

        int nb_underlying = spots->size;
        PnlMat* path = pnl_mat_create(nbSteps_, nb_underlying);
        double dt = maturity_ / nbSteps_;

        PnlVect* gaussian = pnl_vect_create(nb_underlying);
        PnlVect* dW = pnl_vect_create(nb_underlying);
        PnlVect* tmp = pnl_vect_create(nb_underlying);


        for (int j = 0; j < nb_underlying; j++) {
            MLET(path, 0, j) = GET(spots, j);
        }

        for (int i = 1; i <= nbSteps_; i++) {
            pnl_vect_rng_normal(gaussian, nb_underlying, rng_);   // G�n�rer Z ~ N(0, I)
            pnl_mat_mult_vect_inplace(dW, volchol_, gaussian);    // Introduire la corr�lation : dW = volchol * Z
            

            for (int j = 0; j < nb_underlying; j++) {
                double St_prev = MGET(path, i - 1, j);            // Prix pr�c�dent S_{t-dt}
                pnl_mat_get_row(tmp, volchol_, j);
                double sigma_j = pnl_vect_norm_two(tmp);      // Approximation de la volatilit� pour l'actif j
                double drift = (r_ - 0.5 * sigma_j * sigma_j) * dt;
                double diffusion =  sqrt(dt) * GET(dW, j);

                // Mise � jour de S_t,j avec la formule de Black-Scholes
                MLET(path, i, j) = St_prev * exp(drift + diffusion);
            }
        }

        // Lib�ration de la m�moire temporaire
        pnl_vect_free(&gaussian);
        pnl_vect_free(&dW);
        pnl_vect_free(&tmp);

        return path;
    }

    PnlMat* BlackScholesModel::simulate_path_from_t(double t, const PnlMat* past) const {
        // TODO: implement path simulation from time t > 0
    }

    double BlackScholesModel::getRiskFreeRate() const {
        return r_;
    }

    double BlackScholesModel::getMaturity() const {
        return maturity_;
    }

    int BlackScholesModel::getNumTimeSteps() const {
        return nbSteps_;
    }

}
