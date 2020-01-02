///////////////////////////////////////////////////////////////////////////////
// Utilisation simplifiée des sorties PWM
//
// IUT de Cachan
// Version 06/2014 pour xc8
//
// Fonctions disponibles
//
//   void pwm_init(unsigned char period, char nb_canaux);
//     Initialisation des sorties PWM.
//     Cette fonction permet d'utiliser 1 ou 2 sorties PWM en fixant nb_canaux
//     et de régler la fréquence de fonctionnement f = (3e6 / (period+1))
//     pour Fosc = 48 MHz
//     
//   void pwm_setdc1(unsigned int cycles_etat_haut);
//     Réglage du rapport cyclique de PWM1 (broche C2)
//     Exemples de valeurs du rapport cyclique en fonction de la valeur
//     choisie pour period avec la fonction pwm_init :
//       cycles_etat_haut - Rapport cyclique
//              0         -      0
//            period      -      0.25
//          2 * period    -      0.5
//          4 * period    -      1
//
//   void pwm_setdc2(unsigned int cycles_etat_haut);
//     Réglage du rapport cyclique de PWM2 (broche C1)
//
//   Broche - Canal PWM
//     C2   -   PWM1
//     C1   -   PWM2
//
// Pour plus d'informations, consultez p18f4550_39632e.pdf §15
///////////////////////////////////////////////////////////////////////////////

#include <xc.h>

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  pwm_init
//  Valeur de retour :  aucune
//  Paramètres       :  unsigned char period
//                        fréquence de fonctionnement f = (3e6 / (period+1))
//                        pour Fosc = 48 MHz
//                        exemple : si period=150, f~20kHz
//                      char nb_canaux
//                        choix du nombre de canaux PWM (1 ou 2)
//                        1 -> seule la broche C2 est une sortie PWM
//                        2 -> les broches C2 et C1 sont des sorties PWM
//  Description      :  configuration de la fréquence et du nombre de canaux
///////////////////////////////////////////////////////////////////////////////
void pwm_init(unsigned char period, char nb_canaux);

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  pwm_setdc1
//  Valeur de retour :  aucune
//  Paramètres       :  unsigned int cycles_etat_haut
//                        nombre de cycles à l'état haut codé sur 10 bits
//                          1 cycle de period <=> 4 cycles de cycles_etat_haut
//                          rapport cyclique ~ cycles_etat_haut / (4 * period)
//                        exemple : si period=150, f~20kHz
//                            cycles_etat_haut - Rapport cyclique
//                                   0         -      0
//                                 150         -      0.25
//                                 300         -      0.5
//                                 600         -      1
//  Description      :  réglage du rapport cyclique du canal PWM1 (broche C2)
///////////////////////////////////////////////////////////////////////////////
void pwm_setdc1(unsigned int cycles_etat_haut);

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  pwm_setdc2
//  Valeur de retour :  aucune
//  Paramètres       :  unsigned int cycles_etat_haut
//                        nombre de cycles à l'état haut codé sur 10 bits
//                          1 cycle de period <=> 4 cycles de cycles_etat_haut
//                          rapport cyclique ~ cycles_etat_haut / (4 * period)
//  Description      :  réglage du rapport cyclique du canal PWM2 (broche C1)
///////////////////////////////////////////////////////////////////////////////
void pwm_setdc2(unsigned int cycles_etat_haut);
