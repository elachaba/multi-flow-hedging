#include "../Model/BlackScholesModel.hpp"
#include <gtest/gtest.h>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#include <cmath>

namespace models {
	class BlackScholesModelTest : public ::testing::Test {
	protected:
		// Test variables
		double r;
		int nb_underlying;
		PnlMat* volchol;
		PnlVect* spots;
		PnlVect* monitoring_dates;
		PnlRng* rng;
		BlackScholesModel* model;

		void SetUp() override {
			// initialize parameters
			r = 0.05;
			nb_underlying = 5;

			// Monitoring dates
			monitoring_dates = pnl_vect_create_from_list(4, 0.17063492063492064, 0.25793650793650796, 0.3333333333333333, 0.4246031746031746);

			// Volatility-Cholesky matrix (provided directly)
			volchol = pnl_mat_create_from_list(nb_underlying, nb_underlying,
				0.3, 0, 0, 0, 0,
				0.055, 0.24387497, 0, 0, 0,
				-0.0375, -0.02998463, 0.24534603, 0, 0,
				-0.015, -0.01199385, 0.01865883, 0.09634830, 0,
				-0.0675, -0.05397233, 0.08396472, 0.06926412, 0.427999);

			// Initial spot prices for 5 assets: [13, 15, 17, 17, 14]
			spots = pnl_vect_create_from_list(nb_underlying, 13.0, 15.0, 17.0, 17.0, 14.0);

			// random number generator
			rng = pnl_rng_create(PNL_RNG_MERSENNE);
			pnl_rng_sseed(rng, 24);

			// Set up the model
			model = new BlackScholesModel(volchol, r, monitoring_dates, rng);
		}

		void TearDown() override {
			pnl_mat_free(&volchol);
			pnl_vect_free(&spots);
			pnl_vect_free(&monitoring_dates);
			pnl_rng_free(&rng);
			
		}
	};

	TEST_F(BlackScholesModelTest, SimulatedPricesMatchBlackScholesMean) {
		int num_paths = 10000; // Number of Monte Carlo paths
		int nb_steps = monitoring_dates->size + 1; // Total time steps
		double tolerance = 0.01; // Allowable relative error (1%)

		// Initialize array to store the sum of prices for each asset at each time step
		PnlMat* mean_prices = pnl_mat_create(nb_steps, nb_underlying);
		pnl_mat_set_zero(mean_prices);

		// Monte Carlo simulation: Generate paths and sum prices
		for (int p = 0; p < num_paths; p++) {
			PnlMat* path = model->simulate_path_from_zero(spots);

			for (int i = 0; i < nb_steps; i++) {
				for (int j = 0; j < nb_underlying; j++) {
					double St = MGET(path, i, j);
					MLET(mean_prices, i, j) += St / num_paths; // Accumulate mean
				}
			}
			pnl_mat_free(&path);
		}

		// Compare the mean prices to theoretical Black-Scholes expectations
		for (int j = 0; j < nb_underlying; j++) {
			double S0 = GET(spots, j);

			for (int step = 1; step < nb_steps; step++) {
				double t = GET(monitoring_dates, step - 1);
				double expected_mean = S0 * exp(r * t);
				double simulated_mean = MGET(mean_prices, step, j);

				EXPECT_NEAR(simulated_mean, expected_mean, expected_mean * tolerance)
					<< "Mismatch at time step " << step << ", asset " << j;
			}
		}

		pnl_mat_free(&mean_prices);
	}
}
