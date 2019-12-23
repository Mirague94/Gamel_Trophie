///////////////////////////////////////////////////////////////////////////////
// Utilisation simplifiée du convertisseur analogique-numérique (ADC)
//
// IUT de Cachan
// Version 06/2014 pour xc8
//
// Fonctions disponibles
//
//   void adc_init(char numero_dernier_canal);
//     Initialisation de l'ADC en précisant le numéro du dernier canal
//     analogique utilisé
//       adc_init(0); // active un seul canal : AN0.
//       adc_init(3); // active 4 canaux : AN0, AN1, AN2 et AN3
//     Cette fonction permet d'utiliser au maximum 8 canaux : adc_init(7);
//
//   int adc_read(char numero_canal);
//     Lecture de la valeur numérisée d'un canal analogique
//     Broches configurées en analogique par adc_init
//     Temps de conversion : environ 25 us
//
//   Broche - Canal analogique
//     A0   -   AN0
//     A1   -   AN1
//     A2   -   AN2
//     A3   -   AN3
//     A5   -   AN4
//     E0   -   AN5
//     E1   -   AN6
//     E2   -   AN7
//
// Pour plus d'informations, consultez p18f4550_39632e.pdf §21
///////////////////////////////////////////////////////////////////////////////

#include <xc.h>

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  adc_init
//  Valeur de retour :  aucune
//  Paramètres       :  char numero_dernier_canal
//                        valeur entière de 0 à 7, numéro du dernier canal
//                        configuré en entrée analogique
//  Description      :  configuration de l'ADC sur la plage 0-5V
//                      et choix du dernier canal analogique
//                      0 indique seul AN0 en analogique
//                      7 indique AN0, AN1,...,AN7 en analogiques
///////////////////////////////////////////////////////////////////////////////
void adc_init(char numero_dernier_canal);

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  adc_read
//  Valeur de retour :  int  =>  valeur lue sur 10 bits
//  Paramètres       :  char numero_canal
//                        valeur entière de 0 à 7, numéro du canal à convertir
//  Description      :  sélection d'un canal analogique
//                      attente de la fin de la conversion
//                      renvoi de la valeur lue sur 10 bits
//                      durée de la conversion, environ 25us
///////////////////////////////////////////////////////////////////////////////
int adc_read(char numero_canal);
