#include <iostream> // Entrées/Sorties (std::cout, std::cin)
#include <algorithm> // Fonctions utiles sur les collections (std::fill, std::max, std::min)
#include <string>   // Gestion des chaînes de caractères std::string
#include <vector>   // Conteneur dynamique std::vector
#include <chrono>   // Mesure du temps d'exécution haute précision
#include <fstream>  // Lecture de fichiers (std::ifstream)
#include <sstream>  // Flux de traitement de chaînes (std::stringstream)
#include <filesystem> // Gestion des chemins de fichiers
#include <iomanip>  // Formatage des nombres (std::fixed, std::setprecision)

/**
 * 1. METHODE 1 : Recherche "From Scratch" (Algorithme naïf)
 * 
 * Complexité Temporelle (Pire cas) : O(N * M)
 * Complexité Spatiale   (Pire cas) : O(K)
 */
std::vector<size_t> rechercherFromScratch(const std::string& motPrincipal, const std::string& sousMot) {
    std::vector<size_t> positions;


    size_t n = motPrincipal.length();
    size_t m = sousMot.length();

    if (m == 0 || m > n) {
        return positions;
    }

    for (size_t i = 0; i <= n - m; ++i) {
        bool trouve = true;
        for (size_t j = 0; j < m; ++j) {
            if (motPrincipal[i + j] != sousMot[j]) {
                trouve = false;
                break;
            }
        }
        if (trouve) {
            positions.push_back(i);
        }
    }

    return positions;
}

/**
 * 2. METHODE 2 : Recherche avec std::string::find
 * 
 * Complexité Temporelle (Pire cas) : O(N * M)
 * Complexité Spatiale   (Pire cas) : O(K)
 */
std::vector<size_t> rechercherAvecFind(const std::string& motPrincipal, const std::string& sousMot) {
    std::vector<size_t> positions;

    if (sousMot.empty() || sousMot.length() > motPrincipal.length()) {
        return positions;
    }

    size_t pos = motPrincipal.find(sousMot, 0);
    while (pos != std::string::npos) {
        positions.push_back(pos);
        pos = motPrincipal.find(sousMot, pos + 1);
    }

    return positions;
}

/**
 * 3. METHODE 3 : Recherche Knuth-Morris-Pratt (KMP)
 * Complexite temporelle : O(N + M), memoire supplementaire : O(M).
 */
std::vector<size_t> rechercherKMP(const std::string& motPrincipal, const std::string& sousMot) {
    // Prépare la liste des positions où le motif est trouvé.
    std::vector<size_t> positions;
    // Récupère les longueurs du texte et du motif pour éviter de les recalculer.
    const size_t longueurTexte = motPrincipal.length();
    const size_t longueurMotif = sousMot.length();

    // Comme les autres méthodes, ne recherche pas un motif vide ou plus long que le texte.
    if (longueurMotif == 0 || longueurMotif > longueurTexte) {
        return positions;
    }

    // Le tableau LPS mémorise les préfixes réutilisables après une différence.
    std::vector<size_t> tableauLPS(longueurMotif, 0);
    // longueurPrefixe indique la taille du préfixe égal au suffixe déjà trouvé.
    size_t longueurPrefixe = 0;
    // Commence au deuxième caractère du motif pour construire le tableau LPS.
    size_t indiceMotif = 1;

    // Calcule, pour chaque préfixe, le plus long préfixe propre qui est aussi un suffixe.
    while (indiceMotif < longueurMotif) {
        // Si les caractères correspondent, le préfixe commun grandit d'un caractère.
        if (sousMot[indiceMotif] == sousMot[longueurPrefixe]) {
            ++longueurPrefixe;
            tableauLPS[indiceMotif] = longueurPrefixe;
            ++indiceMotif;
        }
        // En cas de différence, essaie un préfixe commun plus court déjà calculé.
        else if (longueurPrefixe > 0) {
            longueurPrefixe = tableauLPS[longueurPrefixe - 1];
        }
        // Sans préfixe commun à réutiliser, la valeur LPS de cette position reste zéro.
        else {
            tableauLPS[indiceMotif] = 0;
            ++indiceMotif;
        }
    }

    // indiceTexte parcourt le texte et indiceMotif parcourt le motif.
    size_t indiceTexte = 0;
    indiceMotif = 0;

    // Cherche le motif sans revenir en arrière dans le texte.
    while (indiceTexte < longueurTexte) {
        // Une correspondance fait avancer les deux indices.
        if (motPrincipal[indiceTexte] == sousMot[indiceMotif]) {
            ++indiceTexte;
            ++indiceMotif;

            // Quand tout le motif correspond, mémorise son début dans le texte.
            if (indiceMotif == longueurMotif) {
                positions.push_back(indiceTexte - longueurMotif);
                // Réutilise le tableau LPS pour aussi détecter les occurrences chevauchantes.
                indiceMotif = tableauLPS[indiceMotif - 1];
            }
        }
        // En cas de différence, reprend au meilleur préfixe connu sans reculer dans le texte.
        else if (indiceMotif > 0) {
            indiceMotif = tableauLPS[indiceMotif - 1];
        }
        // Si aucun caractère du motif ne correspond, avance simplement dans le texte.
        else {
            ++indiceTexte;
        }
    }

    // Renvoie toutes les positions trouvées, comme les deux autres méthodes.
    return positions;
}

