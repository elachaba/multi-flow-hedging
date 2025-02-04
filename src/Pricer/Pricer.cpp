#include <iostream>
#include "Helper/Helper.hpp"
#include "pricer.hpp"
#include "Options/IOption.hpp"
#include "Options/OptionParameters.hpp"
#include "Options/ConditionalMaxOption.cpp"
#include "Options/ConditionalBasketOption.cpp"
#include "pnl/pnl_random.h"
#include "Model/BlackScholesModel.hpp"
#include "MonteCarlo/MonteCarloPricer.hpp"
#include <time.h>



BlackScholesPricer::BlackScholesPricer(nlohmann::json& jsonParams) {
    // Handle volatility matrix
    auto volMatrix = jsonParams.at("VolCholeskyLines").get<std::vector<std::vector<double>>>();
    int rows = volMatrix.size();
    int cols = volMatrix[0].size();
    volatility = pnl_mat_create(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            pnl_mat_set(volatility, i, j, volMatrix[i][j]);
        }
    }

    // Handle payment dates vector
    auto dates = jsonParams.at("MathPaymentDates").get<std::vector<double>>();
    paymentDates = pnl_vect_create(dates.size());
    for (size_t i = 0; i < dates.size(); i++) {
        pnl_vect_set(paymentDates, i, dates[i]);
    }

    // Handle strikes vector
    auto strikeValues = jsonParams.at("Strikes").get<std::vector<double>>();
    strikes = pnl_vect_create(strikeValues.size());
    for (size_t i = 0; i < strikeValues.size(); i++) {
        pnl_vect_set(strikes, i, strikeValues[i]);
    }

    jsonParams.at("DomesticInterestRate").get_to(interestRate);
    jsonParams.at("RelativeFiniteDifferenceStep").get_to(fdStep);
    jsonParams.at("SampleNb").get_to(nSamples);
    jsonParams.at("PayoffType").get_to(payoffType);
    nAssets = volatility->n;
}

BlackScholesPricer::~BlackScholesPricer() {
    if (paymentDates) {
        pnl_vect_free(&paymentDates);
    }
    if (strikes) {
        pnl_vect_free(&strikes);
    }
    if (volatility) {
        pnl_mat_free(&volatility);
    }
}

void BlackScholesPricer::print() {
    std::cout << " === Pricer Math Parameters === " << std::endl;
    std::cout << "nAssets: " << nAssets << std::endl;
    std::cout << "fdStep: " << fdStep << std::endl;
    std::cout << "nSamples: " << nSamples << std::endl;
    std::cout << "PayoffType: " << payoffType << std::endl;
    std::cout << "strikes: ";
    pnl_vect_print_asrow(strikes);
    std::cout << "paymentDates: ";
    pnl_vect_print_asrow(paymentDates);
    std::cout << "volatility: ";
    pnl_mat_print(volatility);
    std::cout << " ========================= " << std::endl;
}

void BlackScholesPricer::priceAndDeltas(const PnlMat* past, double currentDate, bool isMonitoringDate, double& price, double& priceStdDev, PnlVect*& deltas, PnlVect*& deltasStdDev) {

    // Initialize the option based on payoffType
    options::IOption* option = nullptr;

    int option_size = past->n;
    options::OptionParameters params(option_size, strikes, paymentDates);

    // Create the appropriate option based on payoffType
    if (payoffType == "ConditionalBasket") {
        option = new options::ConditionalBasketOption(params);
    }
    else if (payoffType == "ConditionalMax") {
        option = new options::ConditionalMaxOption(params);
    }
    else {
        throw std::invalid_argument("Unknown payoff type: " + payoffType);
    }

    // Create a random number generator
    PnlRng* rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, (unsigned long)time(NULL));

    // Instantiate the Black-Scholes model
    models::BlackScholesModel model(volatility, interestRate, paymentDates, rng);


    // Create MonteCarloPricer with the number of samples
    pricer::MonteCarloPricer mc_pricer(nSamples);

    // Allocate memory for deltas and deltasStdDev
    deltas = pnl_vect_create(option_size);
    deltasStdDev = pnl_vect_create(option_size);

    // Initialize price and priceStdDev
    price = 0.0;
    priceStdDev = 0.0;

    // Call pricedelta_at method for any currentDate (including t = 0)
    mc_pricer.pricedelta_at(currentDate, model, *option, past, price, priceStdDev, deltas, deltasStdDev, fdStep);




    
}

