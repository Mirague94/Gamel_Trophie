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
    int vitesseMax = 180;
    int timer;
    
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
        
        if (debutDeCourse == 1)
        {
            pwm_init (150,2); // initialiser 2 moteurs pour une fréquence de 2kHz
        }
        
        if ((n1<200)&& (n4>200))
        {
            moteurG = 0;
            moteurD = 0;
            lcd_position(1,0);
            lcd_printf("MG %4d",(int) moteurG); // moteur gauche
            lcd_position(1,8);
            lcd_printf("MD %4d",(int) moteurD); // moteur droit
            
            break;

            /*OpenTimer0( TIMER_INT_ON &
                        T0_16BIT &
                        T0_SOURCE_INT &
                        T0_PS_1_256);*/
        }
        
        
        /*while ((ReadTimer0()>3750) || ((n2>400) &&  (n3>400))) ;
        {
            CloseTimer0();
            timer=ReadTimer0();
            n2=adc_read(3); //AN3 pin 6
            n3=adc_read(4); //AN4 pin 10
            pwm_setdc1(0);
            pwm_setdc2(180);
            
            lcd_position(0,0);
            lcd_printf("%3d %7d %3d",n1,timer,n4); // Capteur
            lcd_position(1,0);
            lcd_printf("MG %4d",(int) moteurG); // moteur gauche
            lcd_position(1,8);
            lcd_printf("MD %4d",(int) moteurD); // moteur droit
        }*/
        
        moteurG = vitesseMax * (1023-(double)n3)/1024;
        moteurD = vitesseMax * (1023-(double)n2)/1024;
        
        pwm_setdc1(moteurG);
        pwm_setdc2(moteurD);
        
        // affiche les données des capteurs
        lcd_position(0,0);
        lcd_printf("%3d %7d %3d",n1,timer,n4); // Capteur
        lcd_position(1,0);
        lcd_printf("MG %4d",(int) moteurG); // moteur gauche
        lcd_position(1,8);
        lcd_printf("MD %4d",(int) moteurD); // moteur droit
        
        if (finDeCourse == 0)
        {
            moteurG = 0;
            moteurD = 0;
            lcd_position(1,0);
            lcd_printf("MG %4d",(int) moteurG); // moteur gauche
            lcd_position(1,8);
            lcd_printf("MD %4d",(int) moteurD); // moteur droit
            
            break;
        } 
    }
    return;   
}

