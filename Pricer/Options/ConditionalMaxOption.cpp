#include "IOption.hpp"
#include "OptionParameters.hpp"

namespace options {

    class ConditionalMaxOption : public IOption {
    private:
        OptionParameters& parameters;

    public:
        ConditionalMaxOption(OptionParameters& params) : parameters(params) {}


        double payoff(const PnlMat* const underlying_paths) const override {
            //TODO: Implementation of the payoff function
        }
    };

}