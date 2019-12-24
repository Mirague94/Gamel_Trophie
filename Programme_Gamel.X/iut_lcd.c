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

#include "iut_lcd.h"
#include <stdarg.h>


#ifdef _OMNI_CODE_
#pragma warning push
#pragma warning disable 1496 // disable warnings when using va_arg and va_start arithmetic 
#endif

// Port de donnée de l'afficheur LCD (4 bits)
#define LCD_DATA_PORT         PORTD
#define LCD_TRIS_DATA_PORT    TRISD

// Bits de controle de l'afficheur LCD
#define LCD_RW_PIN   PORTDbits.RD2   // bit LCD_RW
#define LCD_RS_PIN   PORTDbits.RD1   // bit LCD_RS
#define LCD_E_PIN    PORTDbits.RD0   // bit LCD_E

///////////////////////////////////////////////////////////////////////////////
// Déclarations des fonctions internes à la bibliothèque
///////////////////////////////////////////////////////////////////////////////
// Ecriture d'un octet de données sur le LCD avec attente préalable
static void lcd_write_data_busy(unsigned char c);
// Ecriture d'un octet de commande sur le LCD avec attente préalable
static void lcd_write_cmd_busy(unsigned char c);
// Retourne 1 si le LCD est occupé et 0 sinon
static unsigned char lcd_busy(void);
// Un coup d'horloge sur E
static void lcd_clock_e(void);
// Répète n fois l'écriture du caractère c
static void lcd_repete_n(unsigned char n, char c);

///////////////////////////////////////////////////////////////////////////////
// Temporisations nécessaires pour les échanges
// entre le microcontroleur et l'afficheur
// Utilisation de la bibliothèque <delays.h>
///////////////////////////////////////////////////////////////////////////////
#define lcd_delai_250n()       Nop(); Nop(); Nop();
#define lcd_delai_15ms()       _delay(180000)
#define lcd_delai_5ms()        _delay(60000)
#define lcd_delai_100us()      _delay(1200)

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_init
//  Valeur de retour :  aucune
//  Paramètres       :  aucun
//  Description      :  basée sur le coontroleur d'afficheur Hitachi HD44780
//                      cette fonction initialise l'écran LCD
///////////////////////////////////////////////////////////////////////////////

void lcd_init() {
    TRISD &= ~0x07; // Signaux de contrôle en sortie
    TRISD |= 0xF0; // et signaux de données en entrées
    LCD_DATA_PORT &= 0x08; // Tous les signaux à 0
    lcd_delai_15ms(); // 15ms pour permettre le reset du LCD

    LCD_TRIS_DATA_PORT &= 0x0f; // Port données en écriture

    LCD_DATA_PORT &= 0x0f; // Masque pour effacer le port de données
    LCD_DATA_PORT |= 0b00110000; // Masque pour écrire 0011 sur le port de données
    lcd_clock_e();
    lcd_delai_5ms(); // Delai d'au moins 4.1ms

    LCD_DATA_PORT &= 0x0f;
    LCD_DATA_PORT |= 0b00110000;
    lcd_clock_e();
    lcd_delai_100us(); // Delai d'au moins 100us

    LCD_DATA_PORT &= 0x0f;
    LCD_DATA_PORT |= 0b00110000;
    lcd_clock_e();
    lcd_delai_100us(); // Delai d'au moins 37us

    LCD_DATA_PORT &= 0x0f; // 4-bit interface
    LCD_DATA_PORT |= 0b00100000;
    lcd_clock_e();
    lcd_delai_100us(); // Delai d'au moins 37us

    LCD_TRIS_DATA_PORT |= 0xf0; // Port de données en entrée

    lcd_write_cmd_busy(0x28); // Type du LCD 4 bits, 2 lignes, 5x8 pts
    lcd_write_cmd_busy(0x08); // Afficheur OFF
    lcd_clear(); // Efface l'écran
    lcd_write_cmd_busy(0x06); // Incrémentation du curseur automatique
    lcd_write_cmd_busy(0x0C); // Afficheur ON
}

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_clear
//  Valeur de retour :  aucune
//  Paramètres       :  aucun
//  Description      :  efface l'écran LCD
///////////////////////////////////////////////////////////////////////////////

