#include "Arduino.h"
#include "RTClib.h"
#include "Time.h"
#include "Timezone.h"
#include "Adafruit_GFX.h"
#include "Adafruit_PCD8544.h"
#include <avr/sleep.h>

const byte tempOptPin = 2; // F/C selection
enum TEMP_OPT {
	TEMP_OPT_C = 0,
	TEMP_OPT_F = 1
};
RTC_DS3231 rtc;
const byte rtcInterruptPin = 3; // rtc can wake us up every second
const byte LMT86Pin = A3;		// analog temperature sensor
const float aVRefmV = 3270.0;	// analog reference voltage in mV
//US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240};	  //Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};	  //Standard time = UTC - 5 hours
Timezone myTZ(myDST, mySTD);
// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
const byte scePin = 6;   // SCE - Chip select
const byte rstPin = 7;   // RST - Reset
const byte dcPin = 5;    // DC - Data/Command
Adafruit_PCD8544 display = Adafruit_PCD8544(dcPin, scePin, rstPin);

static void displayWarnSetTime() {
	display.setTextSize(1);
	display.setTextColor(BLACK);
	display.setCursor(0,0);
	display.println("Lost power ;-(");
	display.println("Use serial port");
	display.println("to set time");
	display.display();
}

void setup() {
	analogReference(EXTERNAL);	// our AREF is 3.3V
	display.begin();
	// init done
	// you can change the contrast around to adapt the display
	// for the best viewing!
	display.setContrast(40);

	pinMode(rtcInterruptPin, INPUT_PULLUP);
	pinMode(tempOptPin, INPUT);
	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}
	delay(1000);
	if (!rtc.begin()) {
		Serial.println("Couldn't find RTC");
		while (1);
	}

	if (rtc.lostPower()) {
		int c;
		char buf[16];
		char *s;
		int sz;
		uint32_t time = 0;

		displayWarnSetTime();

		while (!time) { 
			Serial.print(String("Enter UTC unix time: "));
			c = -1;
			s = buf;
			sz = sizeof(buf);
			while (c != '\n' && c != '\r' && sz > 1) {
				c = -1;
				while (c == -1)
					c = Serial.read();
				Serial.write(c);
				*s++ = c;
				sz--;
			}
			Serial.println();
			*s = 0;
			time = atol(buf);
		}
		Serial.print(String("thank you for entering "));
		Serial.println(time, DEC);
		rtc.adjust(DateTime(time));

		display.clearDisplay();
	}
	// enabling 1Hz timer from rtc (connected to rtcInterruptPin)
	rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
}

//Function to print time with time zone
void printTemp(time_t t, char *tz, float temperature) {
	char buf[16];
	display.fillScreen(WHITE),
	display.setTextSize(1);
	display.setTextColor(BLACK);
	display.setCursor(0, 0);
	snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour(t), minute(t), second(t));
	Serial.print(buf);
	Serial.print(' ');
	display.println(buf);
	snprintf(buf, sizeof(buf), "%02d %-3.3s %04d", day(t), monthShortStr(month(t)), year(t));
	Serial.print(buf);
	Serial.print(' ');
	display.println(buf);
	switch(digitalRead(tempOptPin)) {
	case TEMP_OPT_C:
		snprintf(buf, sizeof(buf), "TEMP: %02d C", int(temperature));
		break;
	default: // TEMP_OPT_F
		snprintf(buf, sizeof(buf), "TEMP: %02d F", int(temperature*(9.0/5.0)+32.0));
	}
	Serial.print(buf);
	Serial.print(' ');
	display.println(buf);
	Serial.println();
	display.display();
}

float getTempLMT86(short pin) {
	int tempRead = analogRead(pin);
	// as per http://www.ti.com/lit/ds/symlink/lmt86-q1.pdf
	float mV = tempRead*aVRefmV/1024.0;
	float t = (10.888-sqrt(10.888*10.888+4*0.00347*(1777.3-mV)))/(2*(-0.00347))+30.0;
	return t;
}

static void refresh() {
	TimeChangeRule *tcr;		//pointer to the time change rule, use to get TZ abbrev
	time_t local;
	time_t utc = rtc.now().unixtime();
	local = myTZ.toLocal(utc, &tcr);
	printTemp(local, tcr->abbrev, getTempLMT86(LMT86Pin));
}

static void wake () {
	// nop
}


void sleepNow()			// here we put the arduino to sleep
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
	sleep_enable();			 // enables the sleep bit in the mcucr register
							 // so sleep is possible. just a safety pin
	attachInterrupt(digitalPinToInterrupt(rtcInterruptPin), wake, FALLING);
 
	sleep_mode();			 // here the device is actually put to sleep!!
							 // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
	sleep_disable();		 // first thing after waking from sleep:
							 // disable sleep...
	detachInterrupt(digitalPinToInterrupt(rtcInterruptPin));
}

void loop() {
	sleepNow();
	refresh();
}
