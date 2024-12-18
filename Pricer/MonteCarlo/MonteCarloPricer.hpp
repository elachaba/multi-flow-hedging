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
		void price(const models::Model& model, const options::IOption& option, PnlVect* spots, double& price, double& confidence_interval) const;
		void price_at(const double time, const models::Model& underlying_model, const options::IOption& option, PnlMat* past, double& price, double& confidence_interval) const;
	};
}
