#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "pnl/pnl_matrix.h"

namespace model {

    class Model {
    public:
        virtual ~Model() = default; // Destructeur virtuel

        /**
         * @brief Simule les trajectoires d'actifs sous probabilit� risque-neutre.
         * @param path Matrice pour stocker les trajectoires.
         * @param T Horizon temporel de la simulation.
         * @param N Nombre de pas de temps.
         * @param S0 Valeur initiale de l'actif.
         */
        virtual void simulate(PnlMat* path, double T, int N, double S0) const = 0;

        /**
         * @brief Affiche les param�tres du mod�le.
         */
        virtual void printParameters() const = 0;
    };

}

#endif
