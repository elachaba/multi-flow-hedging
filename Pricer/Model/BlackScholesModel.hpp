#pragma once
#ifndef BLACK_SCHOLES_MODEL_HPP
#define BLACK_SCHOLES_MODEL_HPP

#include "model.hpp"

namespace model {

    /**
     * @brief Classe représentant le modèle de Black-Scholes.
     */
    class BlackScholesModel : public Model {
    public:
        /**
         * @brief Constructeur du modèle de Black-Scholes.
         * @param sigma Volatilité de l'actif sous-jacent.
         * @param r Taux sans risque.
         */
        BlackScholesModel(double sigma, double r);

        /**
         * @brief Destructeur.
         */
        virtual ~BlackScholesModel() override = default;

        /**
         * @brief Simule les trajectoires d'actifs sous probabilité risque-neutre.
         * @param path Matrice pour stocker les trajectoires.
         * @param T Horizon temporel de la simulation.
         * @param N Nombre de pas de temps.
         * @param S0 Valeur initiale de l'actif.
         */
        virtual void simulate(PnlMat* path, double T, int N, double S0) const override;

        /**
         * @brief Affiche les paramètres du modèle.
         */
        virtual void printParameters() const override;

        /**
         * @brief Accesseur pour la volatilité.
         * @return Volatilité du modèle.
         */
        double getSigma() const;

        /**
         * @brief Accesseur pour le taux sans risque.
         * @return Taux sans risque du modèle.
         */
        double getRiskFreeRate() const;

    private:
        double sigma_; ///< Volatilité de l'actif sous-jacent.
        double r_;     ///< Taux sans risque.
    };

}

#endif // BLACK_SCHOLES_MODEL_HPP
