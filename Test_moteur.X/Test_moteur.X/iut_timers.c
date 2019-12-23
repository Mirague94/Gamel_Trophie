#include "iut_timers.h"

///////////////////////////////////////////////////////////////////////////////
// Utilisation simplifiée des timers
// Bibliothèque basée sur l'ancien compilateur MCC18
//
// IUT de Cachan
// Version 10/2015 pour xc8
//
///////////////////////////////////////////////////////////////////////////////

/********************************************************************
*    Function Name:  OpenTimer0                                     *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure Timer0    *
*    Description:    This routine resets the Timer0 regs to the     *
*                    POR state and configures the interrupt, clock  *
*                    source, edge and prescaler.                    *
*    Notes:          The bit definitions for config can be found    *
*                    in the timers.h file.                          *
********************************************************************/
void OpenTimer0(unsigned char config)
{
  T0CON = (0x7f & config);  // Configure timer, but don't start it yet
  TMR0H = 0;                // Reset Timer0 to 0x0000
  TMR0L = 0;
  INTCONbits.T0IF = 0;      // Clear Timer0 overflow flag

  if(config&0x80)           // If interrupts enabled
    INTCONbits.T0IE = 1;    // Enable Timer0 overflow interrupt
  else
    INTCONbits.T0IE = 0;

  T0CONbits.TMR0ON = 1;
}

/********************************************************************
*    Function Name:  ReadTimer0                                     *
*    Return Value:   int: Timer0 16-bit value                       *
*    Parameters:     void                                           *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer0.                                        *
********************************************************************/
unsigned int ReadTimer0(void)
{
  union Timers timer;

  timer.bt[0] = TMR0L;  // Copy Timer0 low byte into union
  timer.bt[1] = TMR0H;  // Copy Timer0 high byte into union

  return (timer.lt);    // Return the int
}

/********************************************************************
*    Function Name:  WriteTimer0                                    *
*    Return Value:   void                                           *
*    Parameters:     int: value to write to Timer0                  *
*    Description:    This routine writes a 16-bit value to Timer0   *
*                    Timer0.                                        *
********************************************************************/
void WriteTimer0(unsigned int timer0)
{
  union Timers timer;

  timer.lt = timer0;    // Copy timer value into union

  TMR0H = timer.bt[1];  // Write high byte to Timer0
  TMR0L = timer.bt[0];  // Write low byte to Timer0
}

/********************************************************************
*    Function Name:  CloseTimer0                                    *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine disables the Timer0 interrupt.    *
********************************************************************/
void CloseTimer0(void)
{
  INTCONbits.TMR0IE = 0;  // Disable the Timer0 interrupt
  T0CON=0x7F;             //Stop TMR0
}

/********************************************************************
*    Function Name:  OpenTimer1                                     *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure Timer1    *
*    Description:    This routine first resets the Timer1 regs      *
*                    to the POR state and then configures the       *
*                    interrupt, clock source and 8/16-bit mode.     *
*    Notes:          The bit definitions for config can be found    *
*                    in the timers.h file.                          *
********************************************************************/
void OpenTimer1(unsigned char config)
{
  T1CON = (0x7e & config);  // Set everything except 8/16 mode, and
                            // don't start timer yet

  if( config & 0x40 )       // The 8/16 selection bit isn't in the
      T1CONbits.RD16 = 1;   // right place -- we have to move it
  else
      T1CONbits.RD16 = 0;

  TMR1H=0;             // Clear out timer registers
  TMR1L=0;
  PIR1bits.TMR1IF=0;

  if(config&0x80)      // Enable interrupts if selected
    PIE1bits.TMR1IE=1;
  else
    PIE1bits.TMR1IE=0;
  
  T1CONbits.TMR1ON = 1;  // Start Timer1
}

/********************************************************************
*    Function Name:  ReadTimer1                                     *
*    Return Value:   char: Timer1 16-bit value                      *
*    Parameters:     void                                           *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer1.                                        *
********************************************************************/
unsigned int ReadTimer1(void)
{
  union Timers timer;

  timer.bt[0] = TMR1L;    // Read Lower byte
  timer.bt[1] = TMR1H;    // Read upper byte

  return (timer.lt);      // Return the 16-bit value
}

/********************************************************************
*    Function Name:  WriteTimer1                                    *
*    Return Value:   void                                           *
*    Parameters:     int: value to write to Timer1                  *
*    Description:    This routine writes a 16-bit value to Timer1.  *
********************************************************************/
void WriteTimer1(unsigned int timer1)
{
  union Timers timer;

  timer.lt = timer1;    // Save the 16-bit value in local

  TMR1H = timer.bt[1];  // Write high byte to Timer1 High byte
  TMR1L = timer.bt[0];  // Write low byte to Timer1 Low byte
}

/********************************************************************
*    Function Name:  CloseTimer1                                    *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine disables Timer1 and interrupt.    *
********************************************************************/
void CloseTimer1(void)
{
  T1CONbits.TMR1ON = 0;  // Disable Timer1
  PIE1bits.TMR1IE = 0;   // Disable Timer1 overflow interrupts
}

