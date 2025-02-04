#include <cmath>
#include "MonteCarloPricer.hpp"
#include "MonteCarlo.hpp"
#include "pnl/pnl_matrix.h"


using namespace options;
using namespace models;


namespace pricer {
	void MonteCarloPricer::pricedelta(const Model& model, const IOption& option, PnlVect* spots, double& price, double& confidence_interval, PnlVect* deltas, PnlVect* deltas_std, double eps) const
	{
		MonteCarloAtOrigin mco(option, model, sample_number, spots, eps);
		mco.price(price, confidence_interval);
		mco.delta(deltas, deltas_std);
	}
	void MonteCarloPricer::pricedelta_at(const double time, const models::Model& model, const options::IOption& option, const PnlMat* past, double& price, double& confidence_interval, PnlVect* deltas, PnlVect* deltas_std, double eps) const
	{
		MonteCarloAtTimeT mct(option, model, sample_number, time, past, eps);
		mct.price(price, confidence_interval);
		mct.delta(deltas, deltas_std);
	}
}
