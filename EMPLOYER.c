#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "Employer.h"
#include "utils.h"

// Structure représentant un employé
typedef struct {
    char Nom[100];
    char Prenom[100];
    int Age;
    int salaire;
    char Id[100];
    char Poste[100];
} employer;

// Déclaration des fonctions
void connexionDirigerEmployer();
void afficherEmployeur(const employer *emp);
void redirigerVersPoste(const char *poste, const employer *emp);

// Connexion employé (sans mot de passe)
void connexionDirigerEmployer() {
    char nom[100], prenom[100];
    employer courant;
    int trouve = 0;

    system("cls");
    setColor(14);
    printf("===== 🔐 CONNEXION EMPLOYÉ =====\n\n");
    setColor(7);
    printf("👤 Nom      : ");
    scanf(" %[^\n]", nom);
    printf("🧾 Prénom   : ");
    scanf(" %[^\n]", prenom);

    FILE *f = fopen("employes.csv", "r");
    if (!f) {
        setColor(12);
        printf("❌ Erreur d'ouverture du fichier.\n");
        setColor(7);
        return;
    }

    char ligne[512];
    while (fgets(ligne, sizeof(ligne), f)) {
        employer emp_temp;
        int lus = sscanf(ligne, "%[^,],%[^,],%d,%d,%[^,],%[^\n]",
            emp_temp.Nom, emp_temp.Prenom, &emp_temp.Age, &emp_temp.salaire,
            emp_temp.Id, emp_temp.Poste);

        if (lus == 6 && strcmp(emp_temp.Nom, nom) == 0 && strcmp(emp_temp.Prenom, prenom) == 0) {
            trouve = 1;
            courant = emp_temp;
            break;
        }
    }
    fclose(f);

    if (trouve) {
        setColor(10);
        printf("\n✅ Connexion réussie ! Bienvenue %s %s (poste : %s)\n", courant.Prenom, courant.Nom, courant.Poste);
        afficherEmployeur(&courant);
        printf("\nAppuyez sur une touche pour accéder à votre interface...\n");
        getch();
        redirigerVersPoste(courant.Poste, &courant);
    } else {
        setColor(12);
        printf("\n❌ Nom ou prénom incorrect.\n");
        printf("Appuyez sur une touche pour revenir au menu...\n");
        getch();
    }

    setColor(7);
}

void afficherEmployeur(const employer *emp) {
    setColor(11);
    printf("\n👤 Nom : %s %s\n", emp->Nom, emp->Prenom);
    printf("💼 Poste : %s\n", emp->Poste);
    printf("🎂 Âge : %d\n", emp->Age);
    printf("💰 Salaire : %d Franc CFA\n", emp->salaire);
    setColor(7);
}

void redirigerVersPoste(const char *poste, const employer *emp) {
    setColor(14);
    if (strcmp(poste, "Réceptionniste") == 0) {
        printf("🛎️  Services disponibles pour votre poste : Gestion des réservations, Accueil des clients, Informations générales.\n");
    } else if (strcmp(poste, "Serveur") == 0) {
        printf("🍽️  Services disponibles pour votre poste : Prise de commandes, Service en salle, Assistance aux clients.\n");
    } else if (strcmp(poste, "Cuisinier") == 0) {
        printf("👨‍🍳 Services disponibles pour votre poste : Préparation des repas, Gestion de la cuisine.\n");
    } else if (strcmp(poste, "Femme de ménage") == 0) {
        printf("🧹 Services disponibles pour votre poste : Nettoyage des chambres, Entretien des espaces publics.\n");
    } else {
        setColor(12);
        printf("❌ Poste inconnu. Accès non autorisé.\n");
    }
    setColor(7);
}