/**
 * Remplace toutes les occurrences d'une sous-chaîne par une autre.
 * Exemple : remplacerSousChaine("bonjour monde", "monde", "amis") -> "bonjour amis"
 */
std::string remplacerSousChaine(const std::string& texte, const std::string& ancien, const std::string& nouveau) {
    if (ancien.empty()) {
        return texte;
    }

    std::string resultat;
    size_t debut = 0;

    while (debut <= texte.length()) {
        size_t pos = texte.find(ancien, debut);
        if (pos == std::string::npos) {
            resultat += texte.substr(debut);
            break;
        }

        resultat += texte.substr(debut, pos - debut);
        resultat += nouveau;
        debut = pos + ancien.length();
    }

    return resultat;
}

/**
 * Indique si un octet appartient à un mot composé de lettres ou de chiffres.
 * Les octets UTF-8 sont conservés comme partie d'un mot.
 */
bool estOctetDeMot(unsigned char caractere) {
    return (caractere >= 'a' && caractere <= 'z')
        || (caractere >= 'A' && caractere <= 'Z')
        || (caractere >= '0' && caractere <= '9')
        || caractere >= 0x80;
}

/**
 * Indique si l'octet à cette position est une continuation d'un caractère UTF-8.
 */
bool estContinuationUtf8(const std::string& texte, int position) {
    if (position >= static_cast<int>(texte.length())) {
        return false;
    }
    return (static_cast<unsigned char>(texte[position]) & 0xC0) == 0x80;
}

/**
 * Trouve la plus longue partie propre d'un mot qui apparaît au moins deux fois.
 * Le tableau de suffixes permet de traiter efficacement les fichiers volumineux.
 */
