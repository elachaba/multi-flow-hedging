#include <cmath>
#include "MonteCarlo.hpp"
#include <iostream>

void pricer::MonteCarlo::price(double& price, double& confidence_interval) {
	double sum = 0.0;
	double squared_sum = 0.0;
	double r = model.getRiskFreeRate();
	PnlVect* payoffs;
	for (int i = 0; i < samples_number; i++) {
		const PnlMat* const path = get_path();
		double payoff = option.discounted_payoff(path, r, t);
		sum += payoff;
		squared_sum += payoff * payoff;
	}
	price = sum / samples_number;
	double variance = squared_sum / samples_number - price * price;
	confidence_interval = 1.96 * sqrt(variance / samples_number);


}


void pricer::MonteCarlo::delta(PnlVect* deltas, PnlVect* deltas_std) {
	double diff;
	int nb_underlying = model.getModelSize();
	double r = model.getRiskFreeRate();
	PnlVect* squared_sums = pnl_vect_create(nb_underlying);

	for (int m = 0; m < samples_number; m++) {
		const PnlMat* const path = get_path();
		for (int d = 0; d < nb_underlying; d++) {
			PnlMat* shifted_up = model.shift_asset(path, t, d, eps);
			PnlMat* shifted_down = model.shift_asset(path, t, d, -eps);
			diff = option.discounted_payoff(shifted_up, r, t) - option.discounted_payoff(shifted_down, r, t);
			LET(deltas, d) = GET(deltas, d) + diff;
			LET(squared_sums, d) = GET(squared_sums, d) + diff * diff;
			pnl_mat_free(&shifted_down);
			pnl_mat_free(&shifted_up);
		}
	}



	const PnlMat* _past = get_past();

	for (int d = 0; d < nb_underlying; d++) {
		double s_t = MGET(_past, _past->m - 1, d); // St is last row in the past
		double mean = GET(deltas, d) / (2.0 * samples_number * eps);
		double variance = GET(squared_sums, d) / (4.0 * eps * eps * samples_number) - mean * mean;
		LET(deltas, d) = mean / s_t;
		LET(deltas_std, d) = sqrt(variance) / (s_t * sqrt(samples_number));
	}


	pnl_vect_free(&squared_sums);
}