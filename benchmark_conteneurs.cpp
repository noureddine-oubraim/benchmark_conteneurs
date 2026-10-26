/**
 * =============================================================================
 * BENCHMARK : std::vector vs std::list
 * =============================================================================
 *
 * Ce programme compare les performances des deux conteneurs de la STL C++ :
 *   - std::vector  : tableau dynamique contigu en memoire
 *   - std::list    : liste doublement chainees
 *
 * Operations mesurees (avec iterateurs) :
 *   1. Insertion   -> a la fin, au debut, au milieu
 *   2. Suppression -> a la fin, au debut, au milieu
 *   3. Recherche   -> parcours complet avec iterateur (std::find)
 *   4. Modification-> via iterateur
 *   5. Lecture     -> parcours sequentiel avec iterateur
 *
 * Conclusion : affiche quel conteneur est le meilleur pour chaque cas.
 * =============================================================================
 */

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>   // std::find, std::advance
#include <chrono>
#include <iomanip>
#include <string>
#include <numeric>     // std::iota

// ─────────────────────────────────────────────────────────────────────────────
// Utilitaires de mesure du temps
// ─────────────────────────────────────────────────────────────────────────────

using Horloge  = std::chrono::high_resolution_clock;
using Duree_us = std::chrono::duration<double, std::micro>;  // microsecondes

/** Retourne le temps ecoule en microsecondes entre deux points. */
double duree_us(Horloge::time_point debut, Horloge::time_point fin) {
    return Duree_us(fin - debut).count();
}

// ─────────────────────────────────────────────────────────────────────────────
// Affichage
// ─────────────────────────────────────────────────────────────────────────────

constexpr int LARGEUR_COL = 15;

void afficher_entete() {
    std::cout << "\n";
    std::cout << std::string(74, '=') << "\n";
    std::cout << "   BENCHMARK  std::vector  vs  std::list   (temps en microsecondes)\n";
    std::cout << std::string(74, '=') << "\n";
    std::cout << std::left
              << std::setw(32) << "Operation"
              << std::setw(LARGEUR_COL) << "vector (us)"
              << std::setw(LARGEUR_COL) << "list (us)"
              << "Vainqueur"
              << "\n";
    std::cout << std::string(74, '-') << "\n";
}

void afficher_ligne(const std::string& operation,
                    double tps_vec,
                    double tps_lst,
                    int& score_vec,
                    int& score_lst) {
    std::string vainqueur;
    if (tps_vec < tps_lst) {
        vainqueur = ">>> vector";
        ++score_vec;
    } else if (tps_lst < tps_vec) {
        vainqueur = ">>> list";
        ++score_lst;
    } else {
        vainqueur = "   egal";
    }

    std::cout << std::left
              << std::setw(32) << operation
              << std::setw(LARGEUR_COL) << std::fixed << std::setprecision(3) << tps_vec
              << std::setw(LARGEUR_COL) << tps_lst
              << vainqueur
              << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Parametres du benchmark
// ─────────────────────────────────────────────────────────────────────────────

constexpr int N           = 100'000;   // nombre d'elements initiaux
constexpr int N_OPS       = 10'000;    // nombre d'operations repetees
constexpr int VALEUR_TEST = 42;        // valeur cherchee / modifiee

// ─────────────────────────────────────────────────────────────────────────────
// Remplissage initial des conteneurs
// ─────────────────────────────────────────────────────────────────────────────

std::vector<int> creer_vector() {
    std::vector<int> v(N);
    std::iota(v.begin(), v.end(), 0);
    return v;
}

std::list<int> creer_list() {
    std::list<int> l;
    for (int i = 0; i < N; ++i) l.push_back(i);
    return l;
}

// ─────────────────────────────────────────────────────────────────────────────
// 1. INSERTION
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Insertion a la FIN.
 * vector : push_back()   O(1) amorti
 * list   : push_back()   O(1) toujours
 */
void bench_insertion_fin(double& tps_vec, double& tps_lst) {
    {
        std::vector<int> v = creer_vector();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i)
            v.push_back(i);
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        std::list<int> l = creer_list();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i)
            l.push_back(i);
        tps_lst = duree_us(debut, Horloge::now());
    }
}

/**
 * Insertion au DEBUT.
 * vector : insert(begin(), val)  O(N)  -> decalage de tous les elements
 * list   : push_front(val)       O(1)  -> mise a jour du pointeur tete
 */
