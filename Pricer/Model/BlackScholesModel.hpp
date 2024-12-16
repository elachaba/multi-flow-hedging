#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "Model.hpp"

namespace models {

    /**
     * @brief Class representing the Black-Scholes Model
     */
    class BlackScholesModel : public Model {
    public:
        BlackScholesModel(double sigma, double r);

        virtual ~BlackScholesModel() override = default;

        PnlMat* simulate_path_from_zero(const PnlVect* spots) const override;

        PnlMat* simulate_path_from_t(double t, const PnlMat* past) const override;

        void printParameters() const override;

   
        double getSigma() const;


        double getRiskFreeRate() const;

    private:
        double sigma_;
        double r_;
    };

}

