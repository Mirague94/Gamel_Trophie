///////////////////////////////////////////////////////////////////////////////
// Utilisation simplifiée d'un écran LCD à 2 lignes et 16 colonnes
//
// IUT de Cachan
// Version 11/2015 pour xc8
// Source inspiré des bibliothèques de Microchip MPLAB C18
//
// Fonctions disponibles
//
//   void lcd_init(void);
//     Initialisation de l'écran LCD.
//     Cette fonction configure l'écran LCD.
//     
//   void lcd_clear(void);
//     Efface l'écran.
//
//   void lcd_position(char ligne, char colonne);
//     Positionne le curseur pour l'affichage.
//     Le coin supérieur gauche est en (0,0).
//       colonne 0                    colonne 15
//               |                             |
//     ligne 0 - X X X X X X X X X X X X X X X X
//
//     ligne 1 - X X X X X X X X X X X X X X X X
//
//   void lcd_putc(unsigned char lettre);
//     Affiche un caractère à la position du curseur.
//     Le curseur se décale d'une case vers la droite.
//     Les caractères spéciaux sont interprétés (voir ci-dessous).
//
//   int lcd_printf(const char *f, ...);
//     Fonction printf pour l'écran LCD.
//     Pour une description complète des formats autorisés,
//     voir MPLAB_C18_Libraries_51297f.pdf §4.7
//
// Caractères spéciaux interprétés :
//   \n  passe à la ligne (ne fonctionne que de la ligne 0 à la ligne 1)
//   \b  pour reculer le curseur d'une case
//   \f  pour effacer l'écran
//
// Connexions :
//   Broche - Ecran LCD
//     D0   -    E
//     D1   -    RS
//     D2   -    RW
//     D3   -    non connectée
//   D4-D7  -    Data[4..7]
//
///////////////////////////////////////////////////////////////////////////////

#include <xc.h>

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_init
//  Valeur de retour :  aucune
//  Paramètres       :  aucun
//  Description      :  basée sur le coontroleur d'afficheur Hitachi HD44780
//                      cette fonction initialise l'écran LCD
///////////////////////////////////////////////////////////////////////////////
void lcd_init(void);

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_clear
//  Valeur de retour :  aucune
//  Paramètres       :  aucun
//  Description      :  efface l'écran LCD
///////////////////////////////////////////////////////////////////////////////
void lcd_clear(void);

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_position
//  Valeur de retour :  aucune
//  Paramètres       :  char ligne
//                        numéro de la ligne (0 ou 1)
//                      char colonne
//                        numéro de la colonne (de 0 à 15)
//  Description      :  place le curseur sur la case (ligne, colonne)
///////////////////////////////////////////////////////////////////////////////
void lcd_position(char ligne, char colonne);

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_putc
//  Valeur de retour :  aucune
//  Paramètres       :  unsigned char lettre
//                        code ASCII du caractère à afficher
//  Description      :  affiche un caractère sur l'écran en gérant les
//                      caractères spéciaux \n \f \b
///////////////////////////////////////////////////////////////////////////////
void lcd_putc(unsigned char lettre);

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_printf
//  Valeur de retour :  aucune
//  Paramètres       :  const char *f
//                        chaine de format
//                        pour une description complète des formats autorisés,
//                        voir MPLAB_C18_Libraries_51297f.pdf §4.7
//                        le format float (%f) est partiellement implémenté
//                      ...
//                        variables à convertir
//  Description      :  affiche un message sur l'écran LCD
///////////////////////////////////////////////////////////////////////////////
void lcd_printf(const char *f, ...);


///////////////////////////////////////////////////////////////////////////////
//
// Obsolètes
// Fournies pour compatibilité avec la version précédente de la bibliothèque.
//
///////////////////////////////////////////////////////////////////////////////
void lcd_gotoyx(char ligne, char colonne);
void lcd_puti(int nombre);
void lcd_putrs(const char *chaine);
void lcd_puts(char *chaine);

