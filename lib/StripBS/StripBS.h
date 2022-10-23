#include <Arduino.h>
#define NO_PIN 255
#define DEFAULT_BRIGHTNESS 90
#define DEFAULT_DELAY 50
#define DEFAULT_K 0.2

class StripBS {
    public:
        StripBS(uint8_t pin,  float maxBrightness, int changingDelay, float k, bool useSerial);
        void tick();
        void printBrightness();
        float getCurrentBrightness();
        float getMaxBrightness();
        float getTargetBrightness();
        unsigned long getLastChange();
        void setTargetBrightness(float brightness);
        void setMaxBrightness(float maxBrightness);
        bool getState();
        bool hasChanges();
        void toggleState();
        void turnOn();
        void turnOff();
        void clearHasChanges();
        
    private:
        byte _pin;
        bool _state;
        bool _useSerial;
        bool _hasChanges;
        bool _brightnessEqualFlag;
        unsigned long _lastChange;
        unsigned long _previousPrintTime;
        unsigned long _brightnessEqualMillis;
        float _maxBrightness;
        float _targetBrightness;
        float _currentBrightness;
        float _previousBrightness;
        byte _changingDelay;
        
        float _k;
};