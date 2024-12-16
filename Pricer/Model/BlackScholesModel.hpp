#pragma once
#ifndef BLACK_SCHOLES_MODEL_HPP
#define BLACK_SCHOLES_MODEL_HPP

#include "model.hpp"

namespace model {

    /**
     * @brief Classe repr�sentant le mod�le de Black-Scholes.
     */
    class BlackScholesModel : public Model {
    public:
        /**
         * @brief Constructeur du mod�le de Black-Scholes.
         * @param sigma Volatilit� de l'actif sous-jacent.
         * @param r Taux sans risque.
         */
        BlackScholesModel(double sigma, double r);

        /**
         * @brief Destructeur.
         */
        virtual ~BlackScholesModel() override = default;

        /**
         * @brief Simule les trajectoires d'actifs sous probabilit� risque-neutre.
         * @param path Matrice pour stocker les trajectoires.
         * @param T Horizon temporel de la simulation.
         * @param N Nombre de pas de temps.
         * @param S0 Valeur initiale de l'actif.
         */
        virtual void simulate(PnlMat* path, double T, int N, double S0) const override;

        /**
         * @brief Affiche les param�tres du mod�le.
         */
        virtual void printParameters() const override;

        /**
         * @brief Accesseur pour la volatilit�.
         * @return Volatilit� du mod�le.
         */
        double getSigma() const;

        /**
         * @brief Accesseur pour le taux sans risque.
         * @return Taux sans risque du mod�le.
         */
        double getRiskFreeRate() const;

    private:
        double sigma_; ///< Volatilit� de l'actif sous-jacent.
        double r_;     ///< Taux sans risque.
    };

}

#endif // BLACK_SCHOLES_MODEL_HPP