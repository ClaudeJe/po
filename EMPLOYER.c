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
    char motDePasse[100];  // Le mot de passe (non visible par l'administrateur)
} employer;

// Déclaration des fonctions
void connexionDirigerEmployer();
void afficherEmployeur(const employer *emp);
void saisirMotDePasse1(char *motDePasse);
void redirigerVersPoste(const char *poste, const employer *emp);

// Saisie masquée du mot de passe
void saisirMotDePasse1(char *motDePasse) {
    char ch;
    int i = 0;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            motDePasse[i++] = ch;
            printf("*");
        }
    }
    motDePasse[i] = '\0';
    printf("\n");
}

// Connexion employé (enregistrement du mot de passe à la première connexion)
void connexionDirigerEmployer() {
    char nom[100], prenom[100], mdp[100];
    employer courant;
    int trouve = 0, maj_mdp = 0;

    system("cls");
    setColor(14);
    printf("===== 🔐 CONNEXION EMPLOYÉ =====\n\n");
    setColor(7);
    printf("👤 Nom      : ");
    scanf(" %[^\n]", nom);
    printf("🧾 Prénom   : ");
    scanf(" %[^\n]", prenom);
    printf("🔑 Mot de passe : ");
    saisirMotDePasse1(mdp);

    FILE *f = fopen("employes.csv", "r");
    FILE *temp = fopen("employes_temp.csv", "w");
    if (!f || !temp) {
        setColor(12);
        printf("❌ Erreur d'ouverture des fichiers.\n");
        setColor(7);
        return;
    }

    char ligne[512];
    while (fgets(ligne, sizeof(ligne), f)) {
        employer emp_temp;
        int lus = sscanf(ligne, "%[^,],%[^,],%d,%d,%[^,],%[^,],%[^\n]",
            emp_temp.Nom, emp_temp.Prenom, &emp_temp.Age, &emp_temp.salaire,
            emp_temp.Id, emp_temp.Poste, emp_temp.motDePasse);

        if (lus == 7 && strcmp(emp_temp.Nom, nom) == 0 && strcmp(emp_temp.Prenom, prenom) == 0) {
            // Première connexion : mot de passe vide
            if (strlen(emp_temp.motDePasse) == 0 || strcmp(emp_temp.motDePasse, " ") == 0) {
                strcpy(emp_temp.motDePasse, mdp);
                maj_mdp = 1;
                trouve = 1;
                courant = emp_temp;
                setColor(10);
                printf("🔐 Premier mot de passe enregistré avec succès !\n");
            }
            // Connexions suivantes : on compare seulement
            else if (strcmp(emp_temp.motDePasse, mdp) == 0) {
                trouve = 1;
                courant = emp_temp;
            }
        }

        // Réécriture de la ligne (avec éventuellement le mot de passe mis à jour)
        fprintf(temp, "%s,%s,%d,%d,%s,%s,%s\n",
            emp_temp.Nom, emp_temp.Prenom, emp_temp.Age, emp_temp.salaire,
            emp_temp.Id, emp_temp.Poste, emp_temp.motDePasse);
    }

    fclose(f);
    fclose(temp);

    if (maj_mdp) {
        remove("employes.csv");
        rename("employes_temp.csv", "employes.csv");
    } else {
        remove("employes_temp.csv");
    }

    if (trouve) {
        setColor(10);
        printf("\n✅ Connexion réussie ! Bienvenue %s %s (poste : %s)\n", courant.Prenom, courant.Nom, courant.Poste);
        afficherEmployeur(&courant);
        printf("\nAppuyez sur une touche pour accéder à votre interface...\n");
        getch();
        redirigerVersPoste(courant.Poste, &courant);
    } else {
        setColor(12);
        printf("\n❌ Nom, prénom ou mot de passe incorrect.\n");
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
        // Code pour rediriger vers l'interface de réception
    } else if (strcmp(poste, "Serveur") == 0) {
        printf("🍽️  Services disponibles pour votre poste : Prise de commandes, Service en salle, Assistance aux clients.\n");
        // Code pour rediriger vers l'interface du serveur
    } else if (strcmp(poste, "Cuisinier") == 0) {
        printf("👨‍🍳 Services disponibles pour votre poste : Préparation des repas, Gestion de la cuisine.\n");
        // Code pour rediriger vers l'interface du cuisinier
    } else if (strcmp(poste, "Femme de ménage") == 0) {
        printf("🧹 Services disponibles pour votre poste : Nettoyage des chambres, Entretien des espaces publics.\n");
        // Code pour rediriger vers l'interface de nettoyage
    } else {
        setColor(12);
        printf("❌ Poste inconnu. Accès non autorisé.\n");
    }
    setColor(7);
}
