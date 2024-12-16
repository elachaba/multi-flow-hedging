#pragma once

#include "pnl/pnl_matrix.h"

namespace options {

    class IOption {
    public:
        virtual double payoff(PnlMat* underlying_paths) const = 0;
    };

}
