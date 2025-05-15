#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>
#include <windows.h>
#include <unistd.h>
#include <string.h> // Ajout pour strcmp

#include "GestioClient.h"
#include "GestioPers.h"
#include "EMPLOYER.h"
//#include "UTILS.h"

// Fonction pour changer la couleur du texte dans la console
void setColor1(int colorCode) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, colorCode);
}

// Fonction pour afficher le menu principal avec la sélection
void afficherMenuPrincipal(int selected) {
    system("cls");
    setColor1(11);
    printf("==========================================\n");
    setColor1(14);
    printf("   🌟 BIENVENUE À L'HÔTEL 5 ÉTOILES 🌟\n");
    setColor1(11);
    printf("==========================================\n");
    setColor1(10);
    printf("====== MENU PRINCIPAL ======\n");
    setColor1(7);

    // Instructions
    setColor1(8);
    printf("Utilisez les flèches ↑ ↓ pour naviguer et Entrée pour valider.\n");
    setColor1(7);

    // Options du menu
    if (selected == 0)
        setColor1(10);
    else
        setColor1(7);
    printf("%s Mode Client 👤\n", (selected == 0) ? ">>" : "  ");

    if (selected == 1)
        setColor1(14);
    else
        setColor1(7);
    printf("%s Mode Administrateur 🔐\n", (selected == 1) ? ">>" : "  ");

    if (selected == 2)
        setColor1(9);
    else
        setColor1(7);
    printf("%s Mode Employé 👨‍💼\n", (selected == 2) ? ">>" : "  ");

    if (selected == 3)
        setColor1(12);
    else
        setColor1(7);
    printf("%s Quitter ❌\n", (selected == 3) ? ">>" : "  ");

    /*setColor1(7);
    printf("\nAppuyez sur Entrée pour confirmer votre choix.\n");
    */
}

// Fonction pour la connexion administrateur
int adminConnexion() {
    char motDePasse[50];
    const char motDePasseCorrect[] = "000000"; // Mot de passe par défaut : six zéros

    setColor1(14);
    printf("🔐 Entrez le mot de passe administrateur : ");
    setColor1(7);
    scanf("%s", motDePasse);

    if (strcmp(motDePasse, motDePasseCorrect) == 0) {
        setColor1(10);
        printf("✅ Connexion administrateur réussie !\n");
        setColor1(7);
        sleep(1);
        return 1;
    } else {
        setColor1(12);
        printf("❌ Mot de passe incorrect.\n");
        setColor1(7);
        sleep(1);
        return 0;
    }
}

// Fonction principale de menu
void menuPrincipal() {
    int choix = 0; // index de la sélection
    int nbOptions = 4; // nombre d'options
    int continuer = 1;

    while (continuer) {
        afficherMenuPrincipal(choix);

        int ch = getch();

        if (ch == 224) { // touche fléchée
            ch = getch();
            switch (ch) {
                case 72: // Flèche haut
                    choix = (choix - 1 + nbOptions) % nbOptions;
                    break;
                case 80: // Flèche bas
                    choix = (choix + 1) % nbOptions;
                    break;
            }
        } else if (ch == 13) { // Entrée
            switch (choix) {
                case 0:
                    MenuClient();
                    break;
                case 1:
                    if (adminConnexion()) {
                        menuAdministrateur();
                    } else {
                        setColor1(12);
                        printf("❌ Échec de connexion administrateur.\n");
                        setColor1(7);
                        sleep(1);
                    }
                    break;
                case 2:
                    connexionDirigerEmployer();
                    break;
                case 3:
                    setColor1(10);
                    printf("Merci pour votre visite à l'Hôtel 5 Étoiles. À bientôt !\n");
                    setColor1(7);
                    sleep(1);
                    continuer = 0;
                    break;
            }
        }
    }
}

int main() {
    // Activation UTF-8
    SetConsoleOutputCP(CP_UTF8);
    //setlocale(LC_ALL, "");

    // Définir la console pour supporter les caractères Unicode (pour emojis)
    // Assurez-vous que la police de la console supporte Unicode
    // Par exemple, changer la police manuellement ou dans le code si possible

    system("cls");
    setColor1(11);
    printf("==========================================\n");
    setColor1(14);
    printf("   🌟 BIENVENUE À L'HÔTEL 5 ÉTOILES 🌟\n");
    setColor1(11);
    printf("==========================================\n");
    setColor1(7);

    menuPrincipal();

    return 0;
}
