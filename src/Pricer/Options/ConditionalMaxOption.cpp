#include "IOption.hpp"
#include "OptionParameters.hpp"
#include <cmath>
#include <algorithm>


namespace options {

    class ConditionalMaxOption : public IOption {
    private:
        OptionParameters parameters;
    public:
        ConditionalMaxOption(const OptionParameters& params) : parameters(params) {}


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
            PnlVect* payments = payoff(underlying_paths);
            const PnlVect* monitoring_dates = parameters.getMonitoringDates();

            // Calculate discount factors for all dates
            PnlVect* discount_factors = pnl_vect_create_from_scalar(monitoring_dates->size, t);
            pnl_vect_minus_vect(discount_factors, monitoring_dates);

            // Convert time differences to discount factors
            for (int m = 0; m < monitoring_dates->size; m++) {
                double time_diff = GET(discount_factors, m);  // t - t_m
                LET(discount_factors, m) = exp(r * time_diff);
            }

            // Use scalar product to compute total discounted value
            double result = pnl_vect_scalar_prod(payments, discount_factors);

            // Cleanup
            pnl_vect_free(&payments);
            pnl_vect_free(&discount_factors);

            return result;
        }
    };

}