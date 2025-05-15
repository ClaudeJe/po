#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "GestioPers.h"

#define FICHIER_ADMIN "admin.txt"
#define FICHIER_EMPLOYES "employes.csv"
#define FICHIER_ARCHIVE "employes_archive.csv"
#define FICHIER_MDP "mots_de_passe.csv"

typedef struct {
    char Nom[100];
    char Prenom[100];
    int Age;
    int Salaire;
    int Id;
    char Poste[100];
    char MotDePasse[100];
} employer;

int adminConnecte = 1;

// 🌈 Changer la couleur du texte
void setColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 🔐 Saisie de mot de passe masquée
void saisirMotDePasseMasque(char *mdp) {
    
    char c;
    int i = 0;
    while ((c = getch()) != 13) {
        if (c == 8 && i > 0) {
            printf("\b \b");
            i--;
        } else if (c >= 32 && c <= 126 && i < 99) {
            mdp[i++] = c;
            printf("*");
        }
    }
    mdp[i] = '\0';
    printf("\n");
}

int lastGetId1() {
    FILE *f = fopen(FICHIER_EMPLOYES, "r");
    if (!f) return -1;
    employer e;
    int lastId = -1;
    while (fscanf(f, " %[^,],%[^,],%d,%d,%d,%[^\n]", e.Nom, e.Prenom, &e.Age, &e.Salaire, &e.Id, e.Poste) != EOF) {
        if (e.Id > lastId) lastId = e.Id;
    }
    fclose(f);
    return lastId;
}

// 🔐 Enregistrement administrateur
void enregistrerAdmin() {
    FILE *f = fopen(FICHIER_ADMIN, "r");
    if (f) {
        fclose(f);
        setColor(14); printf("❗ Administrateur déjà enregistré.\n"); setColor(7);
        return;
    }

    char user[100], pass[100];
    setColor(11); printf("=== Enregistrement Administrateur ===\n"); setColor(7);
    printf("Nom d'utilisateur : "); scanf(" %[^\n]", user);
    printf("Mot de passe : "); saisirMotDePasseMasque(pass);

    f = fopen(FICHIER_ADMIN, "w");
    if (!f) { perror("Erreur fichier"); return; }
    fprintf(f, "%s;%s\n", user, pass);
    fclose(f);
    setColor(10); printf("✅ Administrateur enregistré avec succès.\n"); setColor(7);
}

// 🔐 Connexion administrateur
int connecterAdmin() {
    char user[100], pass[100], fileUser[100], filePass[100];
    FILE *f = fopen(FICHIER_ADMIN, "r");
    if (!f) {
        setColor(12); printf("❌ Aucun administrateur. Veuillez vous enregistrer.\n"); setColor(7);
        return 0;
    }

    setColor(11); printf("=== Connexion Administrateur ===\n"); setColor(7);
    printf("Nom d'utilisateur : ");
    scanf(" %[^\n]", user);
    printf("Mot de passe : ");
    saisirMotDePasseMasque(pass);

    // Connexion directe si mot de passe "000000"
    if (strcmp(pass, "000000") == 0) {
        adminConnecte = 1;
        setColor(10); printf("✅ Connexion réussie (mode super admin).\n"); setColor(7);
        fclose(f);
        return 1;
    }

    // Lecture du fichier admin
    if (fscanf(f, " %[^;];%[^\n]", fileUser, filePass) != 2) {
        setColor(12); printf("❌ Fichier admin corrompu.\n"); setColor(7);
        fclose(f);
        return 0;
    }
    fclose(f);

    if (strcmp(user, fileUser) == 0 && strcmp(pass, filePass) == 0) {
        adminConnecte = 1;
        setColor(10); printf("✅ Connexion réussie.\n"); setColor(7);
        return 1;
    } else {
        setColor(12); printf("❌ Identifiants incorrects.\n"); setColor(7);
        adminConnecte = 0;
        return 0;
    }
}

