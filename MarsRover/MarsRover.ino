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

String extractFilename(const char* path) {            // Print the name of the file currently running on the Arduino to the serial monitor.
const char* filename = strrchr(path, '\\');
if (filename != nullptr) { 
  filename++; 
}
return String(filename); 
}
String version = extractFilename(__FILE__);

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)      // This way we can remove all serial prints easily. Thanks, pre-processor.
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

struct Positions{
  int front_left_motor;
  int front_right_motor;
  int back_left_motor;
  int back_right_motor;
  int currentAngle;

};
Positions positions;


void getAim(int *arr, int turnd);
void stepMotors(int turnd);
// int findMax(int a, int b, int c, int d);

void setup() {
  DDRD |= B00111100;                  // High = Output. All StepPins output.
  DDRC |= B00001110;                  // All dirPins output. excpet A0 for pot testing.(Pro mini)
  DDRC &= B11111110;                  // A0, potPin as input for pot. (Pro mini)
  PORTD &= B11000011;                 // All step pins low.
  DDRB |= B00000010;                  // Pin 9 output for voltage to pot. (Pro mini)
  PORTB |= B00000010;
  Serial.begin(9600);                 // Start serial communication at 9600 baud
  Serial.println(version);

  positions.front_left_motor = 0;
  positions.front_right_motor = 0;
  positions.back_left_motor = 0;
  positions.back_right_motor = 0;
  positions.currentAngle = 0;
}

void loop() {
  int targetAngle = map(analogRead(A0), 0, 1018, -45, 45);          // Reads A0 and maps it to suitable turning degrees. limited to 1018 do give bigger range for the 45 value.
                                                                      // This is the turning degree the pot tells us to go.
  
  
  stepMotors(targetAngle);

  
  delay(1000);

}

void getAim(int *arr, int turnd){                                             // turnd is now positions.currentAngle-1 or +1, which is the radius were aiming for, one degree from current position.

  if (turnd == 0){                                                            // If turnd is 0, we return without changing aim[], which leaves it at {0,0,0,0}.
    return;
  }

  uint16_t d = (int)(a / tan(abs(turnd) * (PI / 180.0)));                     // Calculate d for future calculations. Code gets too messy without this step. tan() only uses radians.
  debug("d: ");                                                               
  debugln(d);
  arr[0] = (int)((turnd/0.9)*4);                                             // The currect position.
  arr[1] = (int)((atan(a/(c+d))*(180.0/PI))/0.9*4);                          // The (hopefully) currect position.
  arr[2] = (int)((atan(b/d)*(180.0/PI))/0.9*4);
  arr[3] = (int)((atan(b/(f+d))*(180.0/PI))/0.9*4);
  
  if (turnd < 0){                                                           // If turnd < 0, we are turning further left.
    arr[1] = -arr[1];                                                       // 
  } else {
    arr[2] = -arr[2];
    arr[3] = -arr[3];
  }
  
  if (turnd > 0){                                                           // If turnd > 0, we are turning further right. 
    int temp = arr[0];
    arr[0] = arr[1];
    arr[1] = temp;

    temp = arr[2];
    arr[2] = arr[3];
    arr[3] = temp;
  }
}

void stepMotors(int turnd){
  int aim[4] = {0,0,0,0};                                             // The position we are aiming to turn towards.

  if (turnd < positions.currentAngle){                                // If targetAngle is less than currentAngle, we need decrease our angle by 1 and thus need the aim for currentAngle-1.
    getAim(aim, positions.currentAngle-1);                            // aim is sent as a pointer for getAim to update.
  } else if (turnd > positions.currentAngle){
    getAim(aim, positions.currentAngle+1);
  } else return;                                                      // If turnd is the same as currentAngle, we dont need to do anything.

  
  if (turnd < positions.currentAngle){                                // If targetAngle is less than currentAngle, we need to turn left.
    PORTC |= B00000011;                                               // Forward motors LOW for left turn.
    PORTC &= B11110011;                                               // Back motors HIGH for right turn.
  }

  if (turnd > positions.currentAngle){                                // If targetAngle is greater than currentAngle, we need to turn right.
    PORTC &= B11111100;                                               // FOrward motors LOW for right turn.
    PORTC |= B00001100;                                               // Back motors HIGH for right turn.
  }

  if (turnd != positions.currentAngle){                               // If were not aiming for our current position, we need to figure out how many steps each motor needs to take.
    int numberOfSteps[4] = {0, 0, 0, 0};
    numberOfSteps[0] = abs(aim[0] - positions.front_left_motor);
    numberOfSteps[1] = abs(aim[1] - positions.front_right_motor);
    numberOfSteps[2] = abs(aim[2] - positions.back_left_motor);
    numberOfSteps[3] = abs(aim[3] - positions.back_right_motor);
    
    
    for (int i = 0; i < 4; i++){                                        // Pin2 (VF) steps numberOfSteps[0] times, Pin3 (HF) steps numberOfSteps[1] times etc.
      for (int j = numberOfSteps[i]; j > 0; j--){
      //digitalWrite(i+2, HIGH);
      //delay(stepDelay);
      //digitalWrite(i+2, LOW);
      //delay(stepDelay);

      if (turnd < positions.currentAngle){                          // Were turning left, front motor positions decrease and back motor positions increase.
        switch (i) {
          case 0: positions.front_left_motor--; break;
          case 1: positions.front_right_motor--; break;
          case 2: positions.back_left_motor++; break;
          case 3: positions.back_right_motor++; break;
        }
      }

        if (turnd > positions.currentAngle){                          // Were turning right, front motor positions increase and back motor positions decrease.
          switch (i) {
            case 0: positions.front_left_motor++; break;
            case 1: positions.front_right_motor++; break;
            case 2: positions.back_left_motor--; break;
            case 3: positions.back_right_motor--; break;
            }
        }

      }
    }
    if (turnd < positions.currentAngle){
      positions.currentAngle--;
    } else if (turnd > positions.currentAngle){
      positions.currentAngle++;
    }
    
  }
  
  // int max = findMax(aim[0], aim[1], aim[2], aim[3]);
  
                                                                    // NEXT TIME! Find a way to step the bloody motors!  
                                                                    // 1 steg: Sätt ettan på trunkerad plats av (max+1))/2. 
                                                                    // 2 steg: OM 2 max: 1, 1. Om 3 max: 1, 2. annars; 2, 4.
                                                                    // 
                                                                    // Uppdatera positioner varje varv i main loop för att simulera körning?

  debug("currentAngle: ");
  debugln(positions.currentAngle);
  
  debug("targetAngle: ");
  debugln(turnd);

  debug("front left is: ");
  debugln(positions.front_left_motor);
  
  debug("aim 0 is: ");
  debugln(aim[0]);

  debug("front right is: ");
  debugln(positions.front_right_motor);

  debug("aim 1 is: ");
  debugln(aim[1]);

  debug("back left is: ");
  debugln(positions.back_left_motor);

  debug("aim 2 is: ");
  debugln(aim[2]);

  debug("back right is: ");
  debugln(positions.back_right_motor);

  debug("aim 3 is: ");
  debugln(aim[3]);

  debug("analogA0 is: ");
  debugln(analogRead(A0));

  debugln(" ");

}
/*
int findMax(int a, int b, int c, int d) {
    int maxVal = a;

    if (b > maxVal) maxVal = b;
    if (c > maxVal) maxVal = c;
    if (d > maxVal) maxVal = d;

    return maxVal;
} */