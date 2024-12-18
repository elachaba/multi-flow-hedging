#include "BlackScholesModelTestHelper.hpp"
#include <cmath>


namespace models {
    // Testing functions
    PnlMat* BlackScholesModelTestHelper::simulate_path_from_zero_test(const PnlVect* spots, PnlMat* precomputed_guassians) {
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
            pnl_mat_get_row(gaussian, precomputed_guassians, i - 1);
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

    PnlMat* BlackScholesModelTestHelper::simulate_path_alternative(const PnlVect* spots, const PnlVect* sigma,
        const PnlMat* corr, const PnlMat* precomputed_gaussians) const {
        int nb_underlying = spots->size;
        int nb_steps = observation_dates_->size; // Include the initial step t=0
        PnlMat* path = pnl_mat_create(nb_steps, nb_underlying);
        // Cholesky Decomposition for simulation
        PnlMat* chol = pnl_mat_copy(corr);
        pnl_mat_chol(chol);


        PnlVect* correlated_gaussians = pnl_vect_create(nb_underlying);
        PnlVect* gaussians = pnl_vect_create(nb_underlying);

        // Set initial spot prices at t=0
        for (int j = 0; j < nb_underlying; j++) {
            MLET(path, 0, j) = GET(spots, j);
        }

        // Time stepping
        for (int i = 1; i < nb_steps; i++) {
            double dt = GET(observation_dates_, i) - GET(observation_dates_, i - 1);
            pnl_mat_get_row(gaussians, precomputed_gaussians, i - 1);
            pnl_mat_mult_vect_inplace(correlated_gaussians, chol, gaussians);

            // Update asset prices
            for (int j = 0; j < nb_underlying; j++) {
                double St_prev = MGET(path, i - 1, j);
                double sigma_j = GET(sigma, j);
                double dW = GET(correlated_gaussians, j);
                double drift = (r_ - 0.5 * sigma_j * sigma_j) * dt;
                double diffusion = sigma_j * sqrt(dt) * dW;

                MLET(path, i, j) = St_prev * exp(drift + diffusion);
            }
        }

        pnl_vect_free(&gaussians);
        pnl_vect_free(&correlated_gaussians);
        return path;
    }
}