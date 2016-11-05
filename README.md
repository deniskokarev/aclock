Clock and thermometer with Nokia LCD on Arduino Uno. The main difficulty is 5V->3V logic level translation for LCD

Hardware was designed with DipTrace v2.4.0.2 and it is located in 'hard' directory
The board is single-sided suitable for DIY and fits the screen component "natively"

Software works with Arduino SDK v1.6

git submodule add https://github.com/mjoldfield/Arduino-Makefile
git submodule add https://github.com/adafruit/RTClib
git submodule add https://github.com/PaulStoffregen/Time
git submodule add https://github.com/JChristensen/Timezone

Source code for Nokia LCD demo is taken from https://learn.sparkfun.com/tutorials/graphic-lcd-hookup-guide
