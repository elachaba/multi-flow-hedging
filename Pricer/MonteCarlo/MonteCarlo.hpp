#include "pnl/pnl_matrix.h"
#include "../Options/IOption.hpp"
#include "../Model/Model.hpp"

namespace pricer {
	class MonteCarlo {
	protected:
		const options::IOption& option;
		const models::Model& model;
		const unsigned long samples_number;
		const double eps;
		const double t; // pricing time
		MonteCarlo(const options::IOption& _option, const models::Model& _model,  const unsigned long _samples, const double _t, const double _eps) :
			option(_option), model(_model), samples_number(_samples), t(_t), eps(_eps){}

		virtual const PnlMat* const get_path() const = 0;

	public:
		void price(double& price, double& confidence_interval);
	};


	// Monte Carlo Simulation From 0
	class MonteCarloAtOrigin : public MonteCarlo {
	private:
		const PnlVect* spots;
	
	protected:
		const PnlMat* const get_path() const {
			return model.simulate_path_from_zero(spots);
		}
	
	public:
		MonteCarloAtOrigin(const options::IOption& _option, const models::Model& _model, const unsigned long _samples, PnlVect* _spots) :
			MonteCarlo(_option, _model, _samples, 0.0, 0.0), spots(_spots) {}
		MonteCarloAtOrigin(const options::IOption& _option, const models::Model& _model, const unsigned long _samples, PnlVect* _spots, const double _eps) :
			MonteCarlo(_option, _model, _samples, 0.0, _eps), spots(_spots) {}
	};

	
	// Monte Carlo Simulation From t
	class MonteCarloAtTimeT : public MonteCarlo {
	private:
		const PnlMat* past;
	
	protected:
		const PnlMat* const get_path() const {
			return model.simulate_path_from_t(t, past);
		}
	
	public:
		MonteCarloAtTimeT(const options::IOption& _option, const models::Model& _model, const unsigned long _samples, const double _t, PnlMat* _past) :
			MonteCarlo(_option, _model, _samples, _t, 0.0), past(_past) {}
		MonteCarloAtTimeT(const options::IOption& _option, const models::Model& _model, const unsigned long _samples, const double _t, PnlMat* _past, const double _eps) :
			MonteCarlo(_option, _model, _samples, _t, _eps), past(_past) {}

		
		void delta(PnlVect* deltas, PnlVect* deltas_std) {
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

			for (int d = 0; d < nb_underlying; d++) {
				double s_t = MGET(past, past->m - 1, d); // St is last row in the past
				double mean = GET(deltas, d) / (2.0 * samples_number * eps);
				double variance = GET(squared_sums, d) / (4.0 * eps * eps * samples_number) - mean * mean;
				LET(deltas, d) = mean / s_t;
				LET(deltas_std, d) = sqrt(variance) / (s_t * sqrt(samples_number));
			}

			pnl_vect_free(&squared_sums);
		}
		
	};
}