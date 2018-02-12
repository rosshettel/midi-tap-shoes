
/***********HOBGOB ECLECTRONICS BeatSneaks code**************/

/*This code sends MIDI messages using electrical pulses from FSRs (Force Sensitive Resistors) placed in the heels and toes of a pair of sneakers.
It has been adapted from code by todbot, ladyada, spikenzielabs, and mschaff (ardrumo) - special thanks to X (wxs.ca)

You'll need to grab the Serial-MIDI Converter from spikenzielabs.com */
/************************************************************/

#include <MIDI.h>

#define HEELTHRESHOLD 450
#define TOETHRESHOLD 350
#define LOWERHEELTHRESHOLD 150
#define LOWERTOETHRESHOLD 101               // We're setting both a lower and an upper threshold for the FSRs, so that they will only trigger a MIDI message when we actually want them too.
#define HEEL1PIN 0
#define HEEL2PIN 1
#define TOE1PIN 2
#define TOE2PIN 3 
#define EXTRAPIN1 4
#define EXTRAPIN2 5                         // Define which analog in pins on the arduino we'll be using
int LEDpin = 13;                            // Connect LED to pin 13
int pinNote[6] = {78,65,88,59,41,42};       // This array stores the different MIDI notes that each pin is assigned
boolean heel1 = false;
boolean heel2 = false;
boolean toe1 = false;
boolean toe2 = false;                       //These variables will help us to make sure we don't send multiple MIDI messages per single hit

MIDI_CREATE_DEFAULT_INSTANCE();

void setup(void) 
{
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(57600);
  pinMode(LEDpin, OUTPUT);
}

void loop(void) 
{
  heel1 = sender(analogRead(HEEL1PIN), HEEL1PIN, pinNote[HEEL1PIN], "HEEL1", heel1, HEELTHRESHOLD, LOWERHEELTHRESHOLD);
  heel2 = sender(analogRead(HEEL2PIN), HEEL2PIN, pinNote[HEEL2PIN], "HEEL2", heel2, HEELTHRESHOLD, LOWERHEELTHRESHOLD);
  // We add some extra punch to the toe readings so that they'll sound about as loud as the heel
  toe1 = sender((analogRead(TOE1PIN)+100), TOE1PIN, pinNote[TOE1PIN], "TOE1", toe1, TOETHRESHOLD, LOWERTOETHRESHOLD);
  toe2 = sender((analogRead(TOE2PIN)+100), TOE2PIN, pinNote[TOE2PIN], "TOE2", toe2, TOETHRESHOLD, LOWERTOETHRESHOLD);         
}

// This function sends a MIDI message with a message, pitch, and velocity
void midimsg(unsigned char message, char sensor[], unsigned char pitch, unsigned char velocity) 
{
  // pitch, velocity, channel
  MIDI.sendNoteOn(pitch, velocity, 1);
  Serial.print(sensor + ": ");
  Serial.print(message);
  Serial.print(pitch);
  Serial.print(velocity);
  Serial.println();
}

// This function is what does all the work
boolean sender(int reading, int pin, int pitch, char sensor[], boolean press, int threshold, int lowerthreshold) 
{   
  if (reading >= threshold) {                 // If the user stomps harder than a certain threshold...
    if (!press) {                             // and if the sensor is not already in 'pressed' mode...
        reading = reading/8 - 1;              // convert the FSR reading to a MIDI-friendly value
        digitalWrite(LEDpin, HIGH);      
        midimsg(144, sensor, pitch, reading);         // Send a MIDI message
        delay(30);
        digitalWrite(LEDpin, LOW);
    }  
    press = true;                             // The sensor is now in 'pressed' mode,
  } else if(reading <= lowerthreshold) {      // once the applied force sinks below the lower threshold...
    press = false;                            // the sensor is no longer in 'pressed' mode
  }
  return press;
}

