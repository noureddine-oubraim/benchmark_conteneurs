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
 *  Complexite : O(n) temps,  O(n) espace (file au pire = 1 niveau)
 * ============================================================
 */

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <iomanip>

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
 * @brief Parcourt l'arbre en largeur (BFS - Breadth First Search).
 *
 * Utilise une file (std::queue) pour visiter les noeuds niveau par niveau :
 *   1. Enfiler la racine.
 *   2. Tant que la file n'est pas vide :
 *        - Defiler le noeud courant.
 *        - Stocker sa valeur dans le niveau courant.
 *        - Enfiler ses enfants gauche et droit s'ils existent.
 *
 * @param racine  Pointeur vers la racine de l'arbre (nullptr = arbre vide).
 * @return        Vecteur de vecteurs : chaque sous-vecteur contient les valeurs
 *                d'un niveau de l'arbre, de la racine vers les feuilles.
 * @complexity    Temps O(n), Espace O(n).
 */
std::vector<std::vector<int>> bfs(Noeud* racine) {
    std::vector<std::vector<int>> niveaux;
    if (!racine) return niveaux;

    std::queue<Noeud*> file;
    file.push(racine);

    while (!file.empty()) {
        // Nombre de noeuds sur CE niveau = taille actuelle de la file
        int taille_niveau = static_cast<int>(file.size());
        std::vector<int> niveau_courant;

        for (int i = 0; i < taille_niveau; ++i) {
            Noeud* noeud = file.front();
            file.pop();

            niveau_courant.push_back(noeud->valeur);

            // Enfiler les enfants pour le prochain niveau
            if (noeud->gauche) file.push(noeud->gauche);
            if (noeud->droit)  file.push(noeud->droit);
        }
        niveaux.push_back(niveau_courant);
    }
    return niveaux;
}

// ─────────────────────────────────────────────────────────────
/**
 * @brief Affiche une representation ASCII de l'arbre dans la console.
 *
 * Chaque niveau est centre proportionnellement a la largeur totale de l'arbre.
 * Necessite le resultat de bfs() en entree.
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
        std::size_t espace = largeur_max / nb;
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
 * @brief Affiche le resultat du BFS de facon formatee dans la console.
 *
 * Affiche les noeuds niveau par niveau ainsi que l'ordre de visite
 * complet sous forme "v1 -> v2 -> ... -> vn".
 *
 * @param titre    Titre de l'exemple affiche dans l'encadre.
 * @param niveaux  Resultat de bfs() : vecteur de niveaux.
 */
void afficherBFS(const std::string& titre,
                 const std::vector<std::vector<int>>& niveaux) {
    std::cout << "==========================================================================\n";
    std::cout << "   " << titre << "\n";
    std::cout << "   PARCOURS EN LARGEUR (BFS) - Resultat niveau par niveau\n";
    std::cout << "==========================================================================\n";

    if (niveaux.empty()) {
        std::cout << "  Arbre vide.\n";
        std::cout << "==========================================================================\n\n";
        return;
    }

    int total = 0;
    for (std::size_t lvl = 0; lvl < niveaux.size(); ++lvl) {
        std::cout << "  Niveau " << std::setw(2) << lvl << "  :  [ ";
        for (int val : niveaux[lvl]) {
            std::cout << std::setw(3) << val << " ";
            ++total;
        }
        std::cout << "]\n";
    }

    std::cout << "--------------------------------------------------------------------------\n";
    std::cout << "  Ordre plat  :  ";
    bool first = true;
    for (auto& niveau : niveaux) {
        for (int val : niveau) {
            if (!first) std::cout << " -> ";
            std::cout << val;
            first = false;
        }
    }
    std::cout << "\n";
    std::cout << "  Noeuds visites : " << total << "\n";
    std::cout << "==========================================================================\n\n";
}

// ─────────────────────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────────────────────
int main() {
    std::cout << "\n";
    std::cout << "==========================================================================\n";
    std::cout << "        DEMO BFS (Parcours en Largeur) sur Arbre Binaire\n";
    std::cout << "==========================================================================\n\n";

    // ── Exemple 1 : arbre complet a 3 niveaux ─────────────────
    //
    //          1
    //        /   ╲
    //       2     3
    //      / ╲   / ╲
    //     4   5 6   7
    //
    {
        std::cout << "[Exemple 1]  Arbre complet  {1, 2, 3, 4, 5, 6, 7}\n";
        Noeud* arbre = construireArbre({1, 2, 3, 4, 5, 6, 7});
        auto   niv   = bfs(arbre);
        afficherArbreVisuel(niv);
        afficherBFS("Exemple 1", niv);
        libererArbre(arbre);
    }

    // ── Exemple 2 : arbre desequilibre ────────────────────────
    //
    //          10
    //         /  ╲
    //        5    20
    //       / ╲     ╲
    //      3   7     30
    //     /
    //    1
    //
    {
        std::cout << "[Exemple 2]  Arbre desequilibre  {10, 5, 20, 3, 7, -1, 30, 1}\n";
        Noeud* arbre = construireArbre({10, 5, 20, 3, 7, -1, 30, 1});
        auto   niv   = bfs(arbre);
        afficherArbreVisuel(niv);
        afficherBFS("Exemple 2", niv);
        libererArbre(arbre);
    }

    // ── Exemple 3 : arbre vide ─────────────────────────────────
    {
        std::cout << "[Exemple 3]  Arbre vide\n";
        Noeud* arbre = nullptr;
        auto   niv   = bfs(arbre);
        afficherBFS("Exemple 3", niv);
    }

    std::cout << "Fin du programme.\n\n";
    return 0;
}
