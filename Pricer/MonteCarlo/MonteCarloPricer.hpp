#pragma once
#include "pnl/pnl_vector.h"
#include "../Options/IOption.hpp"
#include "../Model/Model.hpp"

namespace pricer {
	class MonteCarloPricer
	{
	private:
		const unsigned long sample_number;

	public:
		MonteCarloPricer(const unsigned long sample_nb) : sample_number(sample_nb) {};
		void pricedelta(const models::Model& model, const options::IOption& option, PnlVect* spots, double& price, double& confidence_interval, PnlVect* deltas, PnlVect* deltas_std, double eps) const;
		void pricedelta_at(const double time, const models::Model& underlying_model, const options::IOption& option, const PnlMat* past, double& price, double& confidence_interval, PnlVect* deltas, PnlVect* deltas_std, double eps) const;
	};
}
