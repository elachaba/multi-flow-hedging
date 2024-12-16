#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Model.hpp"

namespace model {

    /**
     * @brief Class representing the Black-Scholes Model
     */
    class BlackScholesModel : public Model {
    public:
        BlackScholesModel(double sigma, double r);

        virtual ~BlackScholesModel() override = default;

        void simulate_path_from_zero(PnlMat* path, double final_time, int nb_time_steps, const PnlVect* spots) const override;

        void simulate_path_from_t(PnlMat* path, double t, double final_time, int nb_time_steps, const PnlMat* past) const override;

        void printParameters() const override;

   
        double getSigma() const;


        double getRiskFreeRate() const;

    private:
        double sigma_;
        double r_;
    };

}

