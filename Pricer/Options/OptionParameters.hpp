#include "pnl/pnl_vect.h"

namespace options {

    struct OptionParameters {
    private:
        int option_size;
        PnlVect* strikes;
        PnlVect* dates;

    public:
        OptionParameters(PnlVect* _strikes, PnlVect* _dates)
            : option_size(_strikes->size), strikes(_strikes), dates(_dates) {}


        int getOptionSize() const { return option_size; }

        PnlVect* getStrikes() const { return strikes; }

        PnlVect* getDates() const { return dates; }

        void setStrikes(PnlVect* _strikes) {
            if (strikes) pnl_vect_free(&strikes);
            strikes = _strikes;
            option_size = _strikes->size;
        }

        void setDates(PnlVect* _dates) {
            if (dates) pnl_vect_free(&dates);
            dates = _dates;
        }
    };

}
