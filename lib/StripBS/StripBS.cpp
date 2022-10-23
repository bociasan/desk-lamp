#include <StripBS.h>

StripBS::StripBS(uint8_t pin = NO_PIN, float maxBrightness = DEFAULT_BRIGHTNESS, int changingDelay = DEFAULT_DELAY, float k = DEFAULT_K, bool useSerial = false)
    : _pin(pin), _useSerial(useSerial), _maxBrightness(maxBrightness), _changingDelay(changingDelay), _k(k){
        turnOff();
        _currentBrightness = 0;
        _previousBrightness = 0;
        _previousPrintTime = millis();
        _brightnessEqualFlag = false;
        _hasChanges = false;
        _lastChange = millis();
        pinMode(_pin, OUTPUT);
        analogWrite(pin, _currentBrightness);
        
}

void StripBS::tick(){
    // if (millis() - _lastChange > _changingDelay){
    if ((abs(_currentBrightness - _targetBrightness) > 0.001) && (millis() - _lastChange > _changingDelay)){
        _lastChange = millis();
        _currentBrightness += (_targetBrightness - _currentBrightness) * _k;

        // if (abs(_currentBrightness - _targetBrightness) > 0.001){
            if (_brightnessEqualFlag ){
                if (millis() - _brightnessEqualMillis > _changingDelay){
                    _brightnessEqualFlag = false;
                    if (_currentBrightness > _targetBrightness){
                        _currentBrightness = trunc(_currentBrightness);
                    } else {
                        _currentBrightness = trunc(_currentBrightness)+1;
                    }
                }
            } else if ((int)trunc(_currentBrightness) == (int)trunc(_previousBrightness)){
                _brightnessEqualFlag = true;
                _brightnessEqualMillis = millis();
            }
            //
            analogWrite(_pin, _currentBrightness);
            _previousBrightness = _currentBrightness;
            //printBrightness();
            _hasChanges = true;
            //
        // }

        //mutate de aici

        
    }
}

void StripBS::printBrightness(){
    // if (_useSerial && (millis() - _previousPrintTime > 100) && (abs(_currentBrightness - _targetBrightness) > 0.1)){
    //     _previousPrintTime = millis();
        Serial.print("Current strip brightness: ");
        Serial.println(_currentBrightness);
    // }
}

bool StripBS::getState(){
    return _state;
}

float StripBS::getCurrentBrightness(){
    return _currentBrightness;
}

float StripBS::getMaxBrightness(){
    return _maxBrightness;
}

void StripBS::toggleState(){
    if (_state) {
        turnOff();
    } else {
        turnOn();
    }
}

void StripBS::turnOn(){
    _hasChanges = true;
    _state = true;
    _targetBrightness = _maxBrightness;
}

void StripBS::turnOff(){
    _hasChanges = true;
    _state = false;
    _targetBrightness = 0;
}

bool StripBS::hasChanges(){
    return _hasChanges;
}

void StripBS::clearHasChanges(){
    _hasChanges = false;
}

unsigned long StripBS::getLastChange(){
    return _lastChange;
}

float StripBS::getTargetBrightness(){
    return _targetBrightness;
}

void StripBS::setTargetBrightness(float brightness){
    if (brightness <= _maxBrightness){
        _targetBrightness = brightness;
    } else {
        Serial.printf("TargetBrightness %f is bigger than _maxBrightness %f\n", brightness, _maxBrightness);
    }
}

void StripBS::setMaxBrightness(float maxBrightness){
    if (maxBrightness < 256){
        float newTargetBrightness = mapFloat(_targetBrightness, 0, _maxBrightness, 0, maxBrightness);
        newTargetBrightness = trunc(newTargetBrightness);
        _maxBrightness = maxBrightness;
        Serial.printf("NTB %f = mapFloat(%f, %f, %f)\n", newTargetBrightness, _targetBrightness, _maxBrightness, maxBrightness);

        setTargetBrightness(newTargetBrightness);
    } else{

        Serial.printf("MaxBright is bigger than 255! | %f\n", maxBrightness);
    }
}