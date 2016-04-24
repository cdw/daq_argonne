/*  quick_release
    Control a quick release data acquisition run
    communicate over serial to start a run, taking as inputs
     - pre-delay (time to wait before changing length)
     - step-to-exposure delay
     - exposure duration     
     - post-exposure time before length is reset
    Created by CDW on 2014.12.02
*/


// Timing variables
const int PREDELAY  = 10;      // in ms, before stim goes on
const int ANTDELAY = 250;      // in ms, between whap start and shutter
const int SHUTTERDELAY = 1;    // in ms, between shutter and camera
const int EXPOSUREDUR = 1;     // in ms, camera image duration  TODO: SEND THIS ONE OVER SERIAL TOO
const int POSTEXPOSURE = 1000; // in ms, before returning to length

// Non-global variable declaration
int antdelay = ANTDELAY;
bool run_now = false;

// ArduTimer Hardware configuration
#define SIG1 2;
#define LED1 3;
#define SIG2 4;
#define LED2 5;
#define SIG3 6;
#define LED3 7;
#define SIG4 8;
#define LED4 9;
#define SIG5 10;
#define LED5 11;
#define SIG6 12;
#define LED6 13;


// Pin configuration
int ANT_OUT    = SIG2; // to ant whapper
int ANT_LED    = LED2;
int SHUT_OUT   = SIG4; // to quick shutter
int SHUT_LED   = LED4;
int EXPOSE_OUT = SIG5; // to exposure trigger
int EXPOSE_LED = LED5;
int STIM_IN    = SIG3; // to stimulus trigger
int STIM_LED   = LED3;

// Begin setup of board
void setup()
{
    // Initialize serial interface
    Serial.begin(115200); // Start serial connection
    while (!Serial) {
        ; // wait for serial (for Leonardo only)
    }
    Serial.println("Hi"); // Demonstrate serial connection
    // Set pin modes
    pinMode(ANT_OUT, OUTPUT);
    pinMode(ANT_LED, OUTPUT);
    pinMode(SHUT_OUT, OUTPUT);
    pinMode(SHUT_LED, OUTPUT);
    pinMode(EXPOSE_OUT, OUTPUT);
    pinMode(EXPOSE_LED, OUTPUT);
    pinMode(STIM_IN, INPUT);
    pinMode(STIM_LED, OUTPUT);
    // Initial pin states
    turn_ant(false);
    turn_shutter(false);
    turn_exposure(false);
    // Close out setup
    Serial.println("Setup complete");
}

// Loop through
void loop()
{
    check_serial(); // read input if it exists
    if (run_now) {
        run(PREDELAY, antdelay, SHUTTERDELAY, EXPOSUREDUR, POSTEXPOSURE);
        Serial.println("Run complete"); // tag that we've done a run
        run_now = false; // reset the run flag
    }
}

// Read data from serial, just the step-to-exposure delay
void check_serial()
{
    if(Serial.available() >= 2) {      // only when something is available
        antdelay = Serial.parseInt(); // first int is delay
        Serial.println(antdelay);     // echo back as sanity check
        char ending = (char)Serial.read(); 
        if (ending == '\n') {          // ignore out-of-format commands
            run_now = true;
        }
        else {
            Serial.println("Command ignored: didn't end in newline");
        }
    }
}

// Control a run
void run(int predelay, int antdelay, int shutterdelay, int exposuredur, int postexposure)
{
//    delay(predelay);      // just to let things settle
//    turn_ant(true);       // whap the ant
    while(!digitalRead(STIM_IN)){
      delayMicroseconds(2);
    }
    delay(antdelay);      // wait for motor movement
    turn_shutter(true);   // tell the shutter to open
    delay(shutterdelay);  // let the shutter physically respond
    turn_exposure(true);  // tell the camera to take a pic
    delay(exposuredur);    // let the camera take a pic
    turn_shutter(false);  // tell the shutter to close
    delay(postexposure);  // let the muscle rest
    turn_exposure(false); // reset cam trigger
    turn_ant(false);      // reset whap
//turn_ant(true);
//delay(250);
//turn_shutter(true);
//delay(100);
//turn_exposure(true);
//delay(100);
//turn_exposure(false);
//turn_shutter(false);
//turn_ant(false);
}

// Series of control functions
/* Each takes a state, true for on and false for off. On and off values for 
   each connection are set here. This is complicated since some need to be 
   high when on while others need to be high when off. To double complicate 
   things, I think that the transistor control shield on the ardutimer inverts
   the signal. 

   NOTE: THESE SHOULD BE DOUBLED CHECKED DURING SET UP.
*/

// Ant control
void turn_ant(boolean state){
    if(state == true){
        digitalWrite(ANT_OUT, HIGH); 
        digitalWrite(ANT_LED, HIGH);
    }
    else{
        digitalWrite(ANT_OUT, LOW); 
        digitalWrite(ANT_LED, LOW);
    }
}

// Shutter control
void turn_shutter(boolean state){
    if(state == true){
        digitalWrite(SHUT_OUT, LOW); 
        digitalWrite(SHUT_LED, HIGH);
    }
    else{
        digitalWrite(SHUT_OUT, HIGH); 
        digitalWrite(SHUT_LED, LOW);
    }
}

//Exposure control
void turn_exposure(boolean state){
    if(state == true){
        digitalWrite(EXPOSE_OUT, LOW); 
        digitalWrite(EXPOSE_LED, HIGH);
    }
    else{
        digitalWrite(EXPOSE_OUT, HIGH);
        digitalWrite(EXPOSE_LED, LOW);
    }
}

