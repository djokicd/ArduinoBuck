
//
//   ARDUINO SOURCE CODE
//
//   Danilo Đokić, djokicd@outlook.com
//
//   Buck converter, using I-regulator
//       - Pin 5  - OUTPUT - Going to switching element (inverted, P-type)
//       - Pin A0 - INPUT  - Feedback voltage, 1/2 Vout 
//       - Pin 7  - OUTPUT - Sync signal
//
//   Note:
//   Both pins 5 & 6 are implemented for PWM to prevent Arduino from triggering random interrupts.

bool state;

void setup() {
pinMode(5, OUTPUT);
pinMode(6,OUTPUT);
pinMode(7, OUTPUT);

// Setup for Timer0 interrupt
//  - Timer is used to output PWM modulated signal
TCCR0A=0;
TCCR0B=0;
TCCR0A=0b10100011;// Setting FAST PWM MODE
TCCR0B=0b00000001;// Setting prescaler
OCR0A=50; // Duty cycle for PWM on pin 6
OCR0B=150;// Duty cycle for PWM on pin 5

// Setup for Timer1 interrupt
//  - Used to implement control (or for testing)
cli(); // stop interrupts
TCCR1A = 0;
TCCR1B = 0;
TCNT1  = 0; // initialize counter value to 0
// set compare match register for 8000 Hz increments
OCR1A = 1999; // = 16000000 / (1 * 8000) - 1 (must be <65536)
// turn on CTC mode
TCCR1B |= (1 << WGM12);
// Set CS12, CS11 and CS10 bits for 1 prescaler
TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10);
// enable timer compare interrupt
TIMSK1 |= (1 << OCIE1A);
sei(); // allow interrupts

// Register to be set using the regulator.
OCR0B=64;
state = 0;
}

// Nothing happens in loop. Everything is triggered using interrupts.
void loop(){}

int flag = 0;

float Reading = 0;
float dt = 0.2e-3; // Sampling period (1 / 5kHz)
int CLK_DIV_max = 200; // 5 kHz
int clkdiv = 0;
float refference1 = 2.0;
float refference2 = 2.4;
float ref;

float vIN = 0;
float prevvIN = 0;
float vOUT = 0;
float prevvOUT = 0;


float vIN_int = 0;
float prevvIN_int = 0;
float vOUT_int = 0;
float prevvOUT_int = 0;

float c;
int control;

ISR(TIMER1_COMPA_vect){

   clkdiv++;


   if (clkdiv >= CLK_DIV_max){ // Setting the refference
     if (state)
     {
        ref = refference1;
        digitalWrite(7, HIGH);
     }
     else
     {
        ref = refference2;
        digitalWrite(7, LOW);
     }
     state = !state;

     clkdiv = 0;
   }

   // Regulator calculations
   
   Reading = analogRead(A0);

   prevvIN = vIN;
   vIN = (ref - 2*Reading * 5.00/1023.00 ); // Reading in volts

   vOUT_int += 200e-3 * vIN;

   OCR0B = int( 255-(vOUT_int/4.16 * 255) );
}
