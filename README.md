The file contents is for the light system for the TETFAIR Project ~ Pad Vending machine.

The Light System has two modes which include : Automatic and Manual
 1. Automatic :  Meaning once the system boots up the lights come up which is the normal running mode.
 2. Manual : This mode the lights won't come up will a signal is received ~ "Door Open signal".
    
To switch modes there's a Switch for this.
Also there's an LED indicator to know when the mode is switched.

Note : There are Two Different Neopixels strips one is the main lights for the machine and the other is one for the compartment box.
       The modes above are for the main lights.

The Second Neopixel strip is only triggered once the Pad falls. Using a HRTC5000 Sensor as the trigger and to detect when the pad falls.

***CODE FEAUTURES***

Using EEPROM : Electrically Erasable Programmable Read-Only Memory ~ This saves the last state of the lights in a event of maybe a power Outage.
Also it boots up from the last saved state ~ rather than changing mode all the time you can just set it to one and it remains there

Using Debounce : To avoid misclicks.
