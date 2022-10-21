#include <StripBS.h>

StripBS::StripBS(uint8_t pin = NO_PIN, float brightness = DEFAULT_BRIGHTNESS, int changingDelay = DEFAULT_DELAY, float k = DEFAULT_K, bool useSerial = false)
    : _pin(pin), _useSerial(useSerial), _brightness(brightness), _changingDelay(changingDelay), _k(k){
        _currentBrightness = 0;
        _previousPrintTime = millis();
        _lastChange = millis();
        pinMode(_pin, OUTPUT);
        analogWrite(pin, _currentBrightness);
        
}

void StripBS::tick(){
    if (millis() - _lastChange > _changingDelay){
        _lastChange = millis();
        _currentBrightness += (_brightness - _currentBrightness) * _k;
        analogWrite(_pin, _currentBrightness);

        printBrightness();
    }
}

void StripBS::printBrightness(){
    if (_useSerial && (millis() - _previousPrintTime > 100) && (abs(_currentBrightness - _brightness) > 0.1)){
        _previousPrintTime = millis();
        Serial.print("Current strip brightness: ");
        Serial.println(_currentBrightness);
    }
}
