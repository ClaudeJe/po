#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "utils.h"

#define MAX_CHOICE 100
// Structures
typedef struct {
    char nom[100];
    char prenom[100];
    int age;
    char profession[100];
    char identifiant[100];
    char motdepasse[100]; // <-- mot de passe ajouté ici
    char type_chambre[100];
    char date_arrivee[11];
    char date_depart[11];
} Client;

typedef struct {
    char identifiant[100];
    char nom[100];
    char service[100];
    char chambre[100];
    char date[11];
    char heure[6];
    float prix;
    int payee;
} ServiceReservation;

typedef struct {
    char identifiant[100];
    char nom[100];
    char plat[100];
    char date[11];
    char heure[6];
    float prix;
} Commande;

typedef struct {
    char nom[100];
    float prix;
} Service;

Service services[] = {
    {"Spa", 50000},
    {"Restaurant", 20000},
    {"Salle de sport", 10000},
    {"Conciergerie", 15000},
    {"Massage", 25000},
    {"Piscine", 15000}
};
int nb_services = sizeof(services) / sizeof(services[0]);

typedef struct {
    char type[100];
    float prix;
} Chambre;

Chambre chambres[] = {
    {"Standard", 30000},
    {"Suite", 70000},
    {"Deluxe", 120000}
};
int nb_chambres = sizeof(chambres) / sizeof(chambres[0]);

typedef struct {
    char nom[100];
    float prix;
} Nourriture;

Nourriture nourriture[] = {
    {"Riz au Poulet", 5000},
    {"Poisson Grillé", 7000},
    {"Steak Frites", 8000},
    {"Pâtes Carbonara", 6000},
    {"Salade César", 4500}
};
int nb_nourriture = sizeof(nourriture) / sizeof(nourriture[0]);

typedef enum {
    CARTE_BANCAIRE,
    ESPECES,
    VIREMENT_BANCAIRE
} ModePaiement;

const char *modes_paiement[] = {
    "💳 Carte bancaire",
    "💵 Espèces",
    "🏦 Virement bancaire"
};

int lastGetId() {
    FILE *f = fopen("clients.csv", "r");
    if (!f) return 1; // Premier ID
    char ligne[512];
    int lastId = 0;
    while (fgets(ligne, sizeof(ligne), f)) {
        char tmp[100];
        int id;
        // On suppose que l'identifiant est le 5e champ
        sscanf(ligne, "%*[^,],%*[^,],%*[^,],%*[^,],%[^,]", tmp);
        id = atoi(tmp);
        if (id > lastId) lastId = id;
    }
    fclose(f);
    return lastId + 1;
}

void consulterReservations(const char *identifiant) {
    FILE *f = fopen("reservations.csv", "r");
    if (!f) {
        setColor(12);
        printf("Aucune réservation trouvée.\n");
        setColor(7);
        return;
    }
    ServiceReservation r;
    char ligne[512];
    float total = 0;
    int trouve = 0;
    setColor(11);
    printf("=== Vos réservations ===\n");
    setColor(7);
    while (fgets(ligne, sizeof(ligne), f)) {
        sscanf(ligne, "%[^,],%[^,],%[^,],%[^,],%[^,],%f,%d",
            r.identifiant, r.nom, r.service, r.chambre, r.date, &r.prix, &r.payee);
        if (strcmp(r.identifiant, identifiant) == 0) {
            trouve = 1;
            printf("Service : %s | Chambre : %s | Date : %s | Prix : %.0f FCFA | Payée : %s\n",
                r.service, r.chambre, r.date, r.prix, r.payee ? "Oui" : "Non");
            total += r.prix;
        }
    }
    fclose(f);
    if (trouve) {
        setColor(14);
        printf("\nPrix total de vos réservations : %.0f FCFA\n", total);
        setColor(7);
    } else {
        setColor(12);
        printf("Aucune réservation trouvée pour votre identifiant.\n");
        setColor(7);
    }
    printf("Appuyez sur une touche pour revenir au menu client...");
    getch();
}

// Fonction pour masquer l'identifiant lors de la saisie
void saisirIdentifiantMasque(char *identifiant) {
    int i = 0;
    char ch;
    printf("Entrez votre identifiant : ");
    while ((ch = getch()) != '\r') {
        if (ch == '\b' && i > 0) {
            printf("\b \b");
            i--;
        } else if (ch != '\b' && i < 99) {
            identifiant[i++] = ch;
            printf("*");
        }
    }
    identifiant[i] = '\0';
    printf("\n");
}

