/**
 * ============================================================
 *   Parcours en Largeur (BFS) d'un Arbre Binaire
 *   Breadth-First Search (BFS) on a Binary Tree
 * ============================================================
 *
 *  Algorithme :
 *    1. Enfiler la racine dans une file (std::queue)
 *    2. Tant que la file n'est pas vide :
 *         a. Defiler le noeud courant
 *         b. Traiter / afficher le noeud
 *         c. Enfiler le fils gauche s'il existe
 *         d. Enfiler le fils droit  s'il existe
 *
 *  Fonctionnalites ajoutees :
 *    - Mesure du temps exact d'execution (en us et ns via std::chrono)
 *    - Suivi de la taille (size) et de la capacite (capacity) des conteneurs
 *    - Suivi du pic de taille de la file (consommation memoire max)
 *
 *  Complexite : O(n) temps,  O(n) espace (file au pire = 1 niveau)
 * ============================================================
 */

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <iomanip>
#include <chrono>
#include <algorithm>

// ─────────────────────────────────────────────────────────────
//  Structure d'un noeud
// ─────────────────────────────────────────────────────────────
/**
 * @brief Noeud d'un arbre binaire.
 * @param valeur  Valeur entiere stockee dans le noeud.
 * @param gauche  Pointeur vers le fils gauche (nullptr si absent).
 * @param droit   Pointeur vers le fils droit  (nullptr si absent).
 */
struct Noeud {
    int    valeur;
    Noeud* gauche;
    Noeud* droit;

    explicit Noeud(int val)
        : valeur(val), gauche(nullptr), droit(nullptr) {}
};

// ─────────────────────────────────────────────────────────────
/**
 * @brief Construit un arbre binaire a partir d'un tableau niveau par niveau.
 *
 * Representation identique a LeetCode :
 *   - Les elements sont inseres de gauche a droite, niveau par niveau.
 *   - La valeur -1 signifie "noeud absent" (nullptr).
 *
 * Exemple : {1, 2, 3, -1, 5}  donne :
 *       1
 *      / \
 *     2   3
 *      \
 *       5
 *
 * @param vals  Vecteur d'entiers representant l'arbre en largeur.
 * @return      Pointeur vers la racine de l'arbre construit.
 */
Noeud* construireArbre(const std::vector<int>& vals) {
    if (vals.empty() || vals[0] == -1) return nullptr;

    Noeud* racine = new Noeud(vals[0]);
    std::queue<Noeud*> file;
    file.push(racine);

    std::size_t i = 1;
    while (!file.empty() && i < vals.size()) {
        Noeud* courant = file.front();
        file.pop();

        // Fils gauche
        if (i < vals.size()) {
            if (vals[i] != -1) {
                courant->gauche = new Noeud(vals[i]);
                file.push(courant->gauche);
            }
            ++i;
        }
        // Fils droit
        if (i < vals.size()) {
            if (vals[i] != -1) {
                courant->droit = new Noeud(vals[i]);
                file.push(courant->droit);
            }
            ++i;
        }
    }
    return racine;
}

// ─────────────────────────────────────────────────────────────
/**
 * @brief Libere toute la memoire allouee pour l'arbre.
 *
 * Utilise un parcours BFS pour supprimer chaque noeud sans recursion.
 * A appeler apres utilisation de l'arbre pour eviter les fuites memoire.
 *
 * @param racine  Pointeur vers la racine de l'arbre a liberer.
 */
void libererArbre(Noeud* racine) {
    if (!racine) return;
    std::queue<Noeud*> file;
    file.push(racine);
    while (!file.empty()) {
        Noeud* n = file.front(); file.pop();
        if (n->gauche) file.push(n->gauche);
        if (n->droit)  file.push(n->droit);
        delete n;
    }
}

// ─────────────────────────────────────────────────────────────
/**
 * @brief Structure contenant les resultats et metriques du parcours BFS :
 *        - Les niveaux et l'ordre de visite.
 *        - La taille (size) et la capacite (capacity) des conteneurs.
 *        - Le temps d'execution exact (std::chrono haute resolution).
 */
