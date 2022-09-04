#include <PS2MouseHandler.h>

#define MOUSE_DATA  PIN_PB3
#define MOUSE_CLOCK PIN_PB2
                                 // C64      Amiga mouse
#define SUBD_PIN1   PIN_PA3      // up       mouse v
#define SUBD_PIN2   PIN_PA2      // down     mouse h
#define SUBD_PIN3   PIN_PA1      // left     mouse vq
#define SUBD_PIN4   PIN_PB0      // right    mouse hq
#define SUBD_PIN5   PIN_PB1      // pot y    middle button
#define SUBD_PIN6   PIN_PA6      // button   left button
#define SUBD_PIN9   PIN_PA7      // pot x    right button

#define MODE_UNINITIALIZED 0
#define MODE_JOYSTICK      1
#define MODE_AMIGAMOUSE    2

PS2MouseHandler mouse(MOUSE_CLOCK, MOUSE_DATA, PS2_MOUSE_REMOTE);

byte mode;
unsigned long last_poll;
unsigned int x;
unsigned int y;

void opencollector(byte pin, bool active)
{
    if (active) 
    {
        digitalWrite(pin, LOW);  
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);  
    }
    else
    {
        pinMode(pin, INPUT_PULLUP);
    }
}

void init_subd()
{
    opencollector(SUBD_PIN1, false);
    opencollector(SUBD_PIN2, false);
    opencollector(SUBD_PIN3, false);
    opencollector(SUBD_PIN4, false);
    opencollector(SUBD_PIN5, false);
    opencollector(SUBD_PIN6, false);
    opencollector(SUBD_PIN9, false);
}


void init_joystick()
{
    init_subd();
}

void run_joystick()
{
    opencollector(SUBD_PIN1, mouse.y_movement()<0);
    opencollector(SUBD_PIN2, mouse.y_movement()>0);
    opencollector(SUBD_PIN3, mouse.x_movement()<0);
    opencollector(SUBD_PIN4, mouse.x_movement()>0);
    opencollector(SUBD_PIN6, mouse.button(0));
}

unsigned int amiga_x;
unsigned int amiga_y;

void init_amigamouse()
{
    init_subd();
    amiga_x = 0;
    amiga_y = 0;
}

void run_amigamouse()
{
    unsigned int newx = x >> 1;
    unsigned int newy = y >> 1;
    opencollector(SUBD_PIN6, mouse.button(0)!=0);
    opencollector(SUBD_PIN5, mouse.button(1)!=0);
    opencollector(SUBD_PIN9, mouse.button(2)!=0);
    while ((amiga_x != newx) || (amiga_y != newy))
    {
        if (amiga_x != newx)
        {
            if (mouse.x_movement()<0) { amiga_x = (amiga_x - 1) & 0x7fff; }
            else                      { amiga_x = (amiga_x + 1) & 0x7fff; }
            opencollector(SUBD_PIN2, (amiga_x&0x2) != 0);
            opencollector(SUBD_PIN4, ((amiga_x-1)&0x2) != 0);
        }
    
        if (amiga_y != newy)
        {
            if (mouse.y_movement()<0) { amiga_y = (amiga_y - 1) & 0x7fff; }
            else                      { amiga_y = (amiga_y + 1) & 0x7fff; }
            opencollector(SUBD_PIN1, (amiga_y&0x2) != 0);
            opencollector(SUBD_PIN3, ((amiga_y+1)&0x2) != 0);
        }
        delayMicroseconds(50);
    }
}


void detect_mode()
{
//    mode = MODE_JOYSTICK;  
    mode = MODE_AMIGAMOUSE;  
}

void init_output()
{
    switch (mode)
    {
        case MODE_JOYSTICK:   init_joystick();   break;
        case MODE_AMIGAMOUSE: init_amigamouse(); break;
    }      
}

void run_output()
{
    switch (mode)
    {
        case MODE_JOYSTICK:   run_joystick();   break;
        case MODE_AMIGAMOUSE: run_amigamouse(); break;
    }    
}


void setup()
{   
    mode = MODE_UNINITIALIZED;
}

void loop()
{
    unsigned long now = millis();

    if (mode==MODE_UNINITIALIZED)
    {
        if (mouse.initialise()!=0) { return; }
        
        detect_mode();      
        last_poll = now;        
        init_output();
        x = 0;
        y = 0;
        return;
    }

    last_poll = now;
    mouse.get_data();
    x = x + (unsigned int) mouse.x_movement();
    y = y + (unsigned int) mouse.y_movement();     
    run_output();
}
