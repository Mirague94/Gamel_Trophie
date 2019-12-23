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
//   int adc_read(char numero_channel);
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

#include "iut_adc.h"

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

void adc_init(char numero_dernier_canal) {
    // Configuration des canaux analogiques
    // et mise en entrées des broches associées
    switch (numero_dernier_canal) {
        case 0:
            ADCON1 = 0xE;
            TRISA = TRISA | 0b00000001;
            break;
        case 1:
            ADCON1 = 0xD;
            TRISA = TRISA | 0b00000011;
            break;
        case 2:
            ADCON1 = 0xC;
            TRISA = TRISA | 0b00000111;
            break;
        case 3:
            ADCON1 = 0xB;
            TRISA = TRISA | 0b00001111;
            break;
        case 4:
            ADCON1 = 0xA;
            TRISA = TRISA | 0b00101111;
            break;
        case 5:
            ADCON1 = 0x9;
            TRISA = TRISA | 0b00101111;
            TRISE = TRISE | 0b00000001;
            break;
        case 6:
            ADCON1 = 0x8;
            TRISA = TRISA | 0b00101111;
            TRISE = TRISE | 0b00000011;
            break;
        case 7:
            ADCON1 = 0x7;
            TRISA = TRISA | 0b00101111;
            TRISE = TRISE | 0b00000111;
            break;
        default:
            ADCON1 = 0xE;
            TRISA = TRISA | 0b00000001;
            break;
    }

    // Configuration de la fréquence de conversion (Fosc/64),
    // du temps d'acquisition de 8 periodes de conversion (Rmax ~ 50k)
    // et de la justification à droite du résultat sur 10 bits.
    ADCON2 = 0b10100110;

    // Active le convertisseur analogique-numérique
    ADCON0bits.ADON = 1;
}

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

int adc_read(char numero_canal) {
    // Sélection du canal à convertir
    ADCON0 = ((numero_canal & 0x07) << 2) | 0x01;

    // Début de la conversion
    ADCON0bits.GO = 1;

    // Attente de la fin de la conversion
    while (ADCON0bits.GO);

    // Renvoi du résultat de la conversion
    return (((unsigned int) ADRESH) << 8) | ADRESL;
}
