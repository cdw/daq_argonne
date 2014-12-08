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
const int PREDELAY  = 10;     // in ms
const int STIMDELAY = 200;    // in ms
const int SHUTTERDELAY = 3;   // in ms
const int STEPDELAY = 200;    // in µs
const int EXPOSUREDUR = 500;  // in µs
const int POSTEXPOSURE = 100; // in ms

// Non-global variable declaration
int stepdelay = STEPDELAY;
bool run_now = false;

// Pin configuration
int LEN_OUT    = 2; // to length controller
int LEN_LED    = 3;
int SHUT_OUT   = 4; // to quick shutter
int SHUT_LED   = 5;
int EXPOSE_OUT = 6; // to exposure trigger
int EXPOSE_LED = 7;
int STIM_OUT   = 8; // to stimulus trigger
int STIM_LED   = 9;

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
    digitalWrite(LEN_OUT, LOW);
    digitalWrite(LEN_LED, LOW);
    digitalWrite(SHUT_OUT, LOW);
    digitalWrite(SHUT_LED, LOW);
    digitalWrite(EXPOSE_OUT, LOW);
    digitalWrite(EXPOSE_LED, LOW);
    digitalWrite(STIM_OUT, LOW);
    digitalWrite(STIM_LED, LOW);
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
    }
}

// Control a run
void run(int predelay, int stimdelay, int shutterdelay, int stepdelay, int exposureduration, int postexposure)
{
    delay(predelay);
    digitalWrite(STIM_OUT, HIGH);
    digitalWrite(STIM_LED, HIGH);
    delay(stimdelay);
    digitalWrite(SHUT_OUT, HIGH); // shutter opened before step delay
    digitalWrite(SHUT_LED, HIGH); // may open during pre-delay in future
    delay(shutterdelay);
    digitalWrite(LEN_OUT, HIGH);
    digitalWrite(LEN_LED, HIGH);
    delayMicroseconds(stepdelay);
    digitalWrite(EXPOSE_OUT, HIGH);
    digitalWrite(EXPOSE_LED, HIGH);
    delayMicroseconds(exposureduration);
    digitalWrite(STIM_OUT, LOW);
    digitalWrite(STIM_LED, LOW);
    digitalWrite(EXPOSE_OUT, LOW);
    digitalWrite(EXPOSE_LED, LOW);
    digitalWrite(SHUT_OUT, LOW);
    digitalWrite(SHUT_LED, LOW);
    delay(postexposure);
    digitalWrite(LEN_OUT, LOW);
    digitalWrite(LEN_LED, LOW);
}
