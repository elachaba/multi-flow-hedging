#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include "Model.hpp"

namespace models {

    /**
     * @brief Class representing the Black-Scholes Model
     */
    class BlackScholesModel : public Model {
    public:
        BlackScholesModel(PnlMat* volchol, double r, PnlVect* monitoring_dates, PnlRng* rng);
        virtual ~BlackScholesModel() override = default;

        PnlMat* simulate_path_from_zero(const PnlVect* spots) const override;
        PnlMat* simulate_path_from_t(double t, const PnlMat* past) const override;
        int get_idx_t(double t) const override;
        PnlMat* shift_asset(const PnlMat* const path, double t, int asset, double forward_step_) const override;

    protected:
        PnlMat* volchol_;          // Volatility
        PnlVect* monitoring_dates_; // dates of simulation
        PnlRng* rng_;

    private:
        void fill_path_from_row(PnlMat* path, int row, int nb_steps, int nb_underlying, const PnlMat* past, double t) const;

        // Helper methods to factorize logic
        void generate_correlated_increments(PnlVect* gaussian, PnlVect* dW) const;
        double compute_sigma_j(int j, PnlVect* tmp) const;
        double compute_drift(double sigma_j, double dt) const;
        double compute_diffusion(double dt, double increment) const;
        void advance_step(PnlMat* path, int step, int nb_underlying, double dt, bool usePast, const PnlMat* past,
            PnlVect* gaussian, PnlVect* dW, PnlVect* tmp) const;
    };

}