void bench_insertion_debut(double& tps_vec, double& tps_lst) {
    {
        std::vector<int> v = creer_vector();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i)
            v.insert(v.begin(), i);         // iterateur begin()
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        std::list<int> l = creer_list();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i)
            l.push_front(i);                // O(1) garanti
        tps_lst = duree_us(debut, Horloge::now());
    }
}

/**
 * Insertion au MILIEU.
 * vector : insert(it_milieu, val)  O(N) -> decalage de la moitie
 * list   : advance(it, N/2)        O(N) pour trouver l'iterateur,
 *           insert(it, val)         O(1) pour l'insertion elle-meme
 */
void bench_insertion_milieu(double& tps_vec, double& tps_lst) {
    {
        std::vector<int> v = creer_vector();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i) {
            auto it = v.begin() + static_cast<int>(v.size()) / 2;
            v.insert(it, i);
        }
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        std::list<int> l = creer_list();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i) {
            auto it = l.begin();
            std::advance(it, static_cast<int>(l.size()) / 2); // avance l'iterateur
            l.insert(it, i);
        }
        tps_lst = duree_us(debut, Horloge::now());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 2. SUPPRESSION
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Suppression a la FIN.
 * vector : pop_back()  O(1)
 * list   : pop_back()  O(1)
 */
void bench_suppression_fin(double& tps_vec, double& tps_lst) {
    {
        std::vector<int> v = creer_vector();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i)
            v.pop_back();
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        std::list<int> l = creer_list();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i)
            l.pop_back();
        tps_lst = duree_us(debut, Horloge::now());
    }
}

/**
 * Suppression au DEBUT.
 * vector : erase(begin())  O(N) -> decalage de tous les elements
 * list   : pop_front()     O(1)
 */
void bench_suppression_debut(double& tps_vec, double& tps_lst) {
    {
        std::vector<int> v = creer_vector();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i)
            v.erase(v.begin());             // iterateur begin()
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        std::list<int> l = creer_list();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i)
            l.pop_front();
        tps_lst = duree_us(debut, Horloge::now());
    }
}

/**
 * Suppression au MILIEU.
 * vector : erase(it_milieu)  O(N) -> decalage de la moitie
 * list   : advance(it, N/2)  O(N) pour trouver, O(1) pour decrocher
 */
void bench_suppression_milieu(double& tps_vec, double& tps_lst) {
    {
        std::vector<int> v = creer_vector();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i) {
            auto it = v.begin() + static_cast<int>(v.size()) / 2;
            v.erase(it);
        }
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        std::list<int> l = creer_list();
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i) {
            auto it = l.begin();
            std::advance(it, static_cast<int>(l.size()) / 2);
            l.erase(it);
        }
        tps_lst = duree_us(debut, Horloge::now());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 3. RECHERCHE (avec iterateur std::find)
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Recherche lineaire de VALEUR_TEST.
 * Les deux : O(N) — mais vector profite du cache CPU (memoire contigue).
 * list : sauts de pointeurs -> cache miss frequents.
 */
void bench_recherche(double& tps_vec, double& tps_lst) {
    std::vector<int> v = creer_vector();
    std::list<int>   l = creer_list();

    // Place la valeur cherchee au milieu
    *(v.begin() + N / 2) = VALEUR_TEST;
    auto it_lst = l.begin();
    std::advance(it_lst, N / 2);
    *it_lst = VALEUR_TEST;

    volatile bool trouve = false;

    {
        auto debut = Horloge::now();
        for (int r = 0; r < N_OPS; ++r) {
            auto it = std::find(v.begin(), v.end(), VALEUR_TEST);
            trouve = (it != v.end());
        }
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        auto debut = Horloge::now();
        for (int r = 0; r < N_OPS; ++r) {
            auto it = std::find(l.begin(), l.end(), VALEUR_TEST);
            trouve = (it != l.end());
        }
        tps_lst = duree_us(debut, Horloge::now());
    }
    (void)trouve;
}

// ─────────────────────────────────────────────────────────────────────────────
// 4. MODIFICATION (via iterateur)
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Modification d'un element au milieu via iterateur.
 * vector : acces direct O(1)  (it = begin() + N/2)
 * list   : std::advance O(N), puis modification O(1)
 */
void bench_modification(double& tps_vec, double& tps_lst) {
    std::vector<int> v = creer_vector();
    std::list<int>   l = creer_list();

    {
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i) {
            auto it = v.begin() + N / 2;   // O(1) : acces direct
            *it = i;
        }
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i) {
            auto it = l.begin();
            std::advance(it, N / 2);        // O(N) : maillon par maillon
            *it = i;
        }
        tps_lst = duree_us(debut, Horloge::now());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 5. LECTURE (parcours sequentiel avec iterateur)
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Parcours complet begin() -> end().
 * vector : tres rapide -> memoire contigue, prefetch CPU efficace.
 * list   : plus lent   -> sauts de pointeurs, cache miss frequents.
 */
