#include <Arduino.h>

#include "PS2MouseHandler.h"

PS2MouseHandler::PS2MouseHandler(int clock_pin, int data_pin, int mode) {
  _clock_pin = clock_pin;
  _data_pin = data_pin;
  _mode = mode;
  _initialised = false;
  _disabled = true;
  _enabled = false;
  _device_id = 0x00;
  _no_mouse = false;
  _last_status = 0; // will hold copy of status byte
}

int PS2MouseHandler::clock_pin() {
  return _clock_pin;
}

int PS2MouseHandler::data_pin() {
  return _data_pin;
}

int PS2MouseHandler::device_id () {
  return _device_id;
}

int PS2MouseHandler::status () {
  return _status;
}

int PS2MouseHandler::x_movement () {
  return _x_movement;
}

int PS2MouseHandler::y_movement () {
  return _y_movement;
}

int PS2MouseHandler::z_movement () {
  return _z_movement;
}

bool PS2MouseHandler::button(int button_num){
  return (bool)( _status & get_button_mask(button_num));
}

bool PS2MouseHandler::clicked(int button_num){
  if (button(button_num) && !(bool)( _last_status & get_button_mask(button_num))) {
    return true;
  }
  else {
    return false;
  }
}

uint8_t PS2MouseHandler::get_button_mask(int button){
  uint8_t mask = 0;
  switch(button){
    case 0: // left
      mask = 0x01;
      break;
    case 1: // middle
      mask = 0x04;
      break;
    case 2: // right
      mask = 0x02;
      break;
    default:
      mask = 0;
      break;
  }
  return mask;
}

int PS2MouseHandler::initialise() {

  int counter = 0;
  int return_value = 0;
  // poll mouse to get a connection
  do {
    return_value = try_initialise();
    counter ++;
  } while ((return_value != 0) && (counter < 10));
  return return_value;
}

int PS2MouseHandler::try_initialise() {
  pull_high(_clock_pin); // idle state
  pull_high(_data_pin);
  write(0xff); // Send Reset to the mouse
  if (_no_mouse){
    return 100; // no mouse
  }
  uint8_t bat_code = read_byte();  // Read BAT completion code
  if (bat_code == 0xFC){ // error?
    _no_mouse = true;
    return 101; // BAT failed
  }
  read_byte();  // Device ID - ignore for now - should be 0x00 for mouse

  // set scroll wheel mode if available
  set_sample_rate(200, true);
  set_sample_rate(100, true);
  set_sample_rate(80, true);
  _device_id = get_device_id(); // 0x00 = no scroll wheel

  if (_mode == PS2_MOUSE_REMOTE) {
    set_remote_mode();
  } else {
    enable_data_reporting(); // Tell the mouse to start sending data again
  }
  delayMicroseconds(100);
  _initialised = 1;
  get_data(); // initial read of mouse data
  _last_status = _status; // suppress false button presses
  return 0; // OK
}

void PS2MouseHandler::set_mode(int data) {
  if (_mode == PS2_MOUSE_STREAM) {
    disable_data_reporting(); // Tell the mouse to stop sending data.
  }
  write(data);  // Send Set Mode
  read_byte();  // Read Ack byte
  if (_mode == PS2_MOUSE_STREAM) {
    enable_data_reporting(); // Tell the mouse to start sending data again
  }
  if (_initialised) {
    delayMicroseconds(100);
  }
}

int PS2MouseHandler:: get_device_id(){
  write(0xf2); // Ask mouse for device ID.
  int id =  read_byte(); // Read first byte - gives overall device id
  read_byte(); // try to read second byte if sent - refines device type - not needed
  return id;
}

void PS2MouseHandler::set_remote_mode() {
  set_mode(0xf0);
  _mode = PS2_MOUSE_REMOTE;
}

void PS2MouseHandler::set_stream_mode() {
  set_mode(0xea);
  _mode = PS2_MOUSE_STREAM;
}

void PS2MouseHandler::set_sample_rate(int rate, bool ignore_sample_rate) {
  if (_mode == PS2_MOUSE_STREAM && !ignore_sample_rate) {
    disable_data_reporting(); // Tell the mouse to stop sending data.
  }
  write(0xf3); // Tell the mouse we are going to set the sample rate.
  read_byte(); // Read Ack Byte
  write(rate); // Send Set Sample Rate
  read_byte(); // Read ack byte
  if (_mode == PS2_MOUSE_STREAM && !ignore_sample_rate) {
    enable_data_reporting(); // Tell the mouse to start sending data again
  }
  delayMicroseconds(100);
}

void PS2MouseHandler::set_scaling_2_1() {
  set_mode(0xe7); // Set the scaling to 2:1
}

void PS2MouseHandler::set_scaling_1_1() {
  set_mode(0xe6); // set the scaling to 1:1
}

// This only effects data reporting in Stream mode.
void PS2MouseHandler::enable_data_reporting() {
  if (!_enabled) {
    write(0xf4); // Send enable data reporting
    read_byte(); // Read Ack Byte
    _enabled = true;
  }
}

// Disabling data reporting in Stream Mode will make it behave like Remote Mode
void PS2MouseHandler::disable_data_reporting() {
  if (!_disabled) {
    write(0xf5); // Send disable data reporting
    read_byte(); // Read Ack Byte
    _disabled = true;
  }
}

void PS2MouseHandler::set_resolution(int resolution) {
  if (_mode == PS2_MOUSE_STREAM) {
    enable_data_reporting();
  }
  write(0xe8); // Send Set Resolution
  read_byte(); // Read ack Byte
  write(resolution); // Send resolution setting
  read_byte(); // Read ack Byte
  if (_mode == PS2_MOUSE_STREAM) {
    disable_data_reporting();
  }
  delayMicroseconds(100);
}

void PS2MouseHandler::write(int data) {
  char i;
  char parity = 1;
  pull_high(_data_pin);
  pull_high(_clock_pin);
  delayMicroseconds(300);
  pull_low(_clock_pin);
  delayMicroseconds(300);
  pull_low(_data_pin);
  delayMicroseconds(10);
  pull_high(_clock_pin); // Start Bit
  // wait for mouse to take control of clock
  if (!wait_for_pin(_clock_pin, LOW, 100)) {
      // no connection to mouse
      pull_high(_data_pin); // back to waiting
      _no_mouse = true;
      return;
  } 
  _no_mouse = false; // connection to mouse OK
  // clock is low, and we are clear to send data
  for (i=0; i < 8; i++) {
    if (data & 0x01) {
      pull_high(_data_pin);
    } else {
      pull_low(_data_pin);
    }
    // wait for clock cycle
    while (!digitalRead(_clock_pin)) {;}
    while (digitalRead(_clock_pin)) {;}
    parity = parity ^ (data & 0x01);
    data = data >> 1;
  }
  // parity
  if (parity) {
    pull_high(_data_pin);
  } else {
    pull_low(_data_pin);
  }
  // wait for clock cycle
  while (!digitalRead(_clock_pin)) {;}
  while (digitalRead(_clock_pin)) {;}
  pull_high(_data_pin); // release data line
  while (digitalRead(_data_pin)) {;} // wait for mouse to take over data line
  while (digitalRead(_clock_pin)) {;} // wait for mouse to take over clock
  while ((!digitalRead(_clock_pin)) && (!digitalRead(_data_pin))) {;} // wait for mouse to release clock and data
}

void PS2MouseHandler::hold_incoming_data(){
  pull_low(_clock_pin);
}

void PS2MouseHandler::get_data() {
  _last_status = _status; // save copy of status byte
  write(0xeb); // Send Read Data
  read_byte(); // Read Ack Byte
  _status = read(); // Status byte
  _x_movement = read_movement_9(bitRead(_status, 4)); // X Movement Packet
  _y_movement = read_movement_9(bitRead(_status, 5)); // Y Movement Packet
  if (_device_id > 0){
    // read scroll wheel
    _z_movement = read_movement_z(); // Z Movement Packet
  }
  else {
    _z_movement = 0;
  };
}

uint8_t PS2MouseHandler::read() {
  return read_byte();
}

uint8_t PS2MouseHandler::read_byte() {
  uint8_t data = 0;
  pull_high(_clock_pin);
  pull_high(_data_pin);
  delayMicroseconds(50);

  // read start bit but check for timeout
  if (!wait_for_pin(_clock_pin, LOW, 100)) {
      return 0;
  }
  // delayMicroseconds(5);
  while (!digitalRead(_clock_pin)) {;}
  // read data bits
  for (int i = 0; i < 8; i++) {
    bitWrite(data, i, read_bit());
  }
  read_bit(); // Partiy Bit
  read_bit(); // Stop bit should be 1
  return data;
}

int PS2MouseHandler::read_bit() {
  while (digitalRead(_clock_pin)) {;}
  int bit = digitalRead(_data_pin);
  while (!digitalRead(_clock_pin)) {;}
  return bit;
}

int16_t PS2MouseHandler::read_movement_9(bool sign_bit) {
  // movement data is a 9 bit signed int using status bit and data reading
  int16_t value = read();
  // use status bit to get sign of reading
  if (sign_bit) {
    // fill upper byte with 1's for negative number
    value |= 0xFF00;
  }
  return value;
}

int8_t PS2MouseHandler::read_movement_z() {
  // z data can be mixed with extra button data so only the lower nibble is movement
  // 4 bit signed
  uint8_t value = read(); // an 8 bit unsigned value
  // test bit 3 for sign
  if(bitRead(value, 3)){
    // negative - set upper nibble to 1's
    value |= 0xF0;
  }
  else {
    // positive - set upper nibble to 0's
    value &= 0x0F;
  }
  return value;
}

void PS2MouseHandler::pull_low(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void PS2MouseHandler::pull_high(int pin) {
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

bool PS2MouseHandler::wait_for_pin(int pin, int val, int timeout)
{
    int t;
    for (t=0; t<timeout; t++) {
        int i;
        // number of tries is tuned to use approximately 1 ms
        for (i=0; i<500; i++) {
            if (digitalRead(pin)==val) { return true; }
        }
    }
    return false;
}
