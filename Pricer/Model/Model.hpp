#pragma once

#include "pnl/pnl_matrix.h"
#include "pnl/pnl_vector.h"

namespace models {

    class Model {
    public:
        virtual ~Model() = default;

        /**
         * @brief Simulates the paths of the underlying assets prices under risk-neutral probability.
         * @param path Matrix to stock the paths. Every column corresponds to an asset.
         * @param final_time Time horizon of the simulation.
         * @param nb_time_steps Number of time steps.
         * @param spots Spots vector.
         */
        virtual PnlMat* simulate_path_from_zero(const PnlVect* spots) const = 0;

        /**
         * @brief Simulates the paths of the underlying assets prices under risk-neutral probability.
         * @param path Matrix to stock the paths. Every column corresponds to an asset.
         * @param t Starting time of the simulation.
         * @param final_time Time horizon of the simulation.
         * @param nb_time_steps Number of time steps.
         * @param past Matrix of historic past values of asset prices.
         */
        virtual PnlMat* simulate_path_from_t(double t, const PnlMat* past) const = 0;

        virtual PnlMat* shift_asset(const PnlMat* const path, double t, int asset, double forward_step_) const = 0;

        virtual int get_idx_t(double t) const = 0;

        double getRiskFreeRate() const { return r_;  }
        double getModelSize() const { return model_size; }

    
    protected:
        int model_size;
        double r_; // risk-free rate
    };

}

