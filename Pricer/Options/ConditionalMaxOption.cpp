#include "IOption.hpp"
#include "OptionParameters.hpp"

namespace options {

    class ConditionalMaxOption : public IOption {
    private:
        OptionParameters& parameters;

    public:
        ConditionalBasketOption(OptionParameters& params) : parameters(params) {}


        double payoff(PnlMat* underlying_paths) const override {
            //TODO: Implementation of the payoff function
        }
    };

}