struct ResultatBFS {
    std::vector<std::vector<int>> niveaux;         // Valeurs groupees par niveau
    std::vector<int>              ordre_plat;      // Ordre complet de visite (a plat)
    double                        duree_us = 0.0;  // Temps d'execution en microsecondes (us)
    long long                     duree_ns = 0;    // Temps d'execution en nanosecondes (ns)
    std::size_t                   taille_max_file = 0; // Pic de taille atteint par la file std::queue
    std::size_t                   total_noeuds = 0;    // Nombre total de noeuds visites (size)
};

// ─────────────────────────────────────────────────────────────
/**
 * @brief Parcourt l'arbre en largeur (BFS) et mesure en temps reel :
 *        - Le temps d'execution exact (haute precision).
 *        - La taille (size) et capacite (capacity) des vecteurs.
 *        - Le pic d'occupation memoire de la file (taille max).
 *
 * @param racine  Pointeur vers la racine de l'arbre (nullptr = arbre vide).
 * @return        Structure ResultatBFS complete.
 * @complexity    Temps O(n), Espace O(n).
 */
ResultatBFS bfs(Noeud* racine) {
    // ── Debut de la mesure chronometrique haute resolution ───
    auto t_debut = std::chrono::high_resolution_clock::now();

    ResultatBFS res;
    if (!racine) {
        auto t_fin = std::chrono::high_resolution_clock::now();
        res.duree_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t_fin - t_debut).count();
        res.duree_us = std::chrono::duration<double, std::micro>(t_fin - t_debut).count();
        return res;
    }

    std::queue<Noeud*> file;
    file.push(racine);
    res.taille_max_file = file.size();

    while (!file.empty()) {
        // Maintien du pic de la taille de la file
        if (file.size() > res.taille_max_file) {
            res.taille_max_file = file.size();
        }

        // Nombre de noeuds sur CE niveau = taille actuelle de la file
        int taille_niveau = static_cast<int>(file.size());
        std::vector<int> niveau_courant;

        for (int i = 0; i < taille_niveau; ++i) {
            Noeud* noeud = file.front();
            file.pop();

            niveau_courant.push_back(noeud->valeur);
            res.ordre_plat.push_back(noeud->valeur);

            // Enfiler les enfants pour le niveau suivant
            if (noeud->gauche) file.push(noeud->gauche);
            if (noeud->droit)  file.push(noeud->droit);
        }

        if (file.size() > res.taille_max_file) {
            res.taille_max_file = file.size();
        }

        res.niveaux.push_back(niveau_courant);
        res.total_noeuds += static_cast<std::size_t>(taille_niveau);
    }

    // ── Fin de la mesure chronometrique ───────────────────────
    auto t_fin = std::chrono::high_resolution_clock::now();
    res.duree_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t_fin - t_debut).count();
    res.duree_us = std::chrono::duration<double, std::micro>(t_fin - t_debut).count();

    return res;
}

// ─────────────────────────────────────────────────────────────
/**
 * @brief Affiche une representation ASCII de l'arbre dans la console.
 *
 * Chaque niveau est centre proportionnellement a la largeur totale de l'arbre.
 *
 * @param niveaux  Resultat de bfs() : vecteur de niveaux.
 */
void afficherArbreVisuel(const std::vector<std::vector<int>>& niveaux) {
    if (niveaux.empty()) return;

    std::size_t profondeur  = niveaux.size();
    std::size_t largeur_max = (std::size_t{1} << (profondeur - 1u)) * 4u;

    std::cout << "\n  Representation visuelle :\n\n";
    for (std::size_t lvl = 0; lvl < profondeur; ++lvl) {
        std::size_t nb     = niveaux[lvl].size();
        std::size_t espace = (nb > 0) ? (largeur_max / nb) : largeur_max;
        std::string ligne;
        for (int val : niveaux[lvl]) {
            std::string sv     = std::to_string(val);
            std::size_t sv_len = sv.size();
            std::size_t pad_total = (espace > sv_len) ? (espace - sv_len) : 0u;
            std::size_t pad_left  = pad_total / 2u;
            std::size_t pad_right = pad_total - pad_left;
            ligne += std::string(pad_left, ' ') + sv + std::string(pad_right, ' ');
        }
        std::cout << "  " << ligne << "\n";
    }
    std::cout << "\n";
}

// ─────────────────────────────────────────────────────────────
/**
 * @brief Affiche le detail complet du BFS :
 *        - Les noeuds niveau par niveau
 *        - La taille (size) et la capacite (capacity) de chaque conteneur
 *        - Le pic memoire de la file (std::queue)
 *        - Le temps exact d'execution
 *
 * @param titre  Titre ou description du test.
 * @param res    Structure ResultatBFS issue de l'execution du parcours.
 */
// ─────────────────────────────────────────────────────────────
/**
 * @brief Affiche de maniere concise et claire les resultats du BFS :
 *        - L'ordre de visite (plat et par niveau)
 *        - La taille (size) et la capacite (capacity)
 *        - Le temps d'execution exact
 *
 * @param titre  Titre ou description du test.
 * @param res    Structure ResultatBFS issue du parcours.
 */
void afficherBFS(const std::string& titre, const ResultatBFS& res) {
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << titre << "\n";
    std::cout << "----------------------------------------------------------------------\n";

    if (res.niveaux.empty()) {
        std::cout << "  Arbre vide\n";
        std::cout << "  Temps exact       : " << std::fixed << std::setprecision(3)
                  << res.duree_us << " us (" << res.duree_ns << " ns)\n\n";
        return;
    }

    // 1. Ordre de parcours BFS
    std::cout << "  Ordre BFS         : ";
    for (std::size_t i = 0; i < res.ordre_plat.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << res.ordre_plat[i];
    }
    std::cout << "\n";

    // 2. Organisation par niveau
    std::cout << "  Par niveau        : ";
    for (std::size_t lvl = 0; lvl < res.niveaux.size(); ++lvl) {
        if (lvl > 0) std::cout << " | ";
        std::cout << "N" << lvl << " [";
        for (std::size_t i = 0; i < res.niveaux[lvl].size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << res.niveaux[lvl][i];
        }
        std::cout << "]";
    }
    std::cout << "\n";

    // 3. Taille (size) et capacite (capacity) essentielles
    std::cout << "  Taille (size)     : " << res.ordre_plat.size() << " noeuds\n";
    std::cout << "  Capacite (vector) : " << res.ordre_plat.capacity()
              << " | File (queue max): " << res.taille_max_file << "\n";

    // 4. Temps exact d'execution
    std::cout << "  Temps exact       : " << std::fixed << std::setprecision(3)
              << res.duree_us << " us (" << res.duree_ns << " ns)\n\n";
}

/** Surcharge acceptant directement la liste des niveaux pour compatibilite. */
void afficherBFS(const std::string& titre, const std::vector<std::vector<int>>& niveaux) {
    ResultatBFS res;
    res.niveaux = niveaux;
    for (const auto& niv : niveaux) {
        for (int val : niv) res.ordre_plat.push_back(val);
        res.total_noeuds += niv.size();
    }
    afficherBFS(titre, res);
}

// ─────────────────────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────────────────────
int main() {
    std::cout << "\n======================================================================\n";
    std::cout << "         PARCOURS BFS D'UN ARBRE BINAIRE (METRIQUES & TEMPS)\n";
    std::cout << "======================================================================\n\n";

    // ── Exemple 1 : arbre complet (7 noeuds) ───────────────────
    {
        Noeud* arbre = construireArbre({1, 2, 3, 4, 5, 6, 7});
        auto res = bfs(arbre);
        afficherBFS("[Exemple 1] Arbre complet {1, 2, 3, 4, 5, 6, 7}", res);
        libererArbre(arbre);
    }

    // ── Exemple 2 : arbre desequilibre (6 noeuds) ─────────────
    {
        Noeud* arbre = construireArbre({10, 5, 20, 3, 7, -1, 30, 1});
        auto res = bfs(arbre);
        afficherBFS("[Exemple 2] Arbre desequilibre {10, 5, 20, 3, 7, -1, 30, 1}", res);
        libererArbre(arbre);
    }

    // ── Exemple 3 : arbre vide ─────────────────────────────────
    {
        Noeud* arbre = nullptr;
        auto res = bfs(arbre);
        afficherBFS("[Exemple 3] Arbre vide", res);
    }

    // ── Exemple 4 : arbre parfait (15 noeuds) ──────────────────
    {
        std::vector<int> vals = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        Noeud* arbre = construireArbre(vals);
        auto res = bfs(arbre);
        afficherBFS("[Exemple 4] Arbre parfait {1..15}", res);
        libererArbre(arbre);
    }

    return 0;
}
