#include <PS2MouseHandler.h>
#define MOUSE_DATA 5
#define MOUSE_CLOCK 6

PS2MouseHandler mouse(MOUSE_CLOCK, MOUSE_DATA, PS2_MOUSE_REMOTE);

/**
 * Setup
 */
void setup()
{
//  Serial.begin(38400);
  if(mouse.initialise() != 0){
    // mouse error
//    Serial.println("mouse error");
  };

//  Serial.println(mouse.device_id());
}

/**
 * Main program loop
 */

unsigned long last_run = millis();

void loop()
{
  if (millis() - last_run > 200) {
    last_run = millis();
    mouse.get_data();
//    Serial.print(mouse.device_id()); // device id
//    Serial.print(":");
//    Serial.print(mouse.status()); // Status Byte
//    Serial.print(":");
//    Serial.print(mouse.x_movement()); // X Movement Data
//    Serial.print(",");
//    Serial.print(mouse.y_movement()); // Y Movement Data
//    Serial.print(",");
//    Serial.print(mouse.z_movement()); // Z Movement Data - scroll wheel
//    Serial.print(",");
//    Serial.print(mouse.button(0));
//    Serial.print(",");
//    Serial.print(mouse.button(1));
//    Serial.print(",");
//    Serial.print(mouse.button(2));
//    Serial.print(",");
//    Serial.print(mouse.clicked(0));
//    Serial.print(",");
//    Serial.print(mouse.clicked(1));
//    Serial.print(",");
//    Serial.print(mouse.clicked(2));
//    Serial.println();
  }
}
