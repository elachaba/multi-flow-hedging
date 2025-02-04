#include "IOption.hpp"
#include "OptionParameters.hpp"
#include <algorithm>
#include "../Helper/Helper.hpp"

namespace options {

    class ConditionalBasketOption : public IOption {
    private:
        OptionParameters parameters;
    public:
        ConditionalBasketOption(const OptionParameters& params) : parameters(params) {}


        PnlVect* payoff(const PnlMat* const underlying_paths) const override {
            int nb_payments = underlying_paths->m; // Number of monitoring dates
            int nb_assets = parameters.getOptionSize(); // Number of underlying assets
            PnlVect* payments = pnl_vect_create_from_zero(nb_payments); // Initialize payments with zeros

            for (int m = 0; m < nb_payments; m++) {
                double basket_value = 0.0;

                // Compute the basket value as the average of all asset prices at time m
                for (int n = 0; n < nb_assets; n++) {
                    basket_value += MGET(underlying_paths, m, n);
                }
                basket_value /= nb_assets;

                // Compute the payoff
                double payoff_m = std::max(basket_value - GET(parameters.getStrikes(), m), 0.0);

                // Check if this is the first non-zero payoff
                if (payoff_m > 0.0) {
                    LET(payments, m) = payoff_m; // Set the payoff
                    break; // Stop further calculations
                }
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