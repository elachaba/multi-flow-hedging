#include "IOption.hpp"
#include "OptionParameters.hpp"
#include <algorithm>

namespace options {

    class ConditionalBasketOption : public IOption {
    private:
        OptionParameters parameters;
    public:
        ConditionalBasketOption(OptionParameters& params) : parameters(params) {}


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