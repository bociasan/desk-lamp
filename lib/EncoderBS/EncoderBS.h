#ifndef ENCODERBS_LIBRARY_H
#define ENCODERBS_LIBRARY_H

#include <Arduino.h>
#define NO_PIN 255

class EncoderBS {
    
    static IRAM_ATTR void isr();
    static EncoderBS * EncoderBS_instance;

    public:
        EncoderBS(uint8_t pinA, uint8_t pinB, uint8_t pinSw, bool useSerial);
        void tick();
        void printOnCounterChanged();
        int  getEncoderCounter();
        void setEncoderCounter(int val);

    private:
        byte _pinA, _pinB, _pinSw;
        unsigned long _startTime;
        bool _useSerial;
        unsigned char _lastState = 0, _intermediateState = 5;
        int  _encoderCounter = 0;
        bool _encoderCounterHasChanged = false;
        void handleInterrupt();
};

#endif