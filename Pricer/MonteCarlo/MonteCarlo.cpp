#include <cmath>
#include "MonteCarlo.hpp"

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