// ➕ Ajouter un employé
void Formulaire() {
    if (!adminConnecte) return;
    employer e;
    setColor(11); printf("=== Ajouter un Employé ===\n"); setColor(7);
    printf("Nom : "); scanf(" %[^\n]", e.Nom);
    printf("Prénom : "); scanf(" %[^\n]", e.Prenom);
    printf("Âge : "); scanf("%d", &e.Age);
    printf("Salaire : "); scanf("%d", &e.Salaire);
    printf("Poste : "); scanf(" %[^\n]", e.Poste);

    e.Id = lastGetId1() + 1; // ID auto-incrémenté

    FILE *f = fopen(FICHIER_EMPLOYES, "a");
    if (!f) { perror("Erreur fichier"); return; }
    fprintf(f, "%s,%s,%d,%d,%d,%s\n", e.Nom, e.Prenom, e.Age, e.Salaire, e.Id, e.Poste);
    fclose(f);
    setColor(10); printf("✅ Employé ajouté avec succès. ID attribué : %d\n", e.Id); setColor(7);
}

// 👁️ Affichage des employés
void AfficheFormulaire() {
    if (!adminConnecte) return;
    FILE *f = fopen(FICHIER_EMPLOYES, "r");
    if (!f) { perror("Erreur fichier"); return; }
    employer e;
    setColor(11); printf("=== Liste des Employés ===\n"); setColor(7);
    while (fscanf(f, " %[^,],%[^,],%d,%d,%d,%[^\n]", e.Nom, e.Prenom, &e.Age, &e.Salaire, &e.Id, e.Poste) != EOF) {
        printf("👤 %s %s | 🎂 %d | 💰 %d FCFA | 🆔 %d | 💼 %s\n",
               e.Nom, e.Prenom, e.Age, e.Salaire, e.Id, e.Poste);
    }
    fclose(f);
}

// ✏️ Modifier un employé
void MiseJourEmployer() {
    if (!adminConnecte) return;

    int id, found = 0;
    employer e;

    printf("ID de l'employé à modifier : ");
    scanf("%d", &id);

    FILE *f = fopen(FICHIER_EMPLOYES, "r");
    FILE *temp = fopen("temp.csv", "w");
    if (!f || !temp) { perror("Erreur"); return; }

    while (fscanf(f, " %[^,],%[^,],%d,%d,%d,%[^\n]", e.Nom, e.Prenom, &e.Age, &e.Salaire, &e.Id, e.Poste) != EOF) {
        if (e.Id == id) {
            found = 1;
            setColor(11); printf("=== Modifier l'Employé ===\n"); setColor(7);
            printf("Nouveau nom : "); scanf(" %[^\n]", e.Nom);
            printf("Nouveau prénom : "); scanf(" %[^\n]", e.Prenom);
            printf("Âge : "); scanf("%d", &e.Age);
            printf("Salaire : "); scanf("%d", &e.Salaire);
            printf("Poste : "); scanf(" %[^\n]", e.Poste);
        }
        fprintf(temp, "%s,%s,%d,%d,%d,%s\n", e.Nom, e.Prenom, e.Age, e.Salaire, e.Id, e.Poste);
    }

    fclose(f); fclose(temp);
    remove(FICHIER_EMPLOYES);
    rename("temp.csv", FICHIER_EMPLOYES);

    setColor(found ? 10 : 12);
    printf(found ? "✅ Employé modifié.\n" : "❌ Employé introuvable.\n");
    setColor(7);
}

