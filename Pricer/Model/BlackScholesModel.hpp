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
        BlackScholesModel(PnlMat* volchol, double r, double maturity, int nbSteps, PnlRng* rng);

        virtual ~BlackScholesModel() override = default;

        PnlMat* simulate_path_from_zero(const PnlVect* spots) const override;

        PnlMat* simulate_path_from_t(double t, const PnlMat* past) const override;

        double getRiskFreeRate() const;
        double getMaturity() const;
        int getNumTimeSteps() const;

    private:
        PnlMat* volchol_;    // Volatilité
        double r_;        // Taux sans risque
        double maturity_;        // Maturité de l'option
        int nbSteps_;           // Nombre de pas de temps
        PnlRng* rng_;
    };

}

