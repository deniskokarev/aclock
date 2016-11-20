#include "RTClib.h"
#include "Time.h"
#include "Timezone.h"
#include "Adafruit_GFX.h"
#include "Adafruit_PCD8544.h"
#include <avr/sleep.h>

RTC_DS3231 rtc;
const int LMT86Pin = A3;		// analog temperature sensor
const float aVRefmV = 3270.0;	// analog reference voltage in mV
//US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240};	  //Daylight time = UTC - 4 hours
TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};	  //Standard time = UTC - 5 hours
Timezone myTZ(myDST, mySTD);
// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
const int scePin = 6;   // SCE - Chip select
const int rstPin = 7;   // RST - Reset
const int dcPin = 5;    // DC - Data/Command
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
	display.setContrast(50);

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
}

//Function to print time with time zone
void printTemp(time_t t, char *tz, int temperature) {
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
	snprintf(buf, sizeof(buf), "TEMP: %02d C", temperature);
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

void loop() {
	TimeChangeRule *tcr;		//pointer to the time change rule, use to get TZ abbrev
	time_t local;
	time_t utc = rtc.now().unixtime();
	local = myTZ.toLocal(utc, &tcr);
	printTemp(local, tcr->abbrev, getTempLMT86(LMT86Pin));
	delay(1000);
}