void bench_lecture(double& tps_vec, double& tps_lst) {
    std::vector<int> v = creer_vector();
    std::list<int>   l = creer_list();
    volatile long long somme = 0;

    {
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i) {
            long long s = 0;
            for (auto it = v.begin(); it != v.end(); ++it)
                s += *it;
            somme = s;
        }
        tps_vec = duree_us(debut, Horloge::now());
    }
    {
        auto debut = Horloge::now();
        for (int i = 0; i < N_OPS; ++i) {
            long long s = 0;
            for (auto it = l.begin(); it != l.end(); ++it)
                s += *it;
            somme = s;
        }
        tps_lst = duree_us(debut, Horloge::now());
    }
    (void)somme;
}

// ─────────────────────────────────────────────────────────────────────────────
// main
// ─────────────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "\n";
    std::cout << "Parametres : N = " << N
              << " elements, " << N_OPS << " operations par test\n";
    std::cout << "Patience, benchmark en cours...\n";

    double ins_fin_v,  ins_fin_l;
    double ins_deb_v,  ins_deb_l;
    double ins_mil_v,  ins_mil_l;
    double sup_fin_v,  sup_fin_l;
    double sup_deb_v,  sup_deb_l;
    double sup_mil_v,  sup_mil_l;
    double rech_v,     rech_l;
    double modif_v,    modif_l;
    double lect_v,     lect_l;

    bench_insertion_fin      (ins_fin_v,  ins_fin_l);
    bench_insertion_debut    (ins_deb_v,  ins_deb_l);
    bench_insertion_milieu   (ins_mil_v,  ins_mil_l);
    bench_suppression_fin    (sup_fin_v,  sup_fin_l);
    bench_suppression_debut  (sup_deb_v,  sup_deb_l);
    bench_suppression_milieu (sup_mil_v,  sup_mil_l);
    bench_recherche          (rech_v,     rech_l);
    bench_modification       (modif_v,    modif_l);
    bench_lecture            (lect_v,     lect_l);

    // ── Tableau de resultats ──────────────────────────────────────────────────
    int score_vec = 0, score_lst = 0;

    afficher_entete();

    struct Ligne { std::string nom; double tv, tl; };
    std::vector<Ligne> lignes = {
        { "Insertion FIN",        ins_fin_v,  ins_fin_l  },
        { "Insertion DEBUT",      ins_deb_v,  ins_deb_l  },
        { "Insertion MILIEU",     ins_mil_v,  ins_mil_l  },
        { "Suppression FIN",      sup_fin_v,  sup_fin_l  },
        { "Suppression DEBUT",    sup_deb_v,  sup_deb_l  },
        { "Suppression MILIEU",   sup_mil_v,  sup_mil_l  },
        { "Recherche (std::find)",rech_v,     rech_l     },
        { "Modification milieu",  modif_v,    modif_l    },
        { "Lecture sequentielle", lect_v,     lect_l     },
    };

    for (auto& lg : lignes)
        afficher_ligne(lg.nom, lg.tv, lg.tl, score_vec, score_lst);

    std::cout << std::string(74, '-') << "\n";

    // ── Conclusion ────────────────────────────────────────────────────────────
    std::cout << "\n";
    std::cout << std::string(74, '=') << "\n";
    std::cout << "  CONCLUSION\n";
    std::cout << std::string(74, '=') << "\n";
    std::cout << "  Score final  ->  vector : " << score_vec
              << "  |  list : " << score_lst << "\n\n";

    std::cout << "  QUAND utiliser VECTOR ?\n";
    std::cout << "  [+] Lecture / recherche frequente (cache CPU optimal)\n";
    std::cout << "  [+] Modification par index ou iterateur a acces direct\n";
    std::cout << "  [+] Insertions/suppressions A LA FIN (push_back rapide)\n";
    std::cout << "  [-] Evitez au debut/milieu sur grands tableaux\n\n";

    std::cout << "  QUAND utiliser LIST ?\n";
    std::cout << "  [+] Insertions/suppressions frequentes au DEBUT ou MILIEU\n";
    std::cout << "  [+] Iterateurs non invalides apres modification\n";
    std::cout << "  [-] Evitez si lecture/recherche est l'operation principale\n";
    std::cout << std::string(74, '=') << "\n\n";

    return 0;
}
