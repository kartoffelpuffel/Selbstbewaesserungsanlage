const int moisturePin = 1;
const int relayPin = 9;

const int dry = 530;
const int wet = 227;
const int moisturetarget = 40;
const float volume = 500;
const float durchflussrate = 18; //in ml/s

byte adcsra_save = 135;

// This variable is made volatile because it is changed inside
// an interrupt function
volatile int f_wdt=1;

//debug
int start = 0;


// Watchdog Interrupt Service. This is executed when watchdog timed out.
ISR(WDT_vect) {
	if(f_wdt == 0) {
		// here we can implement a counter the can set the f_wdt to true if
		// the watchdog cycle needs to run longer than the maximum of eight
		// seconds.
		f_wdt=1;
	}
}







void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  setWatchdogFor(1);

  pinMode(moisturePin, INPUT);
	pinMode(relayPin, OUTPUT);
  
}




void loop() {
	if(f_wdt != 1) {
		return;
	}

	// clear the flag so we can run above code again after the MCU wake up
	f_wdt = 0;
  

	int sensorval = analogRead(moisturePin);
	int moisturenow = map(sensorval, dry, wet, 0, 100);
	moisturenow = constrain(moisturenow, 0, 100);

	float wasserpumpen = float((moisturetarget - moisturenow))/100 * volume;

	wasserpumpen *= 1.1;


	if(wasserpumpen > 0){
		int pumptime = (int)(wasserpumpen/durchflussrate)+0.5f;
		digitalWrite(relayPin,HIGH);
		sleepFor(pumptime);
		digitalWrite(relayPin,LOW);
	}

	// 	disable ADC
	ADCSRA = 0;

	sleepFor(4);

	ADCSRA = adcsra_save;
}
