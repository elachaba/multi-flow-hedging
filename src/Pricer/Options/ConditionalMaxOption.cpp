#include "IOption.hpp"
#include "OptionParameters.hpp"
#include <cmath>
#include <algorithm>


namespace options {

    class ConditionalMaxOption : public IOption {
    private:
        OptionParameters parameters;
    public:
        ConditionalMaxOption(OptionParameters& params) : parameters(params) {}


        PnlVect* payoff(const PnlMat* const underlying_paths) const override {
            int nb_payments = underlying_paths->m;
            PnlVect* payments = pnl_vect_create(nb_payments);

            double previous_payment = 0.0; // to track P_{m-1}

            for (int m = 0; m < nb_payments; m++) {
                double max_value = -INFINITY;

                // compute the max value of S_t_m^n for all assets
                for (int n = 0; n < parameters.getOptionSize(); n++) {
                    max_value = std::max(max_value, MGET(underlying_paths, m, n));
                }
                // compute payoff
                double payoff_m = std::max(max_value - GET(parameters.getStrikes(), m), 0.0);
                LET(payments, m) = (previous_payment == 0.0) ? payoff_m : 0.0;
                previous_payment = GET(payments, m);
            }

            
            return payments;
        }

        double discounted_payoff(const PnlMat* const underlying_paths, double r, double t) const override {
            double result = 0.0;
            PnlVect* payments = payoff(underlying_paths); // Compute payoffs
            const PnlVect* monitoring_dates = parameters.getMonitoringDates();

            for (int m = 0; m < payments->size; m++) {
                double t_m = GET(monitoring_dates, m);
                if (t_m < t) continue; // Ignore dates before pricing time t
                double payment_m = GET(payments, m);
                result += payment_m * exp(-r * (t_m - t)); // Discount payoff
            }

            pnl_vect_free(&payments);
            return result;
        }
    };

}