std::vector<std::string> trouverPlusLonguesSousChainesRepetees(const std::string& texte) {
    // Convertit la longueur en entier pour stocker les positions dans des tableaux compacts.
    const int taille = static_cast<int>(texte.length());
    // Un texte de moins de deux caractères ne peut pas contenir de répétition.
    if (taille < 2) {
        return {};
    }

    // longueurAutorisee indique la taille maximale d'une partie de mot à chaque position.
    std::vector<int> longueurAutorisee(taille, 0);
    // Parcourt le texte pour repérer les mots délimités par espaces ou ponctuation.
    for (int debutMot = 0; debutMot < taille;) {
        // Ignore les espaces et les signes qui séparent les mots.
        if (!estOctetDeMot(static_cast<unsigned char>(texte[debutMot]))) {
            ++debutMot;
            continue;
        }

        // Trouve la fin du mot courant.
        int finMot = debutMot;
        while (finMot < taille && estOctetDeMot(static_cast<unsigned char>(texte[finMot]))) {
            ++finMot;
        }

        // Autorise les sous-chaînes dans le mot, mais jamais le mot entier.
        int longueurMot = finMot - debutMot;
        for (int position = debutMot; position < finMot; ++position) {
            longueurAutorisee[position] = std::min(finMot - position, longueurMot - 1);
        }
        debutMot = finMot;
    }

    // suffixes contiendra les positions de départ, triées selon le texte qui suit chaque position.
    std::vector<int> suffixes(taille);
    // temporaire sert de tableau intermédiaire pendant le tri par comptage.
    std::vector<int> temporaire(taille);
    // rangs indique l'ordre connu des préfixes de suffixes.
    std::vector<int> rangs(taille);
    // nouveauxRangs reçoit les rangs calculés à chaque étape du tri.
    std::vector<int> nouveauxRangs(taille);
    // compte sert à compter les suffixes pour chaque rang lors du tri.
    std::vector<int> compte(std::max(taille + 1, 257));

    // Initialise un suffixe à chaque caractère du texte.
    for (int i = 0; i < taille; ++i) {
        suffixes[i] = i;
        // Le caractère est traité comme un octet pour rester compatible avec std::string.
        rangs[i] = static_cast<unsigned char>(texte[i]);
    }

    // Le nombre initial de rangs possibles correspond aux valeurs possibles d'un octet.
    int nombreRangs = 256;
    // longueurPrefixe est la longueur des préfixes déjà pris en compte dans le tri.
    for (int longueurPrefixe = 1;
        longueurPrefixe < taille && (nombreRangs < taille || longueurPrefixe == 1);
        longueurPrefixe *= 2) {
        // Trie les suffixes selon la partie située après le préfixe courant.
        std::fill(compte.begin(), compte.begin() + nombreRangs + 1, 0);
        for (int suffixe : suffixes) {
            int rangSuivant = suffixe + longueurPrefixe < taille
                ? rangs[suffixe + longueurPrefixe] + 1
                : 0;
            ++compte[rangSuivant];
        }
        // Transforme les fréquences en positions de départ dans le tableau trié.
        int position = 0;
        for (int rang = 0; rang <= nombreRangs; ++rang) {
            int frequence = compte[rang];
            compte[rang] = position;
            position += frequence;
        }
        // Place chaque suffixe dans l'ordre de son rang suivant.
        for (int suffixe : suffixes) {
            int rangSuivant = suffixe + longueurPrefixe < taille
                ? rangs[suffixe + longueurPrefixe] + 1
                : 0;
            temporaire[compte[rangSuivant]++] = suffixe;
        }

        // Trie le résultat selon le rang du préfixe courant.
        std::fill(compte.begin(), compte.begin() + nombreRangs + 1, 0);
        for (int suffixe : temporaire) {
            ++compte[rangs[suffixe] + 1];
        }
        // Calcule les positions de départ pour le second tri par comptage.
        position = 0;
        for (int rang = 0; rang <= nombreRangs; ++rang) {
            int frequence = compte[rang];
            compte[rang] = position;
            position += frequence;
        }
        // Produit l'ordre final selon les deux parties du préfixe.
        for (int suffixe : temporaire) {
            suffixes[compte[rangs[suffixe] + 1]++] = suffixe;
        }

        // Attribue de nouveaux rangs aux préfixes désormais deux fois plus longs.
        int nouveauNombreRangs = 1;
        nouveauxRangs[suffixes[0]] = 0;
        for (int i = 1; i < taille; ++i) {
            int suffixePrecedent = suffixes[i - 1];
            int suffixeCourant = suffixes[i];
            int rangSuivantPrecedent = suffixePrecedent + longueurPrefixe < taille
                ? rangs[suffixePrecedent + longueurPrefixe]
                : -1;
            int rangSuivantCourant = suffixeCourant + longueurPrefixe < taille
                ? rangs[suffixeCourant + longueurPrefixe]
                : -1;

            // Deux suffixes reçoivent des rangs différents si leurs deux moitiés diffèrent.
            if (rangs[suffixePrecedent] != rangs[suffixeCourant]
                || rangSuivantPrecedent != rangSuivantCourant) {
                ++nouveauNombreRangs;
            }
            nouveauxRangs[suffixeCourant] = nouveauNombreRangs - 1;
        }

        // Remplace les anciens rangs par les nouveaux pour l'itération suivante.
        rangs.swap(nouveauxRangs);
        nombreRangs = nouveauNombreRangs;

        // Évite un dépassement de capacité lorsque le texte est très grand.
        if (longueurPrefixe > taille / 2) {
            break;
        }
    }

    // inverseRangs donne la position de chaque suffixe dans le tableau trié.
    std::vector<int> inverseRangs(taille);
    for (int i = 0; i < taille; ++i) {
        inverseRangs[suffixes[i]] = i;
    }

    // Compare les suffixes voisins pour mesurer leur préfixe commun.
    int longueurCommune = 0;
    int longueurMaximale = 0;
    // Conserve un début pour chaque sous-chaîne candidate de longueur maximale.
    std::vector<int> debutsMaximaux;
    for (int debut = 0; debut < taille; ++debut) {
        int rang = inverseRangs[debut];
        // Le dernier suffixe n'a pas de voisin suivant à comparer.
        if (rang == taille - 1) {
            longueurCommune = 0;
            continue;
        }

        // Le suffixe suivant dans l'ordre est le seul candidat nécessaire ici.
        int suffixeSuivant = suffixes[rang + 1];
        // Compte les caractères identiques à partir des deux positions.
        while (debut + longueurCommune < taille
            && suffixeSuivant + longueurCommune < taille
            && texte[debut + longueurCommune] == texte[suffixeSuivant + longueurCommune]) {
            ++longueurCommune;
        }

        // Un résultat valide doit commencer au début d'un caractère UTF-8 dans les deux mots.
        if (!estContinuationUtf8(texte, debut) && !estContinuationUtf8(texte, suffixeSuivant)) {
            // Limite le préfixe commun à une partie de mot autorisée dans les deux occurrences.
            int longueurValide = std::min(longueurCommune,
                std::min(longueurAutorisee[debut], longueurAutorisee[suffixeSuivant]));

            // Évite de couper un caractère UTF-8 à la fin de la sous-chaîne.
            while (longueurValide > 0
                && (estContinuationUtf8(texte, debut + longueurValide)
                    || estContinuationUtf8(texte, suffixeSuivant + longueurValide))) {
                --longueurValide;
            }

            // Remplace les anciennes candidates lorsqu'une partie de mot plus longue est trouvée.
            if (longueurValide > longueurMaximale) {
                longueurMaximale = longueurValide;
                debutsMaximaux.clear();
                debutsMaximaux.push_back(debut);
            }
            // Conserve aussi les autres parties de mot de même longueur maximale.
            else if (longueurValide == longueurMaximale && longueurMaximale > 0) {
                debutsMaximaux.push_back(debut);
            }
        }

        // Réutilise le préfixe commun en retirant son premier caractère.
        if (longueurCommune > 0) {
            --longueurCommune;
        }
    }

    // Construit les sous-chaînes correspondant à toutes les répétitions les plus longues.
    std::vector<std::string> resultats;
    for (int debut : debutsMaximaux) {
        resultats.push_back(texte.substr(static_cast<size_t>(debut), static_cast<size_t>(longueurMaximale)));
    }

    // Trie les résultats pour regrouper les doublons, puis ne garde chaque texte qu'une fois.
    std::sort(resultats.begin(), resultats.end());
    resultats.erase(std::unique(resultats.begin(), resultats.end()), resultats.end());
    return resultats;
}