// Fonction pour saisir un mot de passe masqué
void saisirMotDePasse(char *motdepasse, int maxLen) {
    int i = 0;
    char ch;
    while ((ch = getch()) != '\r' && i < maxLen - 1) {
        if (ch == '\b' && i > 0) {
            printf("\b \b");
            i--;
        } else if (ch != '\b') {
            motdepasse[i++] = ch;
            printf("*");
        }
    }
    motdepasse[i] = '\0';
    printf("\n");
}

// Fonction pour enregistrer un client
void AjouterClient(Client c) {
    FILE *f = fopen("clients.csv", "a+");
    if (!f) {
        setColor(12);
        perror("Erreur d'ouverture du fichier clients.csv");
        setColor(7);
        return;
    }
    fprintf(f, "%s,%s,%d,%s,%s,%s,%s,%s,%s\n",
        c.nom, c.prenom, c.age, c.profession,
        c.identifiant, c.motdepasse, // <-- mot de passe ici
        c.type_chambre, c.date_arrivee, c.date_depart);
    fclose(f);
    setColor(10);
    printf("\n✅ Client ajouté avec succès.\n");
    setColor(7);
}

// Fonction pour vérifier si un identifiant existe déjà
int verifier_identifiants(const char *identifiant) {
    FILE *f = fopen("clients.csv", "r");
    if (!f) return 0;
    char ligne[512];
    while (fgets(ligne, sizeof(ligne), f)) {
        char id[100];
        sscanf(ligne, "%*[^,],%*[^,],%*[^,],%*[^,],%[^,]", id);
        if (strcmp(id, identifiant) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

// Fonction de réservation (avec saisie classique)
void reservation() {
    ServiceReservation r;
    time_t t;
    struct tm *tm_info;
    char heure[6];

    // Obtenir l'heure actuelle
    time(&t);
    tm_info = localtime(&t);
    strftime(heure, 6, "%H:%M", tm_info);

    setColor(11);
    printf("\n=== 🛎️ Réservation d'un service ===\n");
    setColor(7);

    // Saisie des informations client
    printf("Identifiant client : ");
    char identifiant[100];
    scanf(" %[^\n]", identifiant);

    printf("Nom du client : ");
    char nom[100];
    scanf(" %[^\n]", nom);

    // Sélection du service avec navigation clavier
    int choix_service = 0;
    while (1) {
        system("cls");
        setColor(11);
        printf("\n=== 🛎️ Réservation d'un service ===\n");
        setColor(7);
        printf("Identifiant client : %s\n", identifiant);
        printf("Nom du client : %s\n", nom);
        printf("\nListe des services disponibles :\n");
        for (int i = 0; i < nb_services; i++) {
            if (i == choix_service) {
                setColor(13);
                printf("→ %d. %s - %.0f FCFA\n", i + 1, services[i].nom, services[i].prix);
                setColor(7);
            } else {
                printf("  %d. %s - %.0f FCFA\n", i + 1, services[i].nom, services[i].prix);
            }
        }
        printf("\nUtilisez ↑/↓ pour naviguer, Entrée pour valider.\n");

        int key = _getch();
        if (key == 224 || key == 0) {
            key = _getch();
            if (key == 72) choix_service = (choix_service == 0) ? nb_services - 1 : choix_service - 1; // Haut
            else if (key == 80) choix_service = (choix_service == nb_services - 1) ? 0 : choix_service + 1; // Bas
        } else if (key == 13) {
            break;
        }
    }
    strcpy(r.service, services[choix_service].nom);
    r.prix = services[choix_service].prix;

    // Affichage des chambres
    printf("\nListe des chambres disponibles :\n");
    for (int i = 0; i < nb_chambres; i++) {
        printf(" %d. %s - %.0f FCFA\n", i + 1, chambres[i].type, chambres[i].prix);
    }
    int choix_chambre = 0;
    do {
        printf("Choisissez une chambre (1-%d) : ", nb_chambres);
        scanf("%d", &choix_chambre);
    } while (choix_chambre < 1 || choix_chambre > nb_chambres);

    strcpy(r.chambre, chambres[choix_chambre - 1].type);
    r.prix += chambres[choix_chambre - 1].prix;

    // Saisie de la date de réservation
    printf("Date (YYYY-MM-DD) : ");
    scanf(" %[^\n]", r.date);

    // Sauvegarde de la réservation dans un fichier
    FILE *f = fopen("reservations.csv", "a");
    if (!f) {
        setColor(12);
        perror("Erreur fichier réservation");
        setColor(7);
        return;
    }

    r.payee = 0;
    fprintf(f, "%s,%s,%s,%s,%s,%.0f,%d\n", r.identifiant, r.nom, r.service, r.chambre, r.date, r.prix, r.payee);

    fclose(f);
    setColor(10);
    printf("✅ Réservation enregistrée avec succès !\n");
    printf("Service : %s | Chambre : %s | Prix : %.0f FCFA | Heure : %s\n", r.service, r.chambre, r.prix, heure);
    setColor(7);
}

// Fonction de paiement avec saisie classique
void paiement() {
    char identifiant[100];
    char service[100];
    printf("Identifiant client : ");
    scanf(" %[^\n]", identifiant);

    printf("Nom du service à payer : ");
    scanf(" %[^\n]", service);

    printf("\nModes de paiement disponibles :\n");
    for (int i = 0; i < 3; i++) {
        printf(" %d. %s\n", i + 1, modes_paiement[i]);
    }
    int choix_mode = 0;
    do {
        printf("Choisissez un mode de paiement (1-3) : ");
        scanf("%d", &choix_mode);
    } while (choix_mode < 1 || choix_mode > 3);

    setColor(10);
    printf("Réservation payée par : %s\n", modes_paiement[choix_mode - 1]);
    setColor(7);

    FILE *f = fopen("reservations.csv", "r");
    FILE *temp = fopen("reservations_temp.csv", "w");

    if (!f || !temp) {
        setColor(12);
        perror("Erreur fichier");
        setColor(7);
        return;
    }

    char ligne[512];
    int trouve = 0;
    while (fgets(ligne, sizeof(ligne), f)) {
        ServiceReservation r;
        sscanf(ligne, "%[^,],%[^,],%[^,],%[^,],%[^,],%f,%d", r.identifiant, r.nom, r.service, r.chambre, r.date, &r.prix, &r.payee);

        if (strcmp(r.identifiant, identifiant) == 1 && strcmp(r.service, service) == 1 && r.payee == 0) {
            r.payee = 1;
            trouve = 1;
            fprintf(temp, "%s,%s,%s,%s,%s,%.0f,%d\n", r.identifiant, r.nom, r.service, r.chambre, r.date, r.prix, r.payee);
        } else {
            fprintf(temp, "%s", ligne);
        }
    }

    fclose(f);
    fclose(temp);

    remove("reservations.csv");
    rename("reservations_temp.csv", "reservations.csv");

    if (trouve) {
        setColor(10);
        printf("✅ Réservation payée avec succès.\n");
    } else {
        setColor(12);
        printf("Aucune réservation non payée trouvée pour ce service.\n");
    }
    setColor(7);
}

void sInscrire() {
    Client c;
    system("cls");
    setColor(11);
    printf("===== INSCRIPTION CLIENT =====\n");
    setColor(7);

    printf("Nom : ");
    scanf(" %[^\n]", c.nom);
    printf("Prénom : ");
    scanf(" %[^\n]", c.prenom);
    printf("Âge : ");
    scanf("%d", &c.age);
    printf("Profession : ");
    scanf(" %[^\n]", c.profession);

    do {
        printf("Identifiant (unique) : ");
        scanf(" %[^\n]", c.identifiant);
        if (verifier_identifiants(c.identifiant)) {
            setColor(12);
            printf("Cet identifiant existe déjà. Veuillez en choisir un autre.\n");
            setColor(7);
        }
    } while (verifier_identifiants(c.identifiant));

    printf("Mot de passe : ");
    saisirMotDePasse(c.motdepasse, sizeof(c.motdepasse)); // <-- mot de passe ici

    printf("Type de chambre (Standard/Suite/Deluxe) : ");
    scanf(" %[^\n]", c.type_chambre);
    printf("Date d'arrivée (YYYY-MM-DD) : ");
    scanf(" %[^\n]", c.date_arrivee);
    printf("Date de départ (YYYY-MM-DD) : ");
    scanf(" %[^\n]", c.date_depart);

    AjouterClient(c);
    printf("Inscription terminée !\n");
    printf("Appuyez sur une touche pour continuer...");
    getch();
}

int seConnecter(char *identifiant) {
    char nom[100], motdepasse[100];
    int trouve = 0;
    system("cls");
    setColor(11);
    printf("===== CONNEXION CLIENT =====\n");
    setColor(7);

    printf("Nom : ");
    scanf(" %[^\n]", nom);
    printf("Mot de passe : ");
    saisirMotDePasse(motdepasse, sizeof(motdepasse)); // <-- mot de passe ici

    FILE *f = fopen("clients.csv", "r");
    if (!f) {
        setColor(12);
        printf("Erreur d'ouverture du fichier clients.csv\n");
        setColor(7);
        return 0;
    }
    char ligne[512];
    while (fgets(ligne, sizeof(ligne), f)) {
        Client c;
        sscanf(ligne, "%[^,],%[^,],%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,\n]",
            c.nom, c.prenom, &c.age, c.profession, c.identifiant, c.motdepasse, // <-- mot de passe ici
            c.type_chambre, c.date_arrivee, c.date_depart);
        if (strcmp(c.nom, nom) == 0 && strcmp(c.motdepasse, motdepasse) == 0) { // <-- mot de passe ici
            strcpy(identifiant, c.identifiant);
            trouve = 1;
            break;
        }
    }
    fclose(f);

    if (trouve) {
        setColor(10);
        printf("Connexion réussie !\n");
        setColor(7);
        return 1;
    } else {
        setColor(12);
        printf("Nom ou mot de passe incorrect.\n");
        setColor(7);
        return 0;
    }
}

void MenuClient() {
    int choix = 0;
    int continuer = 1;
    char identifiant[100];

    while (continuer) {
        // Menu principal client (avant connexion)
        choix = 0;
        while (1) {
            system("cls");
            setColor(11);
            printf("========== MENU CLIENT 👤 ==========\n");
            setColor(7);

            const char* options[] = {
                "S'inscrire 📝",
                "Se connecter 👤",
                "Quitter ❌"
            };

            for (int i = 0; i < 3; i++) {
                if (i == choix) {
                    setColor(13);
                    printf("→ %s\n", options[i]);
                    setColor(7);
                } else {
                    printf("  %s\n", options[i]);
                }
            }

            int key = _getch();
            if (key == 224 || key == 0) {
                key = _getch();
                if (key == 72) choix = (choix == 0) ? 2 : choix - 1; // Haut
                else if (key == 80) choix = (choix == 2) ? 0 : choix + 1; // Bas
            } else if (key == 13) {
                break;
            }
        }

        switch (choix) {
            case 0:
                sInscrire();
                break;
            case 1:
                if (seConnecter(identifiant)) {
                    // Menu client connecté
                    int choixClient = 0;
                    int continuerClient = 1;
                    while (continuerClient) {
                        system("cls");
                        setColor(11);
                        printf("====== Espace Client 👤 ======\n");
                        setColor(7);

                        const char* optionsClient[] = {
                            "Réserver un service 🛎️",
                            "Payer une réservation 💳",
                            "Consulter mes réservations 📄",
                            "Quitter le menu client ❌"
                        };

                        for (int i = 0; i < 4; i++) {
                            if (i == choixClient) {
                                setColor(13);
                                printf("→ %s\n", optionsClient[i]);
                                setColor(7);
                            } else {
                                printf("  %s\n", optionsClient[i]);
                            }
                        }

                        int key = _getch();
                        if (key == 224 || key == 0) {
                            key = _getch();
                            if (key == 72) choixClient = (choixClient == 0) ? 3 : choixClient - 1;
                            else if (key == 80) choixClient = (choixClient == 3) ? 0 : choixClient + 1;
                        } else if (key == 13) {
                            switch (choixClient) {
                                case 0:
                                    reservation();
                                    printf("\nAppuyez sur une touche pour revenir au menu client...");
                                    getch();
                                    system("cls"); // Nettoie le terminal après l'opération
                                    break;
                                case 1:
                                    paiement();
                                    printf("\nAppuyez sur une touche pour revenir au menu client...");
                                    getch();
                                    system("cls"); // Nettoie le terminal après l'opération
                                    break;
                                case 2:
                                    consulterReservations(identifiant);
                                    system("cls"); // Nettoie le terminal après consultation
                                    break;
                                case 3:
                                    setColor(10);
                                    printf("Retour au menu principal...\n");
                                    setColor(7);
                                    continuerClient = 0;
                                    system("cls"); // Nettoie le terminal en quittant le menu client
                                    break;
                            }
                        }
                    }
                }
                break;
            case 2:
                printf("Au revoir !\n");
                continuer = 0;
                break;
        }
    }
}