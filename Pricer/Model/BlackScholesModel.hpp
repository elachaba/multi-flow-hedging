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

        double getRiskFreeRate() const;

    private:
        PnlMat* volchol_;    // Volatility
        double r_;        // Risk-free rate
        PnlVect* monitoring_dates_; // dates of simulation
        PnlRng* rng_;
    };

}

