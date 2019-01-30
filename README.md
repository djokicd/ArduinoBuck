# Buck converter / Hardware

Nice demo of the control system of the Buck DC/DC convertor implemented using Arduino UNO.

## System realization
Schematic of this implementation is in /sch.

### Components
* Diode D - 1N5819 (Schottky barrier).
* Load resistor Rp - 5x 33R/1W in parallel
* MOSFET M - IRF9520
* Capacitor C - 2200uF/25V
* Coil L - 5÷50 mH
* Arduino UNO
* Resistor R0 - 2x 1K/0.25W, for voltage divider

Arduino connections shoud be:
* A0 - Analog input pin, output voltage feedback via the voltage divider 
* 5 - PWM out, for the gate of the switching MOSFET
* 7 - Sync out, for oscilloscope sync (eg. EXT TRIG.)


### Setting the internal timers
Arduino should (a) realize PWM of a given duty cycle & (b) calculate controller output and set the duty ratio. For use of the Ardino as a functional 
PWM generator the function **analogWrite** cannot be used, because of the small PWM default frequency. It is necessary to set up registers of the internal 
Timer 0 of the controller. Setting up the registers can be done using a suitable calcultaror (easily found online). Frequency of PWM should be set to 
62.5 kHz. 

Second timer, **TMR1**, shoud be set up to frequency of 4 kHz, for this the same calculator may be used. It is required to set up interrupt routine which is 
triggered by TMR1, which then is used to configure 8-bit register **OCR0B** which then sets up the PWM duty ratio. This implies that the PWM duty ratio
can be set to accuracy of 0.39%. 

### Input voltage
Due to the large change in duty ratio the converter may enter the _discontinous conduction mode_ when the system is nonlinear. This can be avoided 
by increasing the coil size and with **slow changes on the input**. It is recommended for measuring different system responses that the output voltage
should not change no more than 10% of the supply voltage.


### Component values
Resistance of the load can be 6.6Ω, which can be obtained by connecting five of the 33Ω in paralle. In that case, with the supply voltage of 6V maximum
dissipation on the load will be 6W which should be taken to consideration. Coil inductance can be 45 mH. System should resist changes in supply voltage 
in the range of Vg = 5÷8 V. 

### Control
Controller shoud achieve _loop shaping_. As an example, in /src/i_regulator.ino is the implementation of the I-regulator for this system 

### Other notes
- It is not the best of ideas to connect the output of the Arduino to the Gate of the MOSFET. The arduino can get damaged if, for some reason, the gate of the MOSFET breaks down. However, it has been shown to work in this setup. 
- The power MOSFET **can** overheat and stop working.
- Load resistor shouldn'g be able to overheat, however it may get quite hot!


