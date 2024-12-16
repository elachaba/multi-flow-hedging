#include "pnl/pnl_matrix.h"
#include "IOption.hpp"

namespace pricer {
	class MonteCarlo {
	protected:
		const options::IOption& option;
		const unsigned long sample_number;
		const double t; // pricing time
		MonteCarlo(const options::IOption& _option, const unsigned long _samples, const double _t) :
			option(_option), sample_number(_samples), t(_t) {}

		void price(double& price, double& confidence_interval)
	};

	class MonteCarloAtOrigin : public MonteCarlo {
	private:
		const PnlMat* spots;
	public:
		MonteCarloAtOrigin(const option::IOption& _option, const unsigned long _samples, const double _t, PnlMat* _spots) :
			MonteCarlo(_option, _samples, _t), spots(_spots) {}
	};

	class MonteCarloAtTimeT : public MonteCarlo {
	private:
		const PnlMat* past;
	
	public:
		MonteCarloAtTimeT(const option::IOption& _option, const unsigned long _samples, const double _t, PnlMat* _past) :
			MonteCarlo(_option, _samples, _t), past(_past) {}
	};
}