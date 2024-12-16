#include "pnl/pnl_matrix.h"
#include "../Options/IOption.hpp"
#include "../Model/Model.hpp"

namespace pricer {
	class MonteCarlo {
	protected:
		const options::IOption& option;
		const models::Model& model;
		const unsigned long samples_number;
		const double t; // pricing time
		MonteCarlo(const options::IOption& _option, const models::Model& _model,  const unsigned long _samples, const double _t) :
			option(_option), model(_model), samples_number(_samples), t(_t) {}

		void price(double& price, double& confidence_interval);
	};

	class MonteCarloAtOrigin : public MonteCarlo {
	private:
		const PnlMat* spots;
	public:
		MonteCarloAtOrigin(const options::IOption& _option, const models::Model& _model, const unsigned long _samples, const double _t, PnlMat* _spots) :
			MonteCarlo(_option, _model, _samples, _t), spots(_spots) {}
	};

	class MonteCarloAtTimeT : public MonteCarlo {
	private:
		const PnlMat* past;
	
	public:
		MonteCarloAtTimeT(const options::IOption& _option, const models::Model& _model, const unsigned long _samples, const double _t, PnlMat* _past) :
			MonteCarlo(_option, _model, _samples, _t), past(_past) {}
	};
}