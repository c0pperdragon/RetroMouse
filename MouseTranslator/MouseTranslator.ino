// Target controller:  
//   attiny1604 with 16mhz
//   Disabled millis()/micros() to avoid timing glitches and free timer A
//   setup time 0 ms

#include "PS2MouseHandler.h"

#define MOUSE_DATA  PIN_PB3
#define MOUSE_CLOCK PIN_PB2
                                 // C64      Amiga mouse
#define SUBD_PIN1   PIN_PA1      // up       mouse v
#define SUBD_PIN2   PIN_PA3      // down     mouse h
#define SUBD_PIN3   PIN_PA4      // left     mouse vq
#define SUBD_PIN4   PIN_PA5      // right    mouse hq
#define SUBD_PIN5   PIN_PA6      // pot y    middle button
#define SUBD_PIN6   PIN_PA2      // button   left buttonm
#define SUBD_PIN9   PIN_PA7      // pot x    right button

#define MODE_UNINITIALIZED 0
#define MODE_AMIGAMOUSE    1
#define MODE_C64MOUSE      2
#define MODE_PADDLES       3

PS2MouseHandler mouse(MOUSE_CLOCK, MOUSE_DATA);
unsigned int x;
unsigned int y;

// ------ low-level subd pin handling ---------
// set pin to either low (when active) or having a weak pullup
void set_subd(byte pin, bool active)
{
    if (active) 
    {
        digitalWrite(pin, LOW);  
        pinMode(pin, OUTPUT);
    }
    else
    {
        pinMode(pin, INPUT_PULLUP);
    }
}
void init_subd(bool emulate_pots)
{
    set_subd(SUBD_PIN1, false);
    set_subd(SUBD_PIN2, false);
    set_subd(SUBD_PIN3, false);
    set_subd(SUBD_PIN4, false);
    set_subd(SUBD_PIN5, false);
    set_subd(SUBD_PIN6, false);
    set_subd(SUBD_PIN9, false);
    AC0.INTCTRL = 0x00;          // disable AC interrupt       
    TCA0.SINGLE.INTCTRL = 0x00;  // disable TCA0 interrupts
    if (emulate_pots)
    {
        VREF.CTRLA = 0x01;         // use 1.1V as VREF for the AC
        AC0.MUXCTRLA = 0x02;       // compare PA7 against voltage reference
        AC0.CTRLA = 0x27;          // trigger at negative edge, maximum hysteresis
        AC0.INTCTRL = 0x01;        // enable AC interrupt 

        TCA0.SINGLE.CTRLA = 0x01;          // enable timer at full speed
        TCA0.SINGLE.CTRLB = 0x00;          // normal counting mode
        TCA0.SINGLE.CTRLC = 0x00;          // no timer output on pins
        TCA0.SINGLE.CTRLD = 0x00;          // disable split mode (use single mode)
        TCA0.SINGLE.CTRLECLR = 0x03;       // normal register update, couting up
        TCA0.SINGLE.EVCTRL = 0x00;         // no event couting                
        TCA0.SINGLE.CNT = 0;
        TCA0.SINGLE.PER = 0xffff;
        TCA0.SINGLE.CMP0 = 0xffff;
        TCA0.SINGLE.CMP1 = 0xffff;
        TCA0.SINGLE.INTCTRL = 0x30;        // enable compare interrupt on cannels 0 & 1 
    }
}
unsigned long potx;
unsigned long poty;
void set_pots(unsigned int x, unsigned int y)
{
    TCA0.SINGLE.CMP0BUF = x;
    TCA0.SINGLE.CMP1BUF = y;
}
// triggered when PA7 (PIN_SUBD9) is externally pulled low
ISR (AC0_AC_vect) 
{
    AC0.STATUS = 0x01;        // clear interrupt flag
    PORTA.PIN7CTRL = 0x00;    // disable pull.up
    PORTA.PIN6CTRL = 0x00;    // disable pull-up
    TCA0.SINGLE.CTRLESET = 0x08;  // command to restart the counter
    TCA0.SINGLE.CTRLESET = 0x04;  // command to update values from buffers
}
// triggerd when pull-up for potx should be activated
//ISR (TCA0_CMP0_vect) 
//{
//    TCA0.SINGLE.INTFLAGS = 0x10;   // clear interrupt flag 
//    PORTA.PIN7CTRL = 0x08;         // enable pull.up
//}
ISR (TCA0_CMP0_vect, ISR_NAKED ) {
    asm ( 
         "push r24          \n"  
         "ldi  r24, 0x10    \n"  
         "sts  0x0a0b, r24  \n"
         "ldi  r24, 0x08    \n"
         "sts  0x0417, r24  \n"
         "pop  r24          \n"
         "reti              \n"
    );
}
// triggerd when pull-up for poty should be activated
//ISR (TCA0_CMP1_vect) 
//{
//    TCA0.SINGLE.INTFLAGS = 0x20;   // clear interrupt flag 
//    PORTA.PIN6CTRL = 0x08;         // enable pull-up
//}
ISR (TCA0_CMP1_vect, ISR_NAKED ) {
    asm ( 
         "push r24          \n"  
         "ldi  r24, 0x20    \n"  
         "sts  0x0a0b, r24  \n"
         "ldi  r24, 0x08    \n"
         "sts  0x0416, r24  \n"
         "pop  r24          \n"
         "reti              \n"
    );
}


unsigned int amiga_x;
unsigned int amiga_y;

void init_amigamouse()
{
    init_subd(false);
    amiga_x = 0;
    amiga_y = 0;
}

void run_amigamouse()
{
    unsigned int newx = x >> 2;
    unsigned int newy = y >> 2;
    set_subd(SUBD_PIN6, mouse.button(0)!=0);
    set_subd(SUBD_PIN5, mouse.button(1)!=0);
    set_subd(SUBD_PIN9, mouse.button(2)!=0);
    while ((amiga_x != newx) || (amiga_y != newy))
    {
        if (amiga_x != newx)
        {
            if (mouse.x_movement()<0) { amiga_x = (amiga_x - 1) & 0x3fff; }
            else                      { amiga_x = (amiga_x + 1) & 0x3fff; }
            set_subd(SUBD_PIN2, (amiga_x&0x2) != 0);
            set_subd(SUBD_PIN4, ((amiga_x-1)&0x2) != 0);
        }
    
        if (amiga_y != newy)
        {
            if (mouse.y_movement()<0) { amiga_y = (amiga_y - 1) & 0x3fff; }
            else                      { amiga_y = (amiga_y + 1) & 0x3fff; }
            set_subd(SUBD_PIN1, (amiga_y&0x2) != 0);
            set_subd(SUBD_PIN3, ((amiga_y+1)&0x2) != 0);
        }
        delayMicroseconds(50);
    }
}


#define C64MOUSEMIN 5100
#define C64MOUSEMAX 7200

void init_c64mouse()
{
    init_subd(true);
}

void run_c64mouse()
{
    set_subd(SUBD_PIN6, mouse.button(0)!=0);
    set_subd(SUBD_PIN1, mouse.button(2)!=0);
    long scalex = x & 0x1ff;
    long scaley = y & 0x1ff;
    scalex = (scalex * (C64MOUSEMAX-C64MOUSEMIN)) >> 9;
    scaley = (scaley * (C64MOUSEMAX-C64MOUSEMIN)) >> 9;
    set_pots(C64MOUSEMIN+(int)scalex, C64MOUSEMIN+(int)scaley);
}

#define PADDLEMIN 3900
#define PADDLEMAX 7750

void init_paddles()
{
    init_subd(true);
}

void run_paddles()
{
    set_subd(SUBD_PIN3, mouse.button(0)!=0);
    set_subd(SUBD_PIN4, mouse.button(1)!=0);

    if (x<PADDLEMIN) { x = PADDLEMIN; }
    if (x>PADDLEMAX) { x = PADDLEMAX; }
    if (y<PADDLEMIN) { y = PADDLEMIN; }
    if (y>PADDLEMAX) { y = PADDLEMAX; }
    set_pots(PADDLEMAX-(x-PADDLEMIN),y);
}


int detect_mode()
{
    return MODE_C64MOUSE;  
//    return MODE_PADDLES;  
}


void setup()
{   
    pinMode(PIN_PA4, OUTPUT);    // debugging pin
    digitalWrite(PIN_PA4, LOW);
}

void loop()
{
    // quickly turn on pull-ups to allow PS/2 mouse to auto-detect mode
    pinMode(MOUSE_DATA, INPUT_PULLUP); 
    pinMode(MOUSE_CLOCK, INPUT_PULLUP);
    delay(1000); // give mouse time to power on
    
    if (mouse.initialise()!=0) { return; }
    x = 0;
    y = 0;

    int mode = detect_mode();
    switch (mode)
    {
        case MODE_AMIGAMOUSE: init_amigamouse(); break;
        case MODE_C64MOUSE:   init_c64mouse();   break;
        case MODE_PADDLES:    init_paddles(); break;
    } 
         
    for (;;)
    {
        if (mouse.get_data()!=0) { return; }
        x = x + (unsigned int) mouse.x_movement();
        y = y + (unsigned int) mouse.y_movement();     

        switch (mode)
        {
            case MODE_AMIGAMOUSE: run_amigamouse(); break;
            case MODE_C64MOUSE:   run_c64mouse();   break;
            case MODE_PADDLES:    run_paddles(); break;
        }    
    }
}
 
