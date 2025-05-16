#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "utils.h"

#define MAX_CHOICE 100

typedef struct {
    char nom[100];
    char prenom[100];
    int age;
    char profession[100];
    char identifiant[100];
    char motdepasse[100];
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
    float prix;
    int payee;
} ServiceReservation;

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

const char *modes_paiement[] = {
    "💳 Carte bancaire",
    "💵 Espèces",
    "🏦 Virement bancaire"
};

// Prototypes
void sInscrire();
int seConnecter(char *identifiant);
void reservation(const char *identifiant_client);
void paiement();
void consulterReservations(const char *identifiant);
void afficherReservations();
void AjouterClient(Client c);
void saisirMotDePasse(char *motdepasse, int maxLen);

// Génère l'identifiant suivant (auto-incrémenté à partir de 0)
int lastGetId() {
    FILE *f = fopen("clients.csv", "r");
    if (!f) return 0; // Premier ID = 0
    char ligne[512];
    int lastId = -1;
    while (fgets(ligne, sizeof(ligne), f)) {
        char tmp[100];
        int id;
        sscanf(ligne, "%*[^,],%*[^,],%*[^,],%*[^,],%[^,]", tmp);
        id = atoi(tmp);
        if (id > lastId) lastId = id;
    }
    fclose(f);
    return lastId + 1;
}

// Saisie masquée du mot de passe
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

// Ajoute un client au fichier
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
        c.identifiant, c.motdepasse,
        c.type_chambre, c.date_arrivee, c.date_depart);
    fclose(f);
    setColor(10);
    printf("\n✅ Client ajouté avec succès.\n");
    setColor(7);
}

// Inscription client
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

    int id = lastGetId();
    sprintf(c.identifiant, "%d", id);
    printf("Votre identifiant attribué automatiquement est : %s\n", c.identifiant);

    printf("Mot de passe : ");
    saisirMotDePasse(c.motdepasse, sizeof(c.motdepasse));

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