// 🗑️ Supprimer & archiver
void supprimeEtArchive() {
    if (!adminConnecte) return;
    int id, found = 0;
    employer e;

    printf("ID de l'employé à supprimer : ");
    scanf("%d", &id);

    FILE *f = fopen(FICHIER_EMPLOYES, "r");
    FILE *temp = fopen("temp.csv", "w");
    FILE *arch = fopen(FICHIER_ARCHIVE, "a");
    if (!f || !temp || !arch) { perror("Erreur"); return; }

    while (fscanf(f, " %[^,],%[^,],%d,%d,%d,%[^\n]", e.Nom, e.Prenom, &e.Age, &e.Salaire, &e.Id, e.Poste) != EOF) {
        if (e.Id == id) {
            found = 1;
            fprintf(arch, "%s,%s,%d,%d,%d,%s\n", e.Nom, e.Prenom, e.Age, e.Salaire, e.Id, e.Poste);
        } else {
            fprintf(temp, "%s,%s,%d,%d,%d,%s\n", e.Nom, e.Prenom, e.Age, e.Salaire, e.Id, e.Poste);
        }
    }

    fclose(f); fclose(temp); fclose(arch);
    remove(FICHIER_EMPLOYES);
    rename("temp.csv", FICHIER_EMPLOYES);

    setColor(found ? 10 : 12);
    printf(found ? "✅ Employé supprimé et archivé.\n" : "❌ Employé introuvable.\n");
    setColor(7);
}

// 🔐 Ajouter mot de passe à un employé
void ajouterMotDePasseSiNomExiste() {
    if (!adminConnecte) return;
    char nom[100], mdp[100];
    int found = 0;
    employer e;

    printf("Nom de l'employé : ");
    scanf(" %[^\n]", nom);

    FILE *f = fopen(FICHIER_EMPLOYES, "r");
    FILE *mdpf = fopen(FICHIER_MDP, "a");
    if (!f || !mdpf) { perror("Erreur"); return; }

    while (fscanf(f, " %[^,],%[^,],%d,%d,%d,%[^\n]", e.Nom, e.Prenom, &e.Age, &e.Salaire, &e.Id, e.Poste) != EOF) {
        if (strcmp(e.Nom, nom) == 0) {
            found = 1;
            printf("Mot de passe : ");
            saisirMotDePasseMasque(mdp);
            fprintf(mdpf, "%s,%s\n", nom, mdp);
            setColor(10); printf("✅ Mot de passe ajouté.\n"); setColor(7);
            break;
        }
    }

    if (!found) {
        setColor(12); printf("❌ Employé introuvable.\n"); setColor(7);
    }

    fclose(f); fclose(mdpf);
}

// 🌐 Menu Administrateur avec navigation
void menuAdministrateur() {
    int choix = 0, continuer = 1;
    const char* options[] = {
        "➕ Ajouter un employé\n",
        "✏️ Modifier un employé\n",
        "🗑️ Supprimer un employé\n",
        "👁️ Voir la liste des employés\n",
        "🔐 Ajouter mot de passe\n",
        "🚪 Quitter\n"
    };

    while (continuer) {
        system("cls");
        setColor(11);
        printf("====== MENU ADMINISTRATEUR ======\n");
        setColor(7);
        for (int i = 0; i < 6; i++) {
            if (i == choix) {
                setColor(13); printf("→ %s", options[i]); setColor(7);
            } else {
                printf("  %s", options[i]);
            }
        }

        int key = _getch();
        if (key == 224 || key == 0) {
            key = _getch();
            if (key == 72) choix = (choix == 0) ? 5 : choix - 1;
            else if (key == 80) choix = (choix == 5) ? 0 : choix + 1;
        } else if (key == 13) {
            system("cls");
            switch (choix) {
                case 0: Formulaire(); break;
                case 1: MiseJourEmployer(); break;
                case 2: supprimeEtArchive(); break;
                case 3: AfficheFormulaire(); break;
                case 4: ajouterMotDePasseSiNomExiste(); break;
                case 5: continuer = 0; break;
            }
            if (continuer) {
                setColor(8);
                printf("\nAppuyez sur une touche pour continuer...");
                setColor(7);
                _getch();
                system("cls"); // Nettoie l'écran avant de réafficher le menu
            }
        }
    }
}
