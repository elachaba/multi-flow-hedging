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
        virtual void simulate_path_from_zero(PnlMat* path, double final_time, int nb_time_steps, const PnlVect* spots) const = 0;

        /**
         * @brief Simulates the paths of the underlying assets prices under risk-neutral probability.
         * @param path Matrix to stock the paths. Every column corresponds to an asset.
         * @param t Starting time of the simulation.
         * @param final_time Time horizon of the simulation.
         * @param nb_time_steps Number of time steps.
         * @param past Matrix of historic past values of asset prices.
         */
        virtual void simulate_path_from_t(PnlMat* path, double t, double final_time, int nb_time_steps, const PnlMat* past) const = 0;

        /**
         * @brief Prints the model parameters.
         */
        virtual void printParameters() const = 0;
    };

}