/********************************************************************
*    Function Name:  OpenTimer2                                     *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure Timer2    *
*    Description:    This routine first resets the Timer2 regs      *
*                    to the POR state and then configures the       *
*                    interrupt and clock source.                    *
*    Notes:          The bit definitions for config can be found    *
*                    in the timers.h file.                          *
********************************************************************/
void OpenTimer2(unsigned char config)
{

  T2CON = (0xfb & config);  // Set all configuration values, but
                            // don't start timer yet

  TMR2 = 0;                 // Clear Timer2
  PIR1bits.TMR2IF = 0;

  if(config & 0x80)         // Enable timer interrupts?
    PIE1bits.TMR2IE = 1;
  else
    PIE1bits.TMR2IE = 0;

  T2CONbits.TMR2ON = 1; // Turn on Timer2
}

/********************************************************************
*    Function Name:  ReadTimer2                                     *
*    Return Value:   char: Timer2 8-bit value                       *
*    Parameters:     void                                           *
*    Description:    This routine reads the 8-bit value from        *
*                    Timer2.                                        *
********************************************************************/
unsigned char ReadTimer2(void)
{
  unsigned char timer;

  timer = TMR2;      // Read Timer2

  return (timer);    // Return 8-bits
}

/********************************************************************
*    Function Name:  WriteTimer2                                    *
*    Return Value:   void                                           *
*    Parameters:     char: value to write to Timer2                 *
*    Description:    This routine writes a 8-bit value to Timer2.   *
********************************************************************/
#undef WriteTimer2
void WriteTimer2(unsigned char timer2)
{
  TMR2 = timer2;    // Write byte to Timer2
}

/********************************************************************
*    Function Name:  CloseTimer2                                    *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine disables the Timer2 and the       *
*                    interrupt.                                     *
********************************************************************/
void CloseTimer2(void)
{
  T2CONbits.TMR2ON = 0;  // Disable Timer2
  PIE1bits.TMR2IE = 0;   // Disable Timer2 overflow interrupts
}

/********************************************************************
*    Function Name:  OpenTimer3                                     *
*    Return Value:   void                                           *
*    Parameters:     config: bit definitions to configure Timer3    *
*    Description:    This routine first resets the Timer3 regs      *
*                    to the POR state and then configures the       *
*                    interrupt, clock source.                       *
*    Notes:          The bit definitions for config can be found    *
*                    in the timers.h file.                          *
*                                                                   *
*                    The MPLAB CXX Reference Guide says that        *
*                    OpenTimer1(config) can select Timer 1 as the   *
*                    source for the CCPs.  However, this function   *
*                    does not write the necessary values to bits 6  *
*                    and 3 of T3CON.  Instead, the function         *
*                    incorrectly affects bit 3 of T1CON.            *
*                                                                   *
*                    Because the power-on default is timer1 source  *
*                    for both CCPs, we do not need to even have     *
*                    these values.  Should a user want to change    *
*                    this value, the user would need select the     *
*                    appropriate value when calling OpenTimer3.     *
*                                                                   *
********************************************************************/
void OpenTimer3(unsigned char config)
{
  T3CON = (0x7e & config);  // Set everything except 8/16 mode, and
                            // don't start timer yet

  TMR3H=0;          //Clear timer3-related registers
  TMR3L=0;
  PIR2bits.TMR3IF = 0;

  if(config & 0x80)   // Interrupt on/off
    PIE2bits.TMR3IE = 1;
  else
    PIE2bits.TMR3IE = 0;

  if(config & 0x02)
    T1CONbits.T1OSCEN = 1;  // Enable Timer 1 Oscillator enable
                            // (200KHz max freq)

  if(config & ~T3_8BIT_RW) // Select between 8-bit and 16-bit modes
  {
    T3CONbits.RD16 = 1;
  }
  else
  {
    T3CONbits.RD16 = 0;
  }

  T3CONbits.TMR3ON = 1;   // Turn on Timer3
}

/********************************************************************
*    Function Name:  ReadTimer3                                     *
*    Return Value:   int: Timer3 16-bit value                       *
*    Parameters:     void                                           *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer3.                                        *
********************************************************************/
unsigned int ReadTimer3(void)
{
  union Timers timer;

  timer.bt[0] = TMR3L;   // Read low byte of Timer3
  timer.bt[1] = TMR3H;   // Read high byte of Timer3

  return (timer.lt);     // Return 16-bit value
}

/********************************************************************
*    Function Name:  WriteTimer3                                    *
*    Return Value:   void                                           *
*    Parameters:     int: value to write to Timer3                  *
*    Description:    This routine writes a 16-bit value to Timer3   *
********************************************************************/
void WriteTimer3(unsigned int timer3)
{
  union Timers timer;

  timer.lt = timer3;    // Save value in local variable
  TMR3H = timer.bt[1];  // Write high byte to Timer3
  TMR3L = timer.bt[0];  // Write low byte to Timer3
}

/********************************************************************
*    Function Name:  CloseTimer3                                    *
*    Return Value:   void                                           *
*    Parameters:     void                                           *
*    Description:    This routine disables the Timer3 and the       *
*                    interrupt.                                     *
********************************************************************/
void CloseTimer3(void)
{
  T3CONbits.TMR3ON = 0;  // Disable Timer3
  PIE2bits.TMR3IE = 0;   // Disable Timer3 overflow interrupts
}