// Connexion client
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
    saisirMotDePasse(motdepasse, sizeof(motdepasse));

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
            c.nom, c.prenom, &c.age, c.profession, c.identifiant, c.motdepasse,
            c.type_chambre, c.date_arrivee, c.date_depart);
        if (strcmp(c.nom, nom) == 0 && strcmp(c.motdepasse, motdepasse) == 0) {
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

// Réservation d'un service ou d'une chambre
void reservation(const char *identifiant_client) {
    ServiceReservation r;
    time_t t;
    struct tm *tm_info;
    char heure[6];

    // Récupère le nom du client à partir de l'identifiant
    FILE *fc = fopen("clients.csv", "r");
    char nom_client[100] = "";
    if (fc) {
        char ligne[512];
        while (fgets(ligne, sizeof(ligne), fc)) {
            Client c;
            sscanf(ligne, "%[^,],%[^,],%*d,%*[^,],%[^,],%*[^,],%*[^,],%*[^,],%*[^,\n]",
                c.nom, c.prenom, c.identifiant);
            if (strcmp(c.identifiant, identifiant_client) == 0) {
                strcpy(nom_client, c.nom);
                break;
            }
        }
        fclose(fc);
    }

    strcpy(r.identifiant, identifiant_client);
    strcpy(r.nom, nom_client);

    time(&t);
    tm_info = localtime(&t);
    strftime(heure, 6, "%H:%M", tm_info);

    setColor(11);
    printf("\n=== 🛎️ Réservation d'un service ou d'une chambre ===\n");
    setColor(7);

    // Menu combiné services + chambres
    int total_options = nb_services + nb_chambres;
    int choix = 0;
    while (1) {
        system("cls");
        setColor(11);
        printf("\n=== 🛎️ Réservation d'un service ou d'une chambre ===\n");
        setColor(7);
        printf("Identifiant client : %s\n", r.identifiant);
        printf("Nom du client : %s\n", r.nom);
        printf("\nListe des services et chambres disponibles :\n");
        for (int i = 0; i < nb_services; i++) {
            if (i == choix) {
                setColor(13);
                printf("→ %d. %s - %.0f FCFA\n", i + 1, services[i].nom, services[i].prix);
                setColor(7);
            } else {
                printf("  %d. %s - %.0f FCFA\n", i + 1, services[i].nom, services[i].prix);
            }
        }
        for (int i = 0; i < nb_chambres; i++) {
            int idx = nb_services + i;
            if (idx == choix) {
                setColor(13);
                printf("→ %d. Chambre %s - %.0f FCFA\n", idx + 1, chambres[i].type, chambres[i].prix);
                setColor(7);
            } else {
                printf("  %d. Chambre %s - %.0f FCFA\n", idx + 1, chambres[i].type, chambres[i].prix);
            }
        }
        printf("\nUtilisez ↑/↓ pour naviguer, Entrée pour valider.\n");

        int key = _getch();
        if (key == 224 || key == 0) {
            key = _getch();
            if (key == 72) choix = (choix == 0) ? total_options - 1 : choix - 1;
            else if (key == 80) choix = (choix == total_options - 1) ? 0 : choix + 1;
        } else if (key == 13) {
            break;
        }
    }

    // Remplissage selon le choix
    if (choix < nb_services) {
        // Service classique, pas de choix de chambre
        strcpy(r.service, services[choix].nom);
        strcpy(r.chambre, "-");
        r.prix = services[choix].prix;
    } else {
        // Réservation directe d'une chambre
        int idx_chambre = choix - nb_services;
        strcpy(r.service, "Chambre");
        strcpy(r.chambre, chambres[idx_chambre].type);
        r.prix = chambres[idx_chambre].prix;
    }

    printf("Date (YYYY-MM-DD) : ");
    scanf(" %[^\n]", r.date);

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

// Paiement d'une réservation
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

        if (strcmp(r.identifiant, identifiant) == 0 && strcmp(r.service, service) == 0 && r.payee == 0) {
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

// Affiche les réservations du client connecté
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

// Affiche toutes les réservations
void afficherReservations() {
    FILE *f = fopen("reservations.csv", "r");
    if (!f) {
        setColor(12); printf("❌ Impossible d'ouvrir reservations.csv\n"); setColor(7);
        return;
    }
    char identifiant[100], nom[100], service[100], chambre[100], date[100];
    float prix;
    int payee;
    setColor(11); printf("=== Liste des Réservations ===\n"); setColor(7);
    int nb = 0;
    while (fscanf(f, " %[^,],%[^,],%[^,],%[^,],%[^,],%f,%d",
                  identifiant, nom, service, chambre, date, prix, &payee) == 7) {
        printf("ID: %s | Client: %s | Service: %s | Chambre: %s | Date: %s | Prix: %.0f FCFA | Payée: %s\n",
               identifiant, nom, service, chambre, date, prix, payee ? "Oui" : "Non");
        nb++;
    }
    fclose(f);
    if (nb == 0) {
        setColor(12); printf("Aucune réservation trouvée.\n"); setColor(7);
    }
    printf("\nAppuyez sur une touche pour revenir au menu client...");
    getch();
}

// Menu principal client
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
                            "Afficher toutes les réservations 📋",
                            "Quitter le menu client ❌"
                        };

                        for (int i = 0; i < 5; i++) {
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
                            if (key == 72) choixClient = (choixClient == 0) ? 4 : choixClient - 1;
                            else if (key == 80) choixClient = (choixClient == 4) ? 0 : choixClient + 1;
                        } else if (key == 13) {
                            switch (choixClient) {
                                case 0:
                                    reservation(identifiant);
                                    printf("\nAppuyez sur une touche pour revenir au menu client...");
                                    getch();
                                    system("cls");
                                    break;
                                case 1:
                                    paiement();
                                    printf("\nAppuyez sur une touche pour revenir au menu client...");
                                    getch();
                                    system("cls");
                                    break;
                                case 2:
                                    consulterReservations(identifiant);
                                    system("cls");
                                    break;
                                case 3:
                                    afficherReservations();
                                    printf("\nAppuyez sur une touche pour revenir au menu client...");
                                    getch();
                                    system("cls");
                                    break;
                                case 4:
                                    setColor(10);
                                    printf("Retour au menu principal...\n");
                                    setColor(7);
                                    continuerClient = 0;
                                    system("cls");
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

