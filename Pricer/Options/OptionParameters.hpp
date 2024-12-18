#pragma once

#include "pnl/pnl_vector.h"

namespace options {

    struct OptionParameters {
    private:
        int option_size;
        PnlVect* strikes;
        PnlVect* monitoring_dates;
        PnlVect* observation_dates;

    public:
        // Constructor
        OptionParameters(int _option_size, PnlVect* _strikes, PnlVect* _monitoring_dates, PnlVect* _observation_dates)
            : option_size(_option_size), strikes(_strikes), monitoring_dates(_monitoring_dates), observation_dates(_observation_dates) {}

        // Destructor
        ~OptionParameters() {
            if (strikes) pnl_vect_free(&strikes);
            if (monitoring_dates) pnl_vect_free(&monitoring_dates);
            if (observation_dates) pnl_vect_free(&observation_dates);
        }

        // Getters
        int getOptionSize() const { return option_size; }
        const PnlVect* getStrikes() const { return strikes; }
        const PnlVect* getMonitoringDates() const { return monitoring_dates; }

        // Setters
        void setStrikes(PnlVect* _strikes) {
            if (strikes) pnl_vect_free(&strikes);
            strikes = _strikes;
            option_size = _strikes->size;
        }

        void setMonitoringDates(PnlVect* _monitoring_dates) {
            if (monitoring_dates) pnl_vect_free(&monitoring_dates);
            monitoring_dates = _monitoring_dates;
        }

        // Compute Monitoring Date Indices
        PnlVectInt* get_date_indices() const {
            int nb_monitoring_dates = monitoring_dates->size;
            int nb_observation_dates = observation_dates->size;
            PnlVectInt* date_indices = pnl_vect_int_create(nb_monitoring_dates);

            int target_idx = 0;

            for (int m = 0; m < nb_monitoring_dates; m++) {
                double target = GET(monitoring_dates, m);

                // Ensure we don't go out of bounds while finding the closest index
                while (target_idx < nb_observation_dates - 1 && GET(observation_dates, target_idx) < target) {
                    target_idx++;
                }

                // Assign the current closest index
                LET_INT(date_indices, m) = target_idx;
            }

            return date_indices; // Return the indices of monitoring dates
        }
    };

}
