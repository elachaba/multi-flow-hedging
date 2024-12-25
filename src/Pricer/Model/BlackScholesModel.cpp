#include "BlackScholesModel.hpp"
#include <cmath>
#include <iostream>
#include "../Helper/Helper.hpp"

namespace models {

    BlackScholesModel::BlackScholesModel(PnlMat* volchol, double r, PnlVect* monitoring_dates, PnlRng* rng)
        : volchol_(volchol), monitoring_dates_(monitoring_dates), rng_(rng) {
        this->r_ = r;
        this->model_size = volchol->m;
    }

    PnlMat* BlackScholesModel::simulate_path_from_t(double t, const PnlMat* past) const {
        int nb_underlying = past->n;
        int nb_steps = monitoring_dates_->size;
        PnlMat* path = pnl_mat_create_from_zero(nb_steps, nb_underlying);

        // Copy all known values from past into path
        int i = 0;
        while (i < nb_steps && (GET(monitoring_dates_, i) < t || utils::is_close(GET(monitoring_dates_, i), t))) {
            for (int j = 0; j < nb_underlying; j++) {
                MLET(path, i, j) = MGET(past, i, j);
            }
            i++;
        }

        // Fill the remaining of the path
        fill_path_from_row(path, i, nb_steps, nb_underlying, past, t);

        return path;
    }

    PnlMat* BlackScholesModel::simulate_path_from_zero(const PnlVect* spots) const {
        PnlMat* past = pnl_mat_create(1, spots->size);
        pnl_mat_set_row(past, spots, 0);
        return simulate_path_from_t(0.0, past);
    }

    void BlackScholesModel::fill_path_from_row(PnlMat* path, int row, int nb_steps, int nb_underlying, const PnlMat* past, double t) const {
        PnlVect* gaussian = pnl_vect_create(nb_underlying);
        PnlVect* dW = pnl_vect_create(nb_underlying);
        PnlVect* tmp = pnl_vect_create(nb_underlying);

        bool needIntermediateStep = !utils::compare_rows(path, row - 1, past, past->m - 1);

        // If we need to generate a step from the last known point in 'past'
        if (needIntermediateStep && row < nb_steps) {
            double dt = GET(monitoring_dates_, row) - t;
            advance_step(path, row, nb_underlying, dt, true, past, gaussian, dW, tmp);
        }

        // Advance all remaining steps
        for (int i = row + needIntermediateStep; i < nb_steps; i++) {
            double dt = GET(monitoring_dates_, i) - GET(monitoring_dates_, i - 1);
            advance_step(path, i, nb_underlying, dt, false, past, gaussian, dW, tmp);
        }

        pnl_vect_free(&gaussian);
        pnl_vect_free(&dW);
        pnl_vect_free(&tmp);
    }

    PnlMat* BlackScholesModel::shift_asset(const PnlMat* const path, double t, int asset, double forward_step_) const {
        PnlMat* shifted = pnl_mat_copy(path);
        int idx = get_idx_t(t);
        for (int i = idx; i < shifted->m; i++)
            MLET(shifted, i, asset) = MGET(path, i, asset) * (1 + forward_step_);
        return shifted;
    }

    int BlackScholesModel::get_idx_t(double t) const {
        int nb_time_steps = monitoring_dates_->size;
        int idx = 0;
        while (idx < nb_time_steps - 1 && GET(monitoring_dates_, idx) < t) {
            idx++;
        }
        return idx;
    }

    // ---------------------- Helper Methods ------------------------

    void BlackScholesModel::generate_correlated_increments(PnlVect* gaussian, PnlVect* dW) const {
        pnl_vect_rng_normal(gaussian, gaussian->size, rng_);   // Gaussian ~ N(0, I)
        pnl_mat_mult_vect_inplace(dW, volchol_, gaussian);     // dW = volchol * Gaussian (introduce correlation)
    }

    double BlackScholesModel::compute_sigma_j(int j, PnlVect* tmp) const {
        pnl_mat_get_row(tmp, volchol_, j);
        return pnl_vect_norm_two(tmp); // volatility of asset j
    }

    double BlackScholesModel::compute_drift(double sigma_j, double dt) const {
        return (r_ - 0.5 * sigma_j * sigma_j) * dt;
    }

    double BlackScholesModel::compute_diffusion(double dt, double increment) const {
        return sqrt(dt) * increment;
    }

    void BlackScholesModel::advance_step(PnlMat* path, int step, int nb_underlying, double dt, bool usePast,
        const PnlMat* past, PnlVect* gaussian, PnlVect* dW, PnlVect* tmp) const {
        generate_correlated_increments(gaussian, dW);

        for (int j = 0; j < nb_underlying; j++) {
            double St_prev = usePast ? MGET(past, past->m - 1, j) : MGET(path, step - 1, j);

            double sigma_j = compute_sigma_j(j, tmp);
            double drift = compute_drift(sigma_j, dt);
            double diffusion = compute_diffusion(dt, GET(dW, j));

            MLET(path, step, j) = St_prev * exp(drift + diffusion);
        }
    }

}
