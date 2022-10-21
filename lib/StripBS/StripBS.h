#include <Arduino.h>
#define NO_PIN 255
#define DEFAULT_BRIGHTNESS 90
#define DEFAULT_DELAY 50
#define DEFAULT_K 0.2

class StripBS {
    public:
        StripBS(uint8_t pin,  float brightness, int changingDelay, float k, bool useSerial);
        void tick();
        void printBrightness();
    private:
        byte _pin;
        bool _useSerial;
        unsigned long _lastChange;
        unsigned long _previousPrintTime;
        float _brightness;
        float _currentBrightness;
        byte _changingDelay;

        float _k;
};