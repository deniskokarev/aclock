#include "LCD_Functions.h"
#include <Wire.h>
#include "RTClib.h"
#include "Time.h"
#include "Timezone.h"

RTC_DS3231 rtc;
const int LMT86Pin = A3;
const float aVRefmV = 4950.0;	// reference voltage in mV

//US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240};	  //Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};	  //Standard time = UTC - 5 hours
Timezone myTZ(myDST, mySTD);

void setup() {

	Serial.begin(9600);
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}
	delay(1000);
	lcdBegin(); // This will setup our pins, and initialize the LCD
	
	delay(3000); // wait for console opening
	clearDisplay(WHITE);
	updateDisplay();

	if (! rtc.begin()) {
		Serial.println("Couldn't find RTC");
		while (1);
	}

	if (rtc.lostPower()) {
		setStr((char*)"Lost power ;-(", 0, 10, BLACK);
		setStr((char*)"Enter time_t", 0, 20, BLACK);
		setStr((char*)"@serial port", 0, 30, BLACK);
		updateDisplay();
		int c = -1;
		char buf[16];
		char *s;
		int sz = sizeof(buf);
		uint32_t time = 0;
		//Serial.setTimeout(1000);
		while (!time) { 
			Serial.print(String("Enter unix time: "));
			s = buf;
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
		clearDisplay(WHITE);
		updateDisplay();
	}
}

//Function to print time with time zone
void printTemp(time_t t, char *tz, int temperature) {
	char buf[16];
	snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour(t), minute(t), second(t));
	Serial.print(buf);
	Serial.print(' ');
	setStr(buf, 0, 10, BLACK);
	snprintf(buf, sizeof(buf), "%02d %-3.3s %04d", day(t), monthShortStr(month(t)), year(t));
	Serial.print(buf);
	Serial.print(' ');
	setStr(buf, 0, 20, BLACK);
	snprintf(buf, sizeof(buf), "TEMP: %02d C", temperature);
	Serial.print(buf);
	Serial.print(' ');
	setStr(buf, 0, 30, BLACK);
	Serial.println();
	updateDisplay();
}

float getTempLMT86(short pin) {
	int tempRead = analogRead(pin);
	// as per http://www.ti.com/lit/ds/symlink/lmt86-q1.pdf
	float mV = tempRead*aVRefmV/1024.0;
	float t = (10.888-sqrt(10.888*10.888+4*0.00347*(1777.3-mV)))/(2*(-0.00347))+30.0;
	return t;
}

void loop() {
	TimeChangeRule *tcr;		//pointer to the time change rule, use to get TZ abbrev
	time_t local;
	time_t utc = rtc.now().unixtime();
	local = myTZ.toLocal(utc, &tcr);
	printTemp(local, tcr->abbrev, getTempLMT86(LMT86Pin));
	delay(1000);
}

