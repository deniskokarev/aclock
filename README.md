Nokia LCD with DS3231 RTC Clock and LMT86 thermometer as Arduino UNO shield. 5V->3V logic transition is handled with SN74LVC8T245PWR

Standard Makefile compilation on Mac and Linux. If you already have Arduino SDK installed simply run
```
make && make flash
```
in the project directory to program your Arduino UNO board. Verified with Arduino SDK 1.6 on Mac 10.10 Yosemite

.clang_complete options for vim or Emacs users helps code autocompletion and real-time error highlighting. You may have to tweak it accordingly to your environment. This particular file is for GNU Emacs 24.5 with irony/flycheck on Mac OSX 10.10 (Yesemite) and XCode 7.2

The firmware has Eastern TZ hardcoded with automated adjustment for Daylight Saving Time.

Temperature is displayed in C or F depending on the side switch

When powered first time the RTC has to be set via serial interface. It will expect 32bit unix time in UTC. It may be done like this:

```
# get UNIX time +30 sec
expr `TZ=UTC date +%s` + 30
# copy the value
# run terminal
sudo screen /dev/tty.usbmodem1d121 9600
# paste the unix time value into terminal
# hit enter at precisely 30 sec after
# when done quit terminal with Ctl+a + Ctl+\ + y
```

Hardware was designed with DipTrace v2.4 and it is located in 'hard' directory
The board is single-sided suitable for DIY and fits the screen component "natively"

Used modules:
- git submodule add https://github.com/mjoldfield/Arduino-Makefile
- git submodule add https://github.com/adafruit/RTClib
- git submodule add https://github.com/PaulStoffregen/Time
- git submodule add https://github.com/JChristensen/Timezone
- git submodule add https://github.com/adafruit/Adafruit-GFX-Library
- git submodule add https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library

![Complete Device](https://github.com/deniskokarev/aclock/blob/master/CompleteDevice.jpg "Complete Device")

TODO:
- Get rid of LMT86 temperature sensor and use internal DS3231 sensor instead
- Try simplistic 5V->3V signal conversion with simple 10K or 20K resistors
- Flip shield upside down to have USB connector at the bottom for a stable stance when USB cable connected
