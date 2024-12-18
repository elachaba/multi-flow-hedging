#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>


namespace models {

    BlackScholesModel::BlackScholesModel(PnlMat* volchol, double r, PnlVect* observation_dates, PnlRng* rng)
        : volchol_(volchol), r_(r), observation_dates_(observation_dates), rng_(rng) {}

    PnlMat* BlackScholesModel::simulate_path_from_zero(const PnlVect* spots)  const {

        int nb_underlying = spots->size;
        int nb_steps = observation_dates_->size;
        PnlMat* path = pnl_mat_create(nb_steps, nb_underlying);

        PnlVect* gaussian = pnl_vect_create(nb_underlying);
        PnlVect* dW = pnl_vect_create(nb_underlying);
        PnlVect* tmp = pnl_vect_create(nb_underlying);


        for (int j = 0; j < nb_underlying; j++) {
            MLET(path, 0, j) = GET(spots, j);
        }

        for (int i = 1; i < nb_steps; i++) {
            pnl_vect_rng_normal(gaussian, nb_underlying, rng_);   // Generate Z ~ N(0, I)
            pnl_mat_mult_vect_inplace(dW, volchol_, gaussian);    // Introduce correlation : dW = volchol * Z
            double dt = GET(observation_dates_, i) - GET(observation_dates_, i - 1);

            for (int j = 0; j < nb_underlying; j++) {
                double St_prev = MGET(path, i - 1, j);            // Previous price S_{t-dt}
                pnl_mat_get_row(tmp, volchol_, j);
                double sigma_j = pnl_vect_norm_two(tmp);      // volatility of asset j
                double drift = (r_ - 0.5 * sigma_j * sigma_j) * dt;
                double diffusion =  sqrt(dt) * GET(dW, j);

                // update S_t,j
                MLET(path, i, j) = St_prev * exp(drift + diffusion);
            }
        }

        // free tmp memory
        pnl_vect_free(&gaussian);
        pnl_vect_free(&dW);
        pnl_vect_free(&tmp);

        return path;
    }

    PnlMat* BlackScholesModel::simulate_path_from_t(double t, const PnlMat* past) const {

        int nb_underlying = past->n;
        int nb_steps = observation_dates_->size;
        PnlMat* path = pnl_mat_create(nb_steps, nb_underlying);

        PnlVect* gaussian = pnl_vect_create(nb_underlying);
        PnlVect* dW = pnl_vect_create(nb_underlying);
        PnlVect* tmp = pnl_vect_create(nb_underlying);
        
        int i = 0;

        while (i < nb_steps && GET(observation_dates_, i) <= t) {
            for (int j = 0; j < nb_underlying; j++) {
                MLET(path, i, j) = MGET(past, i, j);
            }
            i++;
        }

        for (i; i < nb_steps; i++) {
            pnl_vect_rng_normal(gaussian, nb_underlying, rng_);   // Generate Z ~ N(0, I)
            pnl_mat_mult_vect_inplace(dW, volchol_, gaussian);    // Introduce correlation : dW = volchol * Z
            double dt = GET(observation_dates_, i) - GET(observation_dates_, i - 1);

            for (int j = 0; j < nb_underlying; j++) {
                double St_prev = MGET(path, i - 1, j);            // Previous price S_{t-dt}
                pnl_mat_get_row(tmp, volchol_, j);
                double sigma_j = pnl_vect_norm_two(tmp);      // volatility of asset j
                double drift = (r_ - 0.5 * sigma_j * sigma_j) * dt;
                double diffusion = sqrt(dt) * GET(dW, j);

                // update S_t,j
                MLET(path, i, j) = St_prev * exp(drift + diffusion);
            }
        }

        // free tmp memory
        pnl_vect_free(&gaussian);
        pnl_vect_free(&dW);
        pnl_vect_free(&tmp);

        return path;

    }

    double BlackScholesModel::getRiskFreeRate() const {
        return r_;
    }


}
