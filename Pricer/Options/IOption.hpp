#pragma once
#ifndef IOPTION_H
#define IOPTION_H

#include "pnl/pnl_matrix.h"

namespace options {

    class IOption {
    public:
        virtual double payoff(PnlMat* underlying_paths) const = 0;
    };

}

#endif
