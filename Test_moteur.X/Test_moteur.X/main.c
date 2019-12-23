/*
 * File:   main.c
 * Author: mviaud
 *
 * Created on 02 december 2019, 14:53
 */

#include <xc.h>
#include "iut_adc.h"
#include "iut_lcd.h"
#include "iut_pwm.h"
#include "iut_timers.h"

void main(void) {
    
    int n1,n2,n3,n4;
    adc_init(5); //initialiser de AN0 à AN5
    lcd_init(); //initialiser le lcd
    int finDeCourse=1, debutDeCourse;
    int etat=0;
    int Vitesse=1.5;
    
    TRISEbits.TRISE1 = 1; // Jack sur RE1
    TRISEbits.TRISE2 = 1; // fin de course sur RE2
    
    
    while(1)
    {
        n1=adc_read(1); //AN1 pin 4
        n2=adc_read(3); //AN3 pin 6
        n3=adc_read(4); //AN4 pin 10
        n4=adc_read(5); //AN5 pin 12
        
        debutDeCourse = PORTEbits.RE1; //lecture sur RE2, pin 11 (Jack)
        finDeCourse = PORTEbits.RE2 ; //lecture Port E pin 14 (FDC)
        
        if (debutDeCourse == 1)
        {
            pwm_init (150,2); // initialiser 2 moteurs pour une fréquence de 2kHz
        }
        
        if (finDeCourse == 0)
        {
            etat=2;
        }
        
        if ((n1<200)&& (n4>200))
        {
            OpenTimer0( TIMER_INT_ON &
                        T0_16BIT &
                        T0_SOURCE_INT &
                        T0_PS_1_256);
        }
        /*
         * if ((n1<200) && (n4<200))
        {
            CloseTimer0();
        }*/
        
        switch (etat)
        {
            case 0:
                pwm_setdc1(150*Vitesse);
                pwm_setdc2(150*Vitesse);
                if (ReadTimer0()>37500) etat=31;
                if ((n3 > 800) && (n2 > 800)) etat=2;
                else
                {
                   if (n3 > 200) etat=21;
                   if (n2 > 200) etat=11;
                }
                break;
            case 2 :
                pwm_setdc1(000*Vitesse);
                pwm_setdc2(000*Vitesse);
                
                // tourner à droite
            case 11:
                pwm_setdc1(150*Vitesse); //moteur gauche avance à 25%
                pwm_setdc2(120*Vitesse); //moteur droit au ralenti
                if (n2 > 300) etat=12;
                else if (n2 < 200) etat=0;
                break;
            case 12:
                pwm_setdc1(150*Vitesse); //moteur gauche avance à 25%
                pwm_setdc2(90*Vitesse); //moteur droit au ralenti
                if (n2 > 400) etat=13;
                else if (n2 < 300) etat=11;
                break;
            case 13:
                pwm_setdc1(150*Vitesse); //moteur gauche avance à 25%
                pwm_setdc2(60*Vitesse); //moteur droit au ralenti
                if (n2 > 500) etat=14;
                else if (n2 < 400) etat=12;
                break;
            case 14:
                pwm_setdc1(150*Vitesse); //moteur gauche avance à 25%
                pwm_setdc2(30*Vitesse); //moteur droit au ralenti
                if (n2 > 600) etat=15;
                else if (n2 < 500) etat=13;
                break;
            case 15:
                pwm_setdc1(150*Vitesse); //moteur gauche avance à 25%
                pwm_setdc2(00*Vitesse); //moteur droit à l'arret
                if (n2 < 600) etat=14;
                break;

                // tourner à gauche
            case 21:
                pwm_setdc1(120*Vitesse); //moteur gauche au ralenti
                pwm_setdc2(150*Vitesse); //moteur droit avance à 25%
                if (n3 > 300) etat=22;
                else if (n3 < 200) etat=0;
                break;
            case 22:
                pwm_setdc1(90*Vitesse); //moteur gauche au ralenti
                pwm_setdc2(150*Vitesse); //moteur droit avance à 25%
                if (n3 > 400) etat=23;
                else if (n3 < 300) etat=21;
                break;
            case 23:
                pwm_setdc1(60*Vitesse); //moteur gauche au ralenti
                pwm_setdc2(150*Vitesse); //moteur droit avance à 25%
                if (n3 > 500) etat=24;
                else if (n3 < 400) etat=22;
                break;
            case 24:
                pwm_setdc1(30*Vitesse); //moteur gauche au ralenti
                pwm_setdc2(150*Vitesse); //moteur droit avance à 25%
                if (n3 > 600) etat=25;
                else if (n3 < 500) etat=23;
                break;
            case 25:
                pwm_setdc1(000*Vitesse); //moteur gauche l'arret
                pwm_setdc2(150*Vitesse); //moteur droit avance à 25%
                if (n3 < 600) etat=24;
                break;
                
                //raccourci
            case 31 :
                pwm_setdc1(000*Vitesse);
                pwm_setdc2(000*Vitesse);
         }

        // affiche les données des capteurs
        
        lcd_position(0,0);
        lcd_printf("%3d %3d %3d %3d",n1,n2,n3,n4); // Capteur Centre Gauche
        lcd_position(1,0);
        lcd_printf("FDC%d",finDeCourse); // fin de course
        lcd_position(1,6);
        lcd_printf("%2d",etat); // fin de course
        lcd_position(1,9);
        lcd_printf("Jack%d",debutDeCourse); // Jack
         
    }
    return;
}
