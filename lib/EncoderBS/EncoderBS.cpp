#include <EncoderBS.h>

IRAM_ATTR void EncoderBS::isr(){
   EncoderBS_instance->handleInterrupt();
}

EncoderBS::EncoderBS(uint8_t pinA = NO_PIN, uint8_t pinB = NO_PIN, uint8_t pinSw = NO_PIN, bool useSerial = false)
    : _pinA(pinA), _pinB(pinB), _pinSw(pinSw), _useSerial(useSerial){
         _startTime = millis();

         EncoderBS_instance = this;
         
         if (_pinA != NO_PIN && _pinB != NO_PIN) {
            attachInterrupt(_pinA, isr, CHANGE);  
            attachInterrupt(_pinB, isr, CHANGE); 
         }

         if (_pinSw != NO_PIN){
            attachInterrupt(_pinSw, isr, CHANGE); 
         }
}

void EncoderBS::handleInterrupt(){
   unsigned char currentState = GPIP(_pinA)<<1 | GPIP(_pinB);
      if ((currentState == 0 || currentState == 3) && currentState != _lastState){
         _lastState = currentState;
         if ((currentState == 0 && _intermediateState == 1) || (currentState == 3 && _intermediateState == 2)) {
         _encoderCounter++;
      } else _encoderCounter--;
      // Serial.println(encoderCounter);
      _encoderCounterHasChanged = true;
      _intermediateState = 5;
      } else {
         _intermediateState = currentState;
      }
}

void EncoderBS::printOnCounterChanged(){
        if (_useSerial && _encoderCounterHasChanged){
            _encoderCounterHasChanged = false;
            Serial.print("Encoder value: ");
            Serial.println(_encoderCounter);
        }
}

int EncoderBS::getEncoderCounter(){
   return _encoderCounter;
}

void EncoderBS::setEncoderCounter(int val){
   _encoderCounter = val;
}

void EncoderBS::tick(){
   printOnCounterChanged();
}

EncoderBS * EncoderBS::EncoderBS_instance;
