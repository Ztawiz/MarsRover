/*

This is a test program for the Mars Rover to see if the stepper motors, its drivers and all pereferals work as intended...

#define VFdirPin PC0 A0           // HIGH = Clockwise = Vänstersväng pga växel. LOW blir högersväng
#define HFdirPin PC1 A1           //
#define VBdirPin PC2 A2           // HIGH = Högersväng då den roterar motsatt främre motorerna.
#define HBdirPin PC3 A3            // HIGH = Högersväng. LOW = Vänstersväng.

#define VFstepPin PD2             // Pin 2
#define HFstepPin PD3             // Pin 3 etc...
#define VBstepPin PD4
#define HBstepPin PD5
#define SleepPIn PD6              // För att motorerna ej ska starta i förväg sätter vi denna till Sleep och den måste bli hög innan vi kan köra.
*/

#define stepDelay 4
#define turnValue 400

int turn = turnValue;                     // Simulera radiosignal
int position = (turnValue/2);
int update = 0;

void turnLeft(void);
void turnRight(void);

void setup() {

  
  DDRD |= B00111100;                // Hög = Output. Alla StepPins output.
  DDRC |= B00001111;                // Alla dirPins output.
  PORTD &= B11000011;               // Alla step pins
}

void loop() {
  // Turn måste hämtas innan detta körs, så den inte ändras efter att update ändrats.
  
  if( turn > position ){          // Vi ska åt höger, dvs framhjul LOW, bakhjul HIGH.
    turnRight();
    update = 1;
  }
  if( turn < position ){
    turnLeft();
    update = -1;
  }
  if ( turn != position){
    PORTD |= B00111100;             // StepPin 2,3,4 och 5 hög.
    delay(stepDelay);
    PORTD &= B11000011;             // StepPin 2,3,4 och 5 låg.
    delay(stepDelay);
    position += update;
    update = 0;
  } else {turn = 0;}
  if ( position == 0){
    turn = turnValue;
  }
}

void turnLeft(void){
  PORTC |= B00000011;             // Fram-motorer HIGH för vänstersväng
  PORTC &= B11110011;             // bak-motorer LOW för västersväng
}

void turnRight(void){
  PORTC &= B11111100;             // Fram-motorer LOW för högersväng
  PORTC |= B00001100;             // Bak-motorer HIGH för högersväng
}