/**
 * Affiche les résultats de la recherche et le temps d'exécution
 */
void afficherResultats(const std::string& nomMethode, const std::vector<size_t>& positions, double tempsNanosecondes) {
    const double tempsSecondes = tempsNanosecondes / 1'000'000'000.0;

    std::cout << "[" << nomMethode << "] ";

    if (positions.empty()) {
        std::cout << "Le mot n'est PAS inclus.";
    } else {
        std::cout << "Le mot est INCLUS! (" << positions.size() << " occurrence(s))";
    }
    std::cout << " | Temps : " << std::fixed << std::setprecision(6)
              << tempsNanosecondes << " ns (" << tempsSecondes << " s)\n";
}

int main() {
    // Demande le fichier à analyser et garde le fichier actuel comme choix par défaut.
    std::string nomFichier;
    std::cout << "Nom du fichier a analyser (Entree pour 1millions-mot.txt) : ";
    std::getline(std::cin, nomFichier);
    if (nomFichier.empty()) {
        nomFichier = "1millions-mot.txt";
    }

    std::filesystem::path cheminFichier = std::filesystem::current_path() / nomFichier;

    if (!std::filesystem::exists(cheminFichier)) {
        std::cerr << "Erreur: Le fichier '" << nomFichier << "' est introuvable dans le dossier courant.\n";
        return 1;
    }

    std::string nomFichierComplet = cheminFichier.string();

    // Ouverture du fichier texte
    std::ifstream fichier(nomFichierComplet);
    if (!fichier.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier '" << nomFichierComplet << "'.\n";
        return 1;
    }

    // Lecture de l'intégralité du contenu du fichier dans la variable 'motPrincipal'
    std::stringstream buffer;
    buffer << fichier.rdbuf();
    std::string motPrincipal = buffer.str();
    fichier.close();

    int choix;
    std::cout << "Que voulez-vous faire ?\n";
    std::cout << "1. Rechercher un mot\n";
    std::cout << "2. Modifier le fichier\n";
    std::cout << "3. Chercher la plus longue sous-chaine repetee dans le fichier\n";
    std::cout << "Votre choix : ";
    std::cin >> choix;
    std::cin.ignore();

    if (choix == 1) {
        std::string sousMot;

        // Saisie du mot à rechercher par l'utilisateur
        std::cout << "Mot a rechercher : ";
        std::getline(std::cin, sousMot);

        // Mesure du temps : From Scratch
        auto debutScratch = std::chrono::high_resolution_clock::now();
        std::vector<size_t> resScratch = rechercherFromScratch(motPrincipal, sousMot);
        auto finScratch = std::chrono::high_resolution_clock::now();
        double tempsScratch = std::chrono::duration<double, std::nano>(finScratch - debutScratch).count();
        afficherResultats("From Scratch", resScratch, tempsScratch);

        // Mesure du temps : std::string::find
        auto debutFind = std::chrono::high_resolution_clock::now();
        std::vector<size_t> resFind = rechercherAvecFind(motPrincipal, sousMot);
        auto finFind = std::chrono::high_resolution_clock::now();
        double tempsFind = std::chrono::duration<double, std::nano>(finFind - debutFind).count();
        afficherResultats("std::string::find", resFind, tempsFind);

        // Mesure le temps de recherche de la méthode Knuth-Morris-Pratt.
        auto debutKMP = std::chrono::high_resolution_clock::now();
        std::vector<size_t> resKMP = rechercherKMP(motPrincipal, sousMot);
        auto finKMP = std::chrono::high_resolution_clock::now();
        double tempsKMP = std::chrono::duration<double, std::nano>(finKMP - debutKMP).count();
        afficherResultats("Knuth-Morris-Pratt (KMP)", resKMP, tempsKMP);
    }
    else if (choix == 2) {
        std::string ancienMot;
        std::string nouveauMot;

        std::cout << "Mot a remplacer : ";
        std::getline(std::cin, ancienMot);

        std::cout << "Nouveau mot : ";
        std::getline(std::cin, nouveauMot);

        std::string texteModifie = remplacerSousChaine(motPrincipal, ancienMot, nouveauMot);

        std::ofstream fichierSortie(nomFichierComplet, std::ios::trunc);
        if (!fichierSortie.is_open()) {
            std::cerr << "Erreur: Impossible d'ecrire dans le fichier '" << nomFichierComplet << "'.\n";
            return 1;
        }

        fichierSortie << texteModifie;
        fichierSortie.close();

        std::cout << "Modification appliquee dans le fichier '" << nomFichier << "'.\n";
    }
    else if (choix == 3) {
        // Démarre le chronomètre avant la construction du tableau de suffixes et la recherche.
        auto debutRecherche = std::chrono::steady_clock::now();
        // Cherche toutes les sous-chaînes distinctes de longueur maximale répétées dans le fichier.
        std::vector<std::string> sousChainesRepetees = trouverPlusLonguesSousChainesRepetees(motPrincipal);
        // Arrête le chronomètre une fois les résultats de la recherche construits.
        auto finRecherche = std::chrono::steady_clock::now();
        // Calcule le temps écoulé en millisecondes.
        double tempsRechercheMs = std::chrono::duration<double, std::milli>(
            finRecherche - debutRecherche).count();

        // Signale que le fichier ne contient aucun caractère répété.
        if (sousChainesRepetees.empty()) {
            std::cout << "Aucune sous-chaine repetee trouvee.\n";
        }
        // Affiche le seul résultat lorsque la longueur maximale n'a qu'une sous-chaîne.
        else if (sousChainesRepetees.size() == 1) {
            std::cout << "Plus longue sous-chaine repetee :\n";
            std::cout << sousChainesRepetees[0] << "\n";
        }
        // Affiche toutes les sous-chaînes distinctes lorsqu'il y a une égalité en longueur.
        else {
            std::cout << "Plusieurs sous-chaines ont la meme longueur maximale ("
                      << sousChainesRepetees[0].length() << " octets) :\n";
            for (const std::string& sousChaine : sousChainesRepetees) {
                std::cout << "- " << sousChaine << "\n";
            }
        }
        // Affiche la durée de tout le traitement de recherche, sans compter l'affichage des résultats.
        std::cout << "Temps total de recherche : " << std::fixed << std::setprecision(3)
                  << tempsRechercheMs << " ms\n";
    }
    else {
        std::cerr << "Choix invalide. Veuillez choisir 1, 2 ou 3.\n";
        return 1;
    }

    return 0;
}
