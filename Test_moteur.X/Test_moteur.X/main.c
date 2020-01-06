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
    double moteurG, moteurD;
    double vitesseMax;
    int etat=1;
    
    TRISEbits.TRISE1 = 1; // Jack sur RE1
    TRISEbits.TRISE2 = 1; // fin de course sur RE2
    
    
    while(1)
    {
        n1=adc_read(1); //AN1 pin 4
        n2=adc_read(3); //AN3 pin 6
        n3=adc_read(4); //AN4 pin 10
        n4=adc_read(5); //AN5 pin 12
        
        debutDeCourse = PORTEbits.RE1; //lecture sur RE2, pin 11 (Jack)
        finDeCourse = PORTEbits.RE2; //lecture Port E pin 14 (FDC)
        
        vitesseMax = adc_read(0)/1023.0*600.0;
        
        switch (etat)
        {
            case 1:
                if (debutDeCourse == 1)
                {
                pwm_init (150,2); // initialiser 2 moteurs pour une fréquence de 2kHz
                etat = 2;
                }
                break;
            case 2:
                moteurG = vitesseMax * (1023-(double)n3)/1024;
                moteurD = vitesseMax * (1023-(double)n2)/1024;
                if (finDeCourse == 0) etat = 3;
                break;
            case 3:
                moteurG = 0;
                moteurD = 0;
                break;
            case 4:
                CloseTimer0();
                pwm_setdc1(0);
                pwm_setdc2(180);
                if ((n2>400) &&  (n3>400)) etat = 2;
                break;
                
        }

        pwm_setdc1((int)moteurG);
        pwm_setdc2((int)moteurD);
        
        // affiche les données des capteurs
        lcd_position(0,0);
        lcd_printf("%3d %3d %3d %3d",n1,n2,n3,n4); // Capteurs
        lcd_position(1,0);
        lcd_printf("%4d",(int) moteurG); // moteur gauche
        lcd_position(1,5);
        lcd_printf("%4d",(int) vitesseMax); // moteur droit
        lcd_position(1,10);
        lcd_printf("%4d",(int) moteurD); // moteur droit
    }
    return;   
}