void lcd_clear(void) {
    lcd_write_cmd_busy(0x01);
}

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_position
//  Valeur de retour :  aucune
//  Paramètres       :  char ligne
//                        numéro de la ligne (0 ou 1)
//                      char colonne
//                        numéro de la colonne (de 0 à 15)
//  Description      :  place le curseur sur la case (ligne, colonne)
///////////////////////////////////////////////////////////////////////////////

void lcd_position(char ligne, char colonne) {
    char address;

    if (ligne != 0) {
        address = 0x40; //ligne 2
    } else {
        address = 0;
    }
    address += colonne;
    lcd_write_cmd_busy(0x80 | address);
}

///////////////////////////////////////////////////////////////////////////////
//  Nom de fonction  :  lcd_putc
//  Valeur de retour :  aucune
//  Paramètres       :  unsigned char lettre
//                        code ASCII du caractère à afficher
//  Description      :  affiche un caractère sur l'écran en gérant les
//                      caractères spéciaux \n \f \b
///////////////////////////////////////////////////////////////////////////////

void lcd_putc(unsigned char lettre) {
    if (lettre >= 0x20) {
        lcd_write_data_busy(lettre);
    } else if (lettre == '\f') {
        lcd_write_cmd_busy(0x01);
    } else if (lettre == '\n') {
        lcd_position(1, 0);
    } else if (lettre == '\b') {
        lcd_write_cmd_busy(0x10);
    } else {
        lcd_write_data_busy(lettre);
    }
}

#define _FLAG_MINUS 0x1
#define _FLAG_PLUS  0x2
#define _FLAG_SPACE 0x4
#define _FLAG_OCTO  0x8
#define _FLAG_ZERO  0x10
#define _FLAG_SIGNED 0x80

static const char s_digits[] = "0123456789abcdef";

#define _FMT_UNSPECIFIED 0
#define _FMT_LONG 1
#define _FMT_SHLONG 2
#define _FMT_BYTE   3

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
void lcd_printf(const char *f, ...) {
    va_list ap;
    unsigned char c;
    //    int count = 0;

    va_start(ap, f);
    for (c = *f; c; c = *++f) {
        if (c == '%') {
            unsigned char flags = 0;
            unsigned char width = 0;
            unsigned char precision = 0;
            unsigned char have_precision = 0;
            unsigned char size = 0;
            unsigned char space_cnt;
            unsigned char cval;
            unsigned long larg;
            far char *romstring;
            char *ramstring;
            int n;

            c = *++f;

             

            // check for a flag.
            //   7.9.6.1
            //   The flag characters and their meanings are:

            //   - The result of the conversion will be left-justified within
            //     the field. (It will be right justified if this flag is not
            //     specified.)
            //   + The result of a signed conversion will always begin with a
            //     plus or a minus sign. (It will begin with a sign only when
            //     a negative value is converted if this flag is not specified.)
            //   space  If the first character of a signed conversion is not a
            //     sign, or if a signed conversion results in no characters, a
            //     space will be prefixed to the result. If the space and + flags
            //     both appear, the space flag will be ignored.
            //   # The result is to be converted to an "alternate form." For o
            //     conversion, it increases the precision to force the first digit
            //     of the result to be a zero. For x (or X) conversion, a nonzero
            //     result will have 0x (or 0X) prefixed to it. For e, E, f, g,
            //     and G conversions, the result will always contain a decimal-
            //     point character, even if no digits follow it. (Normally, a
            //     decimal point character appears in the result of these
            //     conversions only if a digit follows it.) For g and G
            //     conversions, trailing zeros will not be removed from the
            //     result. For other conversions the behaviour is undefined.
            //   0 For d, i, o, u, x, X, e, E, f, g, and G conversions, leading
            //     zeros (following any indication of sign or base) are used to
            //     pad to the field width; no space padding is performed. If
            //     the 0 and - flags both appear, the 0 flag will be ignored.
            //     For other conversions, the behaviour is undefined.
             
            while (c == '-' || c == '+' || c == ' ' || c == '#'
                    || c == '0') {
                switch (c) {
                    case '-':
                        flags |= _FLAG_MINUS;
                        break;
                    case '+':
                        flags |= _FLAG_PLUS;
                        break;
                    case ' ':
                        flags |= _FLAG_SPACE;
                        break;
                    case '#':
                        flags |= _FLAG_OCTO;
                        break;
                    case '0':
                        flags |= _FLAG_ZERO;
                        break;
                }
                c = *++f;
            }
            // the optional width field is next 
            if (c == '*') {
                n = va_arg(ap, int);
                if (n < 0) {
                    flags |= _FLAG_MINUS;
                    width = -n;
                } else
                    width = n;
                c = *++f;
            } else {
                cval = 0;
                while ((c >= '0') && (c <= '9')) {
                    cval = cval * 10 + c - '0';
                    c = *++f;
                }
                width = cval;
            }

            // if '-' is specified, '0' is ignored 
            if (flags & _FLAG_MINUS)
                flags &= ~_FLAG_ZERO;

            // the optional precision field is next 
            if (c == '.') {
                c = *++f;
                if (c == '*') {
                    n = va_arg(ap, int);
                    if (n >= 0) {
                        precision = n;
                        have_precision = 1;
                    }
                    c = *++f;
                } else {
                    cval = 0;
                    while ((c >= '0') && (c <= '9')) {
                        cval = cval * 10 + c - '0';
                        c = *++f;
                    }
                    precision = cval;
                    have_precision = 1;
                }
            }

            // the optional 'h' specifier. since int and short int are
             //  the same size for MPLAB C18, this is a NOP for us. 
            if (c == 'h') {
                c = *++f;
                // if 'c' is another 'h' character, this is an 'hh'
                //   specifier and the size is 8 bits 
                if (c == 'h') {
                    size = _FMT_BYTE;
                    c = *++f;
                }
            }     
            // 'z' and 't' are both 16-bit and so are NOPs 
            else if (c == 't' || c == 'z')
                c = *++f;
                // the 'H' specifier tells us to deal with 24-bit integers.
                //   the 'T' and 'Z' specifiers are also 24 bit 
            else if (c == 'H' || c == 'T' || c == 'Z') {
                size = _FMT_SHLONG;
                c = *++f;
            }         
            // the 'l' specifier tells us to deal with 32-bit integers
            //   'j' specifies intmax_t (long in our case). 
            else if (c == 'l' || c == 'j') {
                size = _FMT_LONG;
                c = *++f;
            }

            switch (c) {
                case '\0':
                    // this is undefined behaviour. we have a trailing '%' character
                    //   in the string, perhaps with some flags, width, precision
                    //   stuff as well, but no format specifier. We'll, arbitrarily,
                    //   back up a character so that the loop will terminate
                    //   properly when it loops back and we'll output a '%'
                    //   character. 
                    --f;
                    // fallthrough 
                case '%':
                    lcd_write_data_busy('%');
                    //                ++count;
                    break;
                case 'c':
                    space_cnt = 0;
                    if (width > 1) {
                        space_cnt = width - 1;
                        //                    count += space_cnt;
                    }
                    if (space_cnt && !(flags & _FLAG_MINUS)) {
                        lcd_repete_n(space_cnt, ' ');
                        space_cnt = 0;
                    }
                    c = va_arg(ap, int);
                    lcd_putc(c);
                    //                ++count;
                    lcd_repete_n(space_cnt, ' ');
                    break;
                case 'S':
                    if (size == _FMT_SHLONG)
                        romstring = va_arg(ap, far char *);
                    else
                        romstring = (far char*) va_arg(ap, near char *);
                    for (n = 0; romstring[n] != 0; n++);
                    // Normalize the width based on the length of the actual
                    //   string and the precision. 
                    if (have_precision && precision < (unsigned char) n)
                        n = precision;
                    if (width < (unsigned char) n)
                        width = n;
                    space_cnt = width - (unsigned char) n;
                    //                count += space_cnt;
                    // we've already calculated the space count that the width
                    //   will require. now we want the width field to have the
                    //   number of character to display from the string itself,
                    //   limited by the length of the actual string and the
                    //   specified precision. 
                    if (have_precision && precision < width)
                        width = precision;
                    // if right justified, we print the spaces before the
                    //   string 
                    if (!(flags & _FLAG_MINUS)) {
                        lcd_repete_n(space_cnt, ' ');
                        space_cnt = 0;
                    }
                    cval = 0;
                    for (c = *romstring; c && cval < width; c = *++romstring) {
                        lcd_putc(c);
                        //                    ++count;
                        ++cval;
                    }
                    // If there are spaces left, it's left justified.
                    //   Either way, calling the function unconditionally
                    //   is smaller code.
                    lcd_repete_n(space_cnt, ' ');
                    break;
                case 's':
                    ramstring = va_arg(ap, char *);
                    for (n = 0; ramstring[n] != 0; n++);
                    // Normalize the width based on the length of the actual
                    //   string and the precision. 
                    if (have_precision && precision < (unsigned char) n)
                        n = precision;
                    if (width < (unsigned char) n)
                        width = n;
                    space_cnt = width - (unsigned char) n;
                    //                count += space_cnt;
                    // we've already calculated the space count that the width
                    //   will require. now we want the width field to have the
                    //   number of character to display from the string itself,
                    //   limited by the length of the actual string and the
                    //   specified precision. 
                    if (have_precision && precision < width)
                        width = precision;
                    // if right justified, we print the spaces before the
                    //   string 
                    if (!(flags & _FLAG_MINUS)) {
                        lcd_repete_n(space_cnt, ' ');
                        space_cnt = 0;
                    }
                    cval = 0;
                    for (c = *ramstring; c && cval < width; c = *++ramstring) {
                        lcd_putc(c);
                        //                    ++count;
                        ++cval;
                    }
                    // If there are spaces left, it's left justified.
                    //   Either way, calling the function unconditionally
                    //   is smaller code. 
                    lcd_repete_n(space_cnt, ' ');
                    break;
                case 'd':
                case 'i':
                    flags |= _FLAG_SIGNED;
                    // fall through 
                case 'o':
                case 'u':
                case 'x':
                case 'X':
                case 'b':
                case 'B':
                    // This is a bit of a sneaky trick. The 'l' and 'hh' size
                    //   specifiers are valid only for the integer conversions,
                    //   not the 'p' or 'P' conversions, and are ignored for the
                    //   latter. By jumping over the additional size specifier
                    //   checks here we get the best code size since we can
                    //   limit the size checks in the remaining code. 
                    if (size == _FMT_LONG) {
                        larg = va_arg(ap, long int);
                        goto _do_integer_conversion;
                    } else if (size == _FMT_BYTE) {
                        if (flags & _FLAG_SIGNED)
                            larg = (signed char) va_arg(ap, int);
                        else
                            larg = (unsigned char) va_arg(ap, unsigned int);
                        goto _do_integer_conversion;
                    }
                    // fall through 
                case 'p':
                case 'P':
                    if (size == _FMT_SHLONG) {
                        if (flags & _FLAG_SIGNED)
                            larg = va_arg(ap, short long int);
                        else
                            larg = va_arg(ap, unsigned short long int);
                    } else if (flags & _FLAG_SIGNED)
                        larg = va_arg(ap, int);
                    else
                        larg = va_arg(ap, unsigned int);
_do_integer_conversion:
                    // default precision is 1 
                    if (!have_precision)
                        precision = 1;

                {
                    unsigned char digit_cnt = 0;
                    unsigned char prefix_cnt = 0;
                    unsigned char sign_char;
                    // A 32 bit number will require at most 32 digits in the
                    //   string representation (binary format). 
                    char buf[33];
                    // Start storing digits least-significant first 
                    char *q = &buf[31];
                    // null terminate the string 
                    buf[32] = '\0';

                    space_cnt = 0;
                    size = 10;

                    switch (c) {
                        case 'b':
                        case 'B':
                            size = 2;
                            break;
                        case 'o':
                            size = 8;
                            break;
                        case 'p':
                        case 'P':
                            // from here on out, treat 'p' conversions just
                            //   like 'x' conversions. 
                            c += 'x' - 'p';
                            // fall through 
                        case 'x':
                        case 'X':
                            size = 16;
                            break;
                    }

                    // if it's an unsigned conversion, we should ignore the
                    //   ' ' and '+' flags 
                    if (!(flags & _FLAG_SIGNED))
                        flags &= ~(_FLAG_PLUS | _FLAG_SPACE);

                    // if it's a negative value, we need to negate the
                    //   unsigned version before we convert to text. Using
                    //   unsigned for this allows us to (ab)use the 2's
                    //   complement system to avoid overflow and be able to
                    //   adequately handle LONG_MIN.

                    //   We'll figure out what sign character to print, if
                    //   any, here as well. 
                    if (flags & _FLAG_SIGNED && ((long) larg < 0)) {
                        larg = -(long) larg;
                        sign_char = '-';
                        ++digit_cnt;
                    } else if (flags & _FLAG_PLUS) {
                        sign_char = '+';
                        ++digit_cnt;
                    } else if (flags & _FLAG_SPACE) {
                        sign_char = ' ';
                        ++digit_cnt;
                    } else
                        sign_char = '\0';
                    // get the digits for the actual number. If the
                    //   precision is zero and the value is zero, the result
                    //   is no characters. 
                    if (precision || larg) {
                        do {
                            cval = s_digits[larg % size];
                            if (c == 'X' && cval >= 'a')
                                cval -= 'a' - 'A';
                            larg /= size;
                            *q-- = cval;
                            ++digit_cnt;
                        } while (larg);
                        // if the '#' flag was specified and we're dealing
                        //   with an 'o', 'b', 'B', 'x', or 'X' conversion,
                        //   we need a bit more. 
                        if (flags & _FLAG_OCTO) {
                            if (c == 'o') {
                                // per the standard, for octal, the '#' flag
                                //   makes the precision be at least one more
                                //   than the number of digits in the number 
                                if (precision <= digit_cnt)
                                    precision = digit_cnt + 1;
                            } else if (c == 'x'
                                    || c == 'X' || c == 'b' || c == 'B')
                                prefix_cnt = 2;
                        }
                    } else
                        digit_cnt = 0;

                    // The leading zero count depends on whether the '0'
                    //   flag was specified or not. If it was not, then the
                    //   count is the difference between the specified
                    //   precision and the number of digits (including the
                    //   sign character, if any) to be printed; otherwise,
                    //   it's as if the precision were equal to the max of
                    //   the specified precision and the field width. If a
                    //   precision was specified, the '0' flag is ignored,
                    //   however. 
                    if ((flags & _FLAG_ZERO) && (width > precision)
                            && !have_precision)
                        precision = width;
                    // for the rest of the processing, precision contains
                    //   the leading zero count for the conversion. 
                    if (precision > digit_cnt)
                        precision -= digit_cnt;
                    else
                        precision = 0;
                    // the space count is the difference between the field
                    //   width and the digit count plus the leading zero
                    //   count. If the width is less than the digit count
                    //   plus the leading zero count, the space count is
                    //   zero. 
                    if (width > precision + digit_cnt + prefix_cnt)
                        space_cnt =
                            width - precision - digit_cnt - prefix_cnt;

                    // for output, we check the justification, if it's
                    //   right justified and the space count is positive, we
                    //   emit the space characters first. 
                    if (!(flags & _FLAG_MINUS) && space_cnt) {
                        lcd_repete_n(space_cnt, ' ');
                        //                        count += space_cnt;
                        space_cnt = 0;
                    }
                    // if we have a sign character to print, that comes
                    //   next 
                    if (sign_char) {
                        lcd_write_data_busy(sign_char);
                    }
                    // if we have a prefix (0b, 0B, 0x or 0X), that's next 
                    if (prefix_cnt) {
                        lcd_write_data_busy('0');
                        lcd_write_data_busy(c);
                    }
                    // if we have leading zeros, they follow. the prefix, if any
                    //   is included in the number of digits when determining how
                    //   many leading zeroes are needed. 
                    if (precision > prefix_cnt)
                        precision -= prefix_cnt;
                    lcd_repete_n(precision, '0');
                    // print the actual number 
                    for (cval = *++q; cval; cval = *++q) {
                        lcd_write_data_busy(cval);
                    }
                    // if there are any spaces left, they go to right-pad
                    //   the field 
                    lcd_repete_n(space_cnt, ' ');

                    //                    count += precision + digit_cnt + space_cnt + prefix_cnt;
                }
                    break;
                case 'f': // implémentation partielle
                {
                    float x;
                    unsigned char i, w;
                    unsigned int k;
                    unsigned char sign, have_sign = 0;
                    x = va_arg(ap, float);
                    if (x < 0.0) {
                        sign = '-';
                        x = -x;
                        have_sign = 1;
                    } else {
                        if (flags & _FLAG_PLUS) {
                            sign = '+';
                            have_sign = 1;
                        } else if (flags & _FLAG_SPACE) {
                            sign = ' ';
                            have_sign = 1;
                        }
                    }
                    for (i = 0; x >= 1; i++) x /= 10;
                    if ((!have_precision) && (width == 0)) {
                        precision = 3;
                        have_precision = 1;
                    }
                    w = precision + i;
                    if (precision > 0) w++;
                    if (have_sign) w++;
                    if (i == 0) w++;
                    if (!have_precision) {
                        if (width > w + 1) {
                            precision = width - (w + 1);
                            w = width;
                        }
                    }
                    if (!(flags & _FLAG_MINUS)) {
                        if (flags & _FLAG_ZERO) {
                            if (have_sign) lcd_write_data_busy(sign);
                            for (; width > w; w++) lcd_write_data_busy('0');
                        } else {
                            for (; width > w; w++) lcd_write_data_busy(' ');
                            if (have_sign) lcd_write_data_busy(sign);
                        }
                    } else {
                        if (have_sign) lcd_write_data_busy(sign);
                    }
                    if (i == 0) {
                        lcd_write_data_busy('0');
                    }
                    for (; i > 0; i--) {
                        x *= 10;
                        k = (unsigned int)x;
                        x = x - k;
                        lcd_write_data_busy(k + '0');
                    }
                    if (precision > 0) lcd_write_data_busy('.');
                    for (; i < precision; i++) {
                        x *= 10;
                        k = (unsigned int)x;
                        x = x - k;
                        lcd_write_data_busy(k + '0');
                    }
                    if (flags & _FLAG_MINUS) {
                        for (; width > w; w++) lcd_write_data_busy(' ');
                    }
                }
                    break;
                default:
                    // undefined behaviour. we do nothing 
                    break;
            }
        } else {
            lcd_putc(c);
            //            ++count;
        }
    }

    va_end(ap);
    //    return count;
}

///////////////////////////////////////////////////////////////////////////////
// Définitions des fonctions internes à la bibliothèque
///////////////////////////////////////////////////////////////////////////////

void lcd_write_cmd_data(unsigned char c) {
    LCD_RW_PIN = 0; // Set control signals
    LCD_TRIS_DATA_PORT &= 0x0f; // Port données en écriture
    LCD_DATA_PORT &= 0x0f; // Efface le port données
    LCD_DATA_PORT |= (c & 0xf0); // Inscrit les bits de poids fort
    lcd_clock_e();
    LCD_DATA_PORT &= 0x0f; // idem, pour les bits de poids faible
    LCD_DATA_PORT |= ((c << 4)&0xf0);
    lcd_clock_e();
    LCD_TRIS_DATA_PORT |= 0xf0;
}

static void lcd_write_data_busy(unsigned char c) {
    while (lcd_busy());
    LCD_RS_PIN = 1;
    lcd_write_cmd_data(c);
}

static void lcd_write_cmd_busy(unsigned char c) {
    while (lcd_busy());
    LCD_RS_PIN = 0;
    lcd_write_cmd_data(c);
}

static unsigned char lcd_busy(void) {
    LCD_RW_PIN = 1; // Set the control bits for read
    LCD_RS_PIN = 0;
    lcd_delai_250n();
    LCD_E_PIN = 1;
    lcd_delai_250n();
    if (LCD_DATA_PORT & 0x80) {
        LCD_E_PIN = 0;
        lcd_delai_250n();
        lcd_clock_e();
        LCD_RW_PIN = 0; // Reset control line
        return 1; // Return TRUE
    } else { // Busy bit is low
        LCD_E_PIN = 0;
        lcd_delai_250n();
        lcd_clock_e();
        LCD_RW_PIN = 0; // Reset control line
        return 0; // Return FALSE
    }
}

static void lcd_clock_e(void) {
    LCD_E_PIN = 1;
    lcd_delai_250n();
    LCD_E_PIN = 0;
    lcd_delai_250n();
}

static void lcd_repete_n(unsigned char n, char c) {
    while (n--) lcd_write_data_busy(c);
}


///////////////////////////////////////////////////////////////////////////////
//
// Obsolètes
// Fournies pour compatibilité avec la version précédente de la bibliothèque.
//
///////////////////////////////////////////////////////////////////////////////

void lcd_gotoyx(char ligne, char colonne) {
    lcd_position((ligne) - 1, (colonne) - 1);
}

void lcd_puti(int nombre) {
    lcd_printf("%d", nombre);
}

void lcd_putrs(const char *chaine) {
    lcd_printf("%S", chaine);
}

void lcd_puts(char *chaine) {
//    lcd_printf("%s", chaine);
}


#ifdef _OMNI_CODE_
#pragma warning pop
#endif