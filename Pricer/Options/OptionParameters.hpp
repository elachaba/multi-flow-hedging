#pragma once

#include "pnl/pnl_vector.h"

namespace options {

    struct OptionParameters {
    private:
        int option_size;
        PnlVect* strikes;
        PnlVect* monitoring_dates;

    public:
        // Constructor
        OptionParameters(int _option_size, PnlVect* _strikes, PnlVect* _monitoring_dates)
            : option_size(_option_size), strikes(_strikes), monitoring_dates(_monitoring_dates) {}

        // **Copy Constructor**
        OptionParameters(const OptionParameters& other)
            : option_size(other.option_size) {
            // Deep copy of strikes
            strikes = pnl_vect_new();
            pnl_vect_clone(strikes, other.strikes);

            // Deep copy of monitoring_dates
            monitoring_dates = pnl_vect_new();
            pnl_vect_clone(monitoring_dates, other.monitoring_dates);
        }

        // **Copy Assignment Operator**
        OptionParameters& operator=(const OptionParameters& other) {
            if (this != &other) {
                option_size = other.option_size;

                // Free existing vectors
                if (strikes) pnl_vect_free(&strikes);
                if (monitoring_dates) pnl_vect_free(&monitoring_dates);

                // Deep copy of strikes
                strikes = pnl_vect_new();
                pnl_vect_clone(strikes, other.strikes);

                // Deep copy of monitoring_dates
                monitoring_dates = pnl_vect_new();
                pnl_vect_clone(monitoring_dates, other.monitoring_dates);
            }
            return *this;
        }

        // Destructor
        ~OptionParameters() {
            if (strikes) pnl_vect_free(&strikes);
            if (monitoring_dates) pnl_vect_free(&monitoring_dates);
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

    };
}
