#pragma once

#include "../Model/BlackScholesModel.hpp"

namespace models {

    class BlackScholesModelTestHelper : public BlackScholesModel {
    public:
        // Inherit the constructors from BlackScholesModel
        using BlackScholesModel::BlackScholesModel;

        // Test-specific methods
        PnlMat* simulate_path_from_zero_test(const PnlVect* spots, PnlMat* precomputed_gaussians);

        PnlMat* simulate_path_alternative(const PnlVect* spots, const PnlVect* sigma,
            const PnlMat* corr, const PnlMat* precomputed_gaussians) const;
    };

}