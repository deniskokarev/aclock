Clock and thermometer with Nokia LCD on Arduino Uno. The main difficulty is 5V->3V logic level translation for LCD

The unit is hardcoded to work with Eastern TZ
When powered first time the RTC time has to be set via serial interface.
It will expect 32bit unix time

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

Hardware was designed with DipTrace v2.4.0.2 and it is located in 'hard' directory
The board is single-sided suitable for DIY and fits the screen component "natively"

Software works with Arduino SDK v1.6

Used modules:
- git submodule add https://github.com/mjoldfield/Arduino-Makefile
- git submodule add https://github.com/adafruit/RTClib
- git submodule add https://github.com/PaulStoffregen/Time
- git submodule add https://github.com/JChristensen/Timezone
- Nokia LCD demo https://learn.sparkfun.com/tutorials/graphic-lcd-hookup-guide

![Complete Device](https://github.com/deniskokarev/aclock/blob/master/CompleteDevice.jpg "Complete Device")
