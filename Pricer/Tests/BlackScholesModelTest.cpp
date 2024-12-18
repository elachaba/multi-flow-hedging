#include "BlackScholesModelTestHelper.hpp"
#include <gtest/gtest.h>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

namespace models {
    class BlackScholesModelTest : public ::testing::Test {
    protected:
        BlackScholesModelTestHelper* model;
        double r;
        int nb_underlying;
        PnlVect* spots;
        PnlVect* sigma;
        PnlMat* corr;
        PnlMat* volchol;
        PnlMat* precomputed_gaussians;
        PnlVect* monitoring_dates;

        void SetUp() override {
            r = 0.05; // Risk-free rate
            nb_underlying = 5;

            // Spot prices for 5 assets
            spots = pnl_vect_create_from_list(nb_underlying, 13.0, 15.0, 17.0, 17.0, 14.0);

            // Volatility vector
            sigma = pnl_vect_create_from_list(nb_underlying, 0.3, 0.25, 0.25, 0.1, 0.45);

            // Correlation matrix (5x5, symmetric)
            corr = pnl_mat_create_from_list(nb_underlying, nb_underlying,
                1.0, 0.22, -0.15, -0.15, -0.15,
                0.22, 1.0, -0.15, -0.15, -0.15,
                -0.15, -0.15, 1.0, 0.22, 0.22,
                -0.15, -0.15, 0.22, 1.0, 0.22,
                -0.15, -0.15, 0.22, 0.22, 1.0);

            // Volatility-Cholesky matrix (provided directly)
            volchol = pnl_mat_create_from_list(nb_underlying, nb_underlying,
                0.3, 0, 0, 0, 0,
                0.055, 0.24387497, 0, 0, 0,
                -0.0375, -0.02998463, 0.24534603, 0, 0,
                -0.015, -0.01199385, 0.01865883, 0.09634830, 0,
                -0.0675, -0.05397233, 0.08396472, 0.06926412, 0.427999);

            // Monitoring dates
            monitoring_dates = pnl_vect_create_from_list(5, 0.0, 0.17063492063492064, 0.25793650793650796,
                0.3333333333333333, 0.4246031746031746);

            // Precomputed Gaussian matrix (4 steps x 5 assets)
            precomputed_gaussians = pnl_mat_create_from_list(4, nb_underlying,
                0.1, -0.1, 0.2, -0.2, 0.3,
                0.05, -0.05, 0.15, -0.15, 0.25,
                0.2, -0.2, 0.3, -0.3, 0.4,
                0.1, 0.0, -0.1, 0.05, -0.05);

            // Initialize the Black-Scholes model
            model = new BlackScholesModelTestHelper(volchol, r, monitoring_dates, nullptr);
        }

        void TearDown() override {
            pnl_vect_free(&spots);
            pnl_vect_free(&sigma);
            pnl_mat_free(&corr);
            pnl_mat_free(&volchol);
            pnl_mat_free(&precomputed_gaussians);
            pnl_vect_free(&monitoring_dates);
        }
    };

    TEST_F(BlackScholesModelTest, CompareSimulatedPaths) {
        // Simulate paths using both methods
        PnlMat* path_model = model->simulate_path_from_zero_test(spots, precomputed_gaussians);
        PnlMat* path_alternative = model->simulate_path_alternative(spots, sigma, corr, precomputed_gaussians);

        // Compare paths at each step and for each asset
        for (int i = 0; i < path_model->m; i++) {
            for (int j = 0; j < path_model->n; j++) {
                double value_model = MGET(path_model, i, j);
                double value_alternative = MGET(path_alternative, i, j);

                EXPECT_NEAR(value_model, value_alternative, 1e-6)
                    << "Mismatch at step " << i << ", asset " << j;
            }
        }

        // Free allocated paths
        pnl_mat_free(&path_model);
        pnl_mat_free(&path_alternative);
    }
}
