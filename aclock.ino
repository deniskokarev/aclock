#include "LCD_Functions.h"
#include <Wire.h>
#include "RTClib.h"
#include "Time.h"
#include "Timezone.h"

RTC_DS3231 rtc;
const int LMT86Pin = A3;
const float aVRefmV = 4950.0;	// referrence voltage in mV

//US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240};	  //Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};	  //Standard time = UTC - 5 hours
Timezone myTZ(myDST, mySTD);

void setup() {

	Serial.begin(9600);

	delay(3000); // wait for console opening

	if (! rtc.begin()) {
		Serial.println("Couldn't find RTC");
		while (1);
	}

	if (rtc.lostPower()) {
		Serial.println("RTC lost power, lets set the time!");
		rtc.adjust(DateTime(1970, 1, 1, 0, 0, 0));
		// This line sets the RTC with an explicit date & time, for example to set
		// January 21, 2014 at 3am you would call:
		// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
	}
}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val) {
	if (val < 10) Serial.print('0');
	Serial.print(val, DEC);
	return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val) {
	Serial.print(':');
	if(val < 10) Serial.print('0');
	Serial.print(val, DEC);
}

//Function to print time with time zone
void printTemp(time_t t, char *tz, int temperature) {
	sPrintI00(hour(t));
	sPrintDigits(minute(t));
	sPrintDigits(second(t));
	Serial.print(' ');
	Serial.print(dayShortStr(weekday(t)));
	Serial.print(' ');
	sPrintI00(day(t));
	Serial.print(' ');
	Serial.print(monthShortStr(month(t)));
	Serial.print(' ');
	Serial.print(year(t));
	Serial.print(' ');
	Serial.print(tz);
	Serial.print(' ');
	sPrintI00(temperature);
	Serial.println("°C");
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
	delay(3000);
}

