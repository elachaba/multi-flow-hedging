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
    jsonParams.at("VolCholeskyLines").get_to(volatility);
    jsonParams.at("MathPaymentDates").get_to(paymentDates);
    jsonParams.at("Strikes").get_to(strikes);
    jsonParams.at("DomesticInterestRate").get_to(interestRate);
    jsonParams.at("RelativeFiniteDifferenceStep").get_to(fdStep);
    jsonParams.at("SampleNb").get_to(nSamples);
    jsonParams.at("PayoffType").get_to(payoffType);
    nAssets = volatility->n;
}

BlackScholesPricer::~BlackScholesPricer() {
    pnl_vect_free(&paymentDates);
    pnl_vect_free(&strikes);
    pnl_mat_free(&volatility);
}

void BlackScholesPricer::print() {
    std::cout << " === Pricer Parameters === " << std::endl;
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

    // Assuming you have access to option parameters
    int option_size = past->n;
    options::OptionParameters params(option_size, strikes, paymentDates);

    // Create the appropriate option based on payoffType
    if (payoffType == "ConditionalBasket") {
        option = new options::ConditionalBasketOption(params);
    }
    else if (payoffType == "MaxBasket") {
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
    mc_pricer.pricedelta_at(currentDate, model, *option, past, price, priceStdDev, deltas, deltasStdDev);




    
}



int main() {
    // Exemple de paramètres JSON pour initialiser le BlackScholesPricer
    nlohmann::json jsonParams = {
    {"VolCholeskyLines", {
        {0.3, 0, 0, 0, 0},
        {0.055, 0.24387496796514396, 0, 0, 0},
        {-0.0375, -0.029984627208829176, 0.24534602530130287, 0, 0},
        {-0.015, -0.011993850883531672, 0.018658826230573976, 0.09634830431658281, 0},
        {-0.0675, -0.05397232897589252, 0.08396471803758289, 0.0692641235193512, 0.4279990011937917}
    }},
    {"MathPaymentDates", {0.17063492063492064, 0.25793650793650796, 0.3333333333333333}},
    {"Strikes", {15, 19, 18}},
    {"DomesticInterestRate", 0.05},
    {"RelativeFiniteDifferenceStep", 0.1},
    {"SampleNb", 4},
    {"PayoffType", "ConditionalBasket"}
    };


    try {
        // Initialisation du pricer avec les paramètres JSON
        BlackScholesPricer pricer(jsonParams);

        // Impression des paramètres pour vérifier l'initialisation
        pricer.print();


        // Déclaration des variables pour priceAndDeltas
        PnlMat* past = pnl_mat_create_from_scalar(2, 2, 100.0); // Exemple de données passées
        double currentDate = 0.5;
        bool isMonitoringDate = true;
        double price, priceStdDev;
        PnlVect* deltas = nullptr;
        PnlVect* deltasStdDev = nullptr;

        // Appel de la méthode priceAndDeltas
        pricer.priceAndDeltas(past, currentDate, isMonitoringDate, price, priceStdDev, deltas, deltasStdDev);

        // Affichage des résultats
        std::cout << "Price: " << price << std::endl;
        std::cout << "Price StdDev: " << priceStdDev << std::endl;
        std::cout << "Deltas: ";
        pnl_vect_print_asrow(deltas);
        std::cout << "Deltas StdDev: ";
        pnl_vect_print_asrow(deltasStdDev);

        // Libération des ressources
        pnl_mat_free(&past);
        pnl_vect_free(&deltas);
        pnl_vect_free(&deltasStdDev);

    }
    catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }

    return 0;
}