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

		virtual PnlMat* get_path() const = 0;
		virtual const PnlMat* get_past() const = 0;

	public:
		void price(double& price, double& confidence_interval);
		void delta(PnlVect* deltas, PnlVect* deltas_std);
	};


	// Monte Carlo Simulation From 0
	class MonteCarloAtOrigin : public MonteCarlo {
	private:
		const PnlVect* spots;
	
	protected:
		PnlMat* get_path() const {
			return model.simulate_path_from_zero(spots);
		}

		const PnlMat* get_past() const {
			PnlMat* past = pnl_mat_create(1, spots->size);
			pnl_mat_set_row(past, spots, 0);
			return past;
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
		PnlMat* get_path() const {
			return model.simulate_path_from_t(t, past);
		}

		const PnlMat* get_past() const {
			return past;
		}
	
	public:
		MonteCarloAtTimeT(const options::IOption& _option, const models::Model& _model, const unsigned long _samples, const double _t, const PnlMat* _past) :
			MonteCarlo(_option, _model, _samples, _t, 0.0), past(_past) {}
		MonteCarloAtTimeT(const options::IOption& _option, const models::Model& _model, const unsigned long _samples, const double _t, const PnlMat* _past, const double _eps) :
			MonteCarlo(_option, _model, _samples, _t, _eps), past(_past) {}

		
		
		
	};
}