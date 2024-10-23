/*

This is a test program for the Mars Rover to see if the stepper motors, its drivers and all pereferals work as intended...

#define VFdirPin PC0 A0           // HIGH = Clockwise = Vänstersväng pga växel. LOW blir högersväng
#define HFdirPin PC1 A1           //
#define VBdirPin PC2 A2           // HIGH = Högersväng då den roterar motsatt främre motorerna.
#define HBdirPin PC3 A3            // HIGH = Högersväng. LOW = Vänstersväng.
#define PotPin   PC5 A5           // Input for potentiometer.

#define VFstepPin PD2             // Pin 2
#define HFstepPin PD3             // Pin 3 etc...
#define VBstepPin PD4
#define HBstepPin PD5

The scope for d is 317 mm to 1700 mm, which is 1 degree to 45 degrees for the forward inner wheel.
*/

#define stepDelay 4
#define turnValue 400

uint16_t position[4] = {0,0,0,0};   // The current position of the stepper motors. {Left Forward, RF, LB, RB}
uint16_t aim[4] = {0,0,0,0};        // The position we are aiming to turn towards.


void setup() {
  DDRD |= B00111100;                // High = Output. All StepPins output.
  DDRC |= B00001111;                // All dirPins output.
  DDRC &= B11101111;                // A5, potPin as input.
  PORTD &= B11000011;               // All step pins low.

}

void loop() {
  // put your main code here, to run repeatedly:

}
