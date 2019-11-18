/*
 * 2019-11-18
 * 
 * Arduino Mega 2560 ADK
 * Adafruit Music Maker Shield (PID: 1788)
 * Adafruit MAX9744 20W Amplifier (PID: 1752)
 *  
 * ADK controls the 1788 via SPI
 * The 1788 stores the audio files on an SD card for access
 * 
 */

//============  Includes / Defines  ============
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// ---- Music Maker Shield ----
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7       // VS1053 chip select pin (output)
#define SHIELD_DCS    6       // VS1053 Data/command select pin (output)
#define CARDCS 4              // Card chip select pin
#define DREQ 3                // VS1053 Data request, DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt


//============  Module Configuration  ============
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

//============  Constants  ============
// --------- Arrays ---------
const int soundFiles[] = {"001.mp3","002.mp3","003.mp3","004.mp3","005.mp3","006.mp3","007.mp3","008.mp3","009.mp3"};

// --------- Integers ---------
const int soundList = 9;
const int errorDelay = 100;

//============  VARIABLES (will change)  ============
// --------- Integers ---------
int file = 0;
int count = 0;
int8_t volume = 20;

//============  Setup function  ============
void setup() {
  // ---- Serial Setup for Monitior ----
  Serial.begin(9600);
  Serial.println("Adafruit VS1053");

  // ---- Pin Setup ----
  pinMode(LED_BUILTIN, OUTPUT);

  // ---- Initialize and check for VS1053 ----
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1){
      ledError(2);
     }
  }
  Serial.println(F("VS1053 found"));
  
  // ---- Initialize and check SD Card ----
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1)while (1){
      ledError(3);
     }
  }

  // ---- Initialize and check Interrupt Pin ----
  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT)){
    Serial.println(F("DREQ pin is not an interrupt pin"));
    while (1)while (1){
      ledError(4);
    }
  }
  
  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20,20);
    
  // Play one file, don't return until complete
  Serial.println(F("Playing Startup track"));
  musicPlayer.playFullFile("/startup.mp3");
}

//============  Loop function  ============
void loop() {
  if(count < 20){
    file = random(0,10);
    Serial.println(F("Playing Track"));
    musicPlayer.playFullFile(soundFiles[file]);
  }
  delay(1000);
  count++;
}

//============  File listing helper  ============
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

//============  Onboard LED Error Flash  ============
/*
 * This sub routine flashes the onboard LED in an error state.  The flash is a quick pulse, followed by a delay.
 * 2 pulses = Arduino cannot find the VS1053
 * 3 pulses = the arduino cannot read the SD card
 * 4 pulses = The pin set to be the interrupt pin is not an interrupt pin - reconfigure
 */
void ledError(int steps) {
  for (int i = 0; i <= steps; i++){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(errorDelay);
    digitalWrite(LED_BUILTIN, LOW);
    delay(errorDelay);
  }
  delay(errorDelay*3);
}
