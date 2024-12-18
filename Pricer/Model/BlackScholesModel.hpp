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
        BlackScholesModel(PnlMat* volchol, double r, PnlVect* observation_dates, PnlRng* rng);

        virtual ~BlackScholesModel() override = default;

        PnlMat* simulate_path_from_zero(const PnlVect* spots) const override;

        PnlMat* simulate_path_from_t(double t, const PnlMat* past) const override;

    protected:
        PnlMat* volchol_;    // Volatility
        PnlVect* observation_dates_; // dates of simulation
        PnlRng* rng_;
    
    private:
        void fill_path_from_row(PnlMat* path, int row, int nb_steps, int nb_underlying) const;
    };

}

