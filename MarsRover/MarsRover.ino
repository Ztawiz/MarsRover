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
#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)      // 
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#define stepDelay 4                   // Delay between steps. Might be used as delay in our polling.
const float a = 317.18;                         // Distance in mm from turning axis to the forward motors.
const float b = 280.36;                         // Distance in mm from turning axis to the backward motors.
const float c = 263.5;                         // Distance in mm between the forward motors.
const float f = 335.3;                         // Distance in mm between the backward motors.

int16_t position[4] = {0,0,0,0};      // The current position of the stepper motors. {Left Forward, RF, LB, RB}

void getAim(int16_t *arr, int8_t turnd);

void setup() {
  DDRD |= B00111100;                  // High = Output. All StepPins output.
  DDRC |= B00001110;                  // All dirPins output. excpet A0 for pot testing
  DDRC &= B11111110;                  // A0, potPin as input for pot. (Pro mini)
  PORTD &= B11000011;                 // All step pins low.
  DDRB |= B00000010;                  // Pin 9 output for voltage to pot.
  PORTB |= B00000010;
  Serial.begin(9600);                 // Start serial communication at 9600 baud

}

void loop() {
  int16_t aim[4] = {0,0,0,0};                                         // The position we are aiming to turn towards.
  int8_t TurnDegree = map(analogRead(A0), 0, 1018, -45, 45);          // Reads A0 and maps it to suitable turning degrees. limited to 1018 do give bigger range for the 45 value.

  
  getAim(aim, TurnDegree);                                            // This is just a test radius before we start using the pot.
  
  debug("TurnDegree: ");
  debugln(TurnDegree);
  debug("#44 - aim 0 is: ");
  debugln(aim[0]);
  debug("#53 - aim 1 is: ");
  debugln(aim[1]);
  debug("#59 - aim 2 is: ");
  debugln(aim[2]);
  debug("#61 - aim 3 is: ");
  debugln(aim[3]);
  debug("analogA0 is: ");
  debugln(analogRead(A0));

  debugln(" ");

  delay(1000);
  

}

void getAim(int16_t *arr, int8_t turnd){

  if (turnd == 0){
    return;
  }

  uint16_t d = (uint16_t)(a / tan(abs(turnd) * PI / 180.0));                     // Calculate d for future calculations. Code gets too messy without this step. tan() only uses radians.
  debug("d: ");
  debugln(d);
 
  arr[0] = (int16_t)((turnd/0.9)*4);                                             // The currect position.
  
  arr[1] = (int16_t)((atan(a/(c+d))*(180.0/PI))/0.9*4);                          // The (hopefully) currect position.
  
  arr[2] = (int16_t)((atan(b/d)*(180.0/PI))/0.9*4);
  
  arr[3] = (int16_t)((atan(b/(f+d))*(180.0/PI))/0.9*4);

  
  if (turnd < 0){
    arr[1] = -arr[1];
  } else {
    arr[2] = -arr[2];
    arr[3] = -arr[3];
  }
  
  if (turnd > 0){
    int16_t temp = arr[0];
    arr[0] = arr[1];
    arr[1] = temp;

    temp = arr[2];
    arr[2] = arr[3];
    arr[3] = temp;
  }

}