#include <cmath>
#include "MonteCarloPricer.hpp"
#include "MonteCarlo.hpp"
#include "pnl/pnl_matrix.h"

using namespace options;
using namespace models;


namespace pricer {
	void MonteCarloPricer::price(const Model& model, const IOption& option, PnlVect* spots, double& price, double& confidence_interval) const
	{
		MonteCarloAtOrigin mco(option, model, sample_number, spots);
		mco.price(price, confidence_interval);
	}
	void MonteCarloPricer::price_at(const double time, const models::Model& model, const options::IOption& option, PnlMat* past, double& price, double& confidence_interval) const
	{
		MonteCarloAtTimeT mct(option, model, sample_number, time, past);
		mct.price(price, confidence_interval);
	}
}
