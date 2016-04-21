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
const int STIMDELAY = 250;     // in ms, between stim and shutter
const int SHUTTERDELAY = 1000; // in µs, between shutter and length
const int STEPDELAY = 200;     // in µs, between length and camera
const int EXPOSUREDUR = 100;   // in µs, camera image duration
const int POSTEXPOSURE = 1000; // in ms, before returning to length

// Non-global variable declaration
int stepdelay = STEPDELAY;
bool run_now = false;

// ArduTimer Hardware hconfiguration
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
int LEN_OUT    = SIG1; // to length controller
int LEN_LED    = LED1;
int SHUT_OUT   = SIG4; // to quick shutter
int SHUT_LED   = LED4;
int EXPOSE_OUT = SIG5; // to exposure trigger
int EXPOSE_LED = LED5;
int STIM_OUT   = SIG3; // to stimulus trigger
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
    pinMode(LEN_OUT, OUTPUT);
    pinMode(LEN_LED, OUTPUT);
    pinMode(SHUT_OUT, OUTPUT);
    pinMode(SHUT_LED, OUTPUT);
    pinMode(EXPOSE_OUT, OUTPUT);
    pinMode(EXPOSE_LED, OUTPUT);
    pinMode(STIM_OUT, OUTPUT);
    pinMode(STIM_LED, OUTPUT);
    // Initial pin states
    turn_length(false);
    turn_stimulus(false);
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
        run(PREDELAY, STIMDELAY, SHUTTERDELAY, stepdelay, EXPOSUREDUR, POSTEXPOSURE);
        Serial.println("Run complete"); // tag that we've done a run
        run_now = false; // reset the run flag
    }
}

// Read data from serial, just the step-to-exposure delay
void check_serial()
{
    if(Serial.available() >= 2) {      // only when something is available
        stepdelay = Serial.parseInt(); // first int is delay
        Serial.println(stepdelay);     // echo back as sanity check
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
void run(int predelay, int stimdelay, int shutterdelay, int stepdelay, int exposureduration, int postexposure)
{
    delay(predelay);
    turn_stimulus(true);
    delay(stimdelay);
    turn_shutter(true);
    // shutter opened before step delay
    // may open during pre-delay in future as latency is established
    delayMicroseconds(shutterdelay);
    turn_length(true);
    delayMicroseconds(stepdelay);
    turn_exposure(true);
    delayMicroseconds(exposureduration);
    turn_stimulus(false);
    turn_exposure(false);
    turn_shutter(false);
    delay(postexposure);
    turn_length(false);
}

// Series of control functions
/* Each takes a state, true for on and false for off. On and off values for 
   each connection are set here. This is complicated since some need to be 
   high when on while others need to be high when off. To double complicate 
   things, I think that the transistor control shield on the ardutimer inverts
   the signal. 

   NOTE: THESE SHOULD BE DOUBLED CHECKED DURING SET UP.
*/

// Length control
void turn_length(boolean state){
    if(state == true){
        digitalWrite(LEN_OUT, HIGH); 
        digitalWrite(LEN_LED, HIGH);
    }
    else{
        digitalWrite(LEN_OUT, LOW); 
        digitalWrite(LEN_LED, LOW);
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

//Stimulus control
void turn_stimulus(boolean state){
    if(state == true){
        digitalWrite(STIM_OUT, LOW); 
        digitalWrite(STIM_LED, HIGH);
    }
    else{
        digitalWrite(STIM_OUT, HIGH); 
        digitalWrite(STIM_LED, LOW);
    }
}
