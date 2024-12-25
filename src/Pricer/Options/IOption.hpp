#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

namespace options {

    class IOption {
    public:
        virtual PnlVect* payoff(const PnlMat* const underlying_paths) const = 0;
		
        /**
        * @brief Actualise (discount) the payoffs to the pricing time t.
        * @param payoffs Vector of payoffs for each monitoring date.
        * @param monitoring_dates Vector of monitoring dates.
        * @param r Risk-free interest rate.
        * @param t Pricing time.
        * @return The total discounted payoff.
        */
        virtual double discounted_payoff(const PnlMat* const underlying_paths, double r, double t) const = 0;
    };

}
