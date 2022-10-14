// Target controller:  
//   attiny1604 with 16mhz
//   Disabled millis()/micros() to avoid timing glitches and free timer A
//   setup time 0 ms

#include "PS2MouseHandler.h"

#define DEBUG       PIN_PB1
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

#define MODE_AMIGA    1
#define MODE_C64      2
#define MODE_ATARI    3
byte mode;

PS2MouseHandler mouse(MOUSE_CLOCK, MOUSE_DATA);
unsigned int x;
unsigned int y;



// ------ low-level subd pin handling ---------
bool countedges;
unsigned int edgecounter;
unsigned int edgeseparation;

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
void init_subd(bool emulate_pots, bool count)
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

    TCA0.SINGLE.CTRLA = 0x01;          // enable timer at full speed
    TCA0.SINGLE.CTRLB = 0x00;          // normal counting mode
    TCA0.SINGLE.CTRLC = 0x00;          // no timer output on pins
    TCA0.SINGLE.CTRLD = 0x00;          // disable split mode (use single mode)
    TCA0.SINGLE.CTRLECLR = 0x03;       // normal register update, counting up
    TCA0.SINGLE.EVCTRL = 0x00;         // no event couting                
    TCA0.SINGLE.CNT = 0;
    TCA0.SINGLE.PER = 0xffff;
    TCA0.SINGLE.CMP0 = 0xffff;
    TCA0.SINGLE.CMP1 = 0xffff;
    if (emulate_pots)
    {
        countedges = false;        
        TCA0.SINGLE.INTCTRL = 0x30;        // enable compare interrupt on cannels 0 & 1 
    }
    else if (count)
    {
        countedges = true;
    }
    if (emulate_pots || count)
    {
        VREF.CTRLA = 0x00;         // use 0.55V as VREF for the AC
        AC0.MUXCTRLA = 0x02;       // compare PA7 against voltage reference
        AC0.CTRLA = 0x27;          // trigger at negative edge, maximum hysteresis
        AC0.INTCTRL = 0x01;        // enable AC interrupt 
    }    
}
void set_pots(unsigned int x, unsigned int y)
{
    TCA0.SINGLE.CMP0BUF = x;
    TCA0.SINGLE.CMP1BUF = y;
}
// triggered when PA7 (PIN_SUBD9) goes low 
ISR (AC0_AC_vect) 
{
    if (countedges) 
    { 
        unsigned int c = TCA0.SINGLE.CNT; // read counter value to determine period length
        TCA0.SINGLE.CTRLESET = 0x08;      // command to restart the counter
        c = c+100;                         // fine-tuned
        if (c>7000 && c<9000) // check for plausibility
        {   // slowly adjust detected period
            if (c<edgeseparation-10) { edgeseparation-=3; }
            else if (c<edgeseparation) { edgeseparation--; }
            else if (c>edgeseparation+10) { edgeseparation+=3; }
            else if (c>edgeseparation) { edgeseparation++; }
        }
        edgecounter++; 
    } 
    else
    {
        // turn off pull-ups and trigger timer to later re-enable them
        PORTA.PIN7CTRL = 0x00;    // disable pull.up
        PORTA.PIN6CTRL = 0x00;    // disable pull-up
        TCA0.SINGLE.CTRLESET = 0x08;  // command to restart the counter
        TCA0.SINGLE.CTRLESET = 0x04;  // command to update values from buffers        
    }        
    AC0.STATUS = 0x01;        // clear interrupt flag
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

void init_amiga()
{
    init_subd(false,false);
    amiga_x = 0;
    amiga_y = 0;
}

void run_amiga()
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


#define C64MOUSEMIN 5000
unsigned int c64_mousespan; 

#define C64PADDLEMIN 3900
#define C64PADDLEMAX 7750

bool c64_usepaddles;
int c64_prevleftbutton;
int c64_countclicks;

void init_c64()
{
    init_subd(true,false);
    c64_usepaddles = false;
}

void run_c64()
{
    if (c64_usepaddles)
    {
        set_subd(SUBD_PIN1, mouse.button(2)!=0);
        set_subd(SUBD_PIN3, mouse.button(0)!=0);
        set_subd(SUBD_PIN6, false);
        if (x<C64PADDLEMIN) { x = C64PADDLEMIN; }
        if (x>C64PADDLEMAX) { x = C64PADDLEMAX; }
        if (y<C64PADDLEMIN) { y = C64PADDLEMIN; }
        if (y>C64PADDLEMAX) { y = C64PADDLEMAX; }
        set_pots(C64PADDLEMAX-(x-C64PADDLEMIN),y);
    }
    else
    {
        set_subd(SUBD_PIN1, mouse.button(2)!=0);
        set_subd(SUBD_PIN3, false);
        set_subd(SUBD_PIN6, mouse.button(0)!=0);
        long scalex = x & 0x1ff;
        long scaley = y & 0x1ff;
        scalex = (scalex * (c64_mousespan)) >> 9;
        scaley = (scaley * (c64_mousespan)) >> 9;
        set_pots(C64MOUSEMIN+(int)scalex, C64MOUSEMIN+(int)scaley);
    }
    // handle manual c64 mode switching
    if (mouse.button(2)!=0)   // while holding right mouse button 
    {
        if (mouse.button(0)!=0 && c64_prevleftbutton==0) { c64_countclicks++; }      
    }
    else                      // after releasing right mouse button
    {
        if (c64_countclicks==5) { c64_usepaddles = true; }        
        if (c64_countclicks==6) { c64_usepaddles = false; }
        c64_countclicks = 0;
    }
    c64_prevleftbutton = mouse.button(0);
}


void init_atari()
{
    init_subd(false,false);
}

void run_atari()
{
    set_subd(SUBD_PIN1, mouse.button(2)!=0);
    set_subd(SUBD_PIN3, mouse.button(0)!=0);
    set_subd(SUBD_PIN6, false);

    // TODO: paddle emulation
}

void detect_mode()
{
    edgecounter = 0;
    edgeseparation = 8192;  // first estimate of scan interval (in arduino clocks) 

    init_subd(false,true);
    delay(500);
    init_subd(false,false);
    pinMode(SUBD_PIN9, INPUT);
    delay(500);
    
    if (edgecounter>500) 
    { 
        mode=MODE_C64; 
        c64_mousespan = edgeseparation / 4;
    }
    else 
    { 
        if (digitalRead(SUBD_PIN9)==HIGH) { mode=MODE_AMIGA; }
        else { mode = MODE_ATARI; }
    }    
}


void setup()
{   
    // debugging pin
    pinMode(DEBUG, OUTPUT);    
    digitalWrite(DEBUG, LOW);
    
    // quickly turn on pull-ups to allow mouse to auto-detect PS/2 mode
    pinMode(MOUSE_DATA, INPUT_PULLUP); 
    pinMode(MOUSE_CLOCK, INPUT_PULLUP);

    detect_mode();
    switch (mode)
    {
        case MODE_AMIGA:  init_amiga();   break;
        case MODE_C64:    init_c64();     break;
        case MODE_ATARI:  init_atari();  break;
    }     
}

void loop()
{
    if (mouse.initialise()!=0) { return; }
    x = 0;
    y = 0;
         
    for (;;)
    {
        if (mouse.get_data()!=0) { return; }
        x = x + (unsigned int) mouse.x_movement();
        y = y + (unsigned int) mouse.y_movement();     

        switch (mode)
        {
            case MODE_AMIGA:  run_amiga();  break;
            case MODE_C64:    run_c64();    break;
            case MODE_ATARI:  run_atari(); break;
        }    
    }
}
 
