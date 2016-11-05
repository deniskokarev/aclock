#include "LCD_Functions.h"
#include <Wire.h>
#include "RTClib.h"
#include "Time.h"
#include "Timezone.h"

RTC_DS3231 rtc;

//US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240};    //Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};     //Standard time = UTC - 5 hours
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
void printTime(time_t t, char *tz) {
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
    Serial.println();
}

void loop() {
	TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev
	time_t local;
	time_t utc = rtc.now().unixtime();
    printTime(utc, (char*)"UTC");
    local = myTZ.toLocal(utc, &tcr);
    printTime(local, tcr -> abbrev);
    delay(3000);
}

