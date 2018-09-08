/*
 * Copyright 2017 BrewPi/Elco Jacobs.
 * Copyright 2017 Matthew McGowan.
 *
 * This file is part of BrewPi.
 * 
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "Ticks.h"
#include <stdint.h>

/**
 * A Ticks implementation that increments the millis count each time it is called.
 * This is used for testing.
 */
class MockTicks {
public:
    MockTicks(uint8_t increment)
        : _increment(increment)
        , _ticks(0)
    {
    }
    MockTicks()
        : _increment(1)
        , _ticks(0)
    {
    }

    ticks_millis_t millis() { return _ticks += _increment; }
    ticks_micros_t micros() { return 1000 * _ticks++; }
    ticks_seconds_t seconds() { return millis() / 1000; }
    ticks_seconds_t timeSinceSeconds(ticks_seconds_t timeStamp) { return ::timeSinceSeconds(seconds(), timeStamp); }
    ticks_millis_t timeSinceMillis(ticks_millis_t timeStamp) { return ::timeSinceMillis(millis(), timeStamp); }
    void reset(void) { _ticks = 0; };
    void advance(ticks_millis_t adv)
    {
        _ticks += adv;
    }

private:
    uint32_t _increment;
    uint32_t _ticks;
};

/**
 * Externally provided millis timer. The calling code takes care of advancing the timer by calling setMillis or incMillis.
 * This is used for testing and also by the simulator to provide simulated time.
 */
class ExternalTicks {
public:
    ExternalTicks()
        : _ticks(0)
    {
    }

    ticks_millis_t millis() { return _ticks; }
    ticks_micros_t micros() { return _ticks * 1000; }
    ticks_seconds_t seconds() { return millis() / 1000; }
    ticks_seconds_t timeSinceSeconds(ticks_seconds_t timeStamp) { return ::timeSinceSeconds(seconds(), timeStamp); }
    ticks_millis_t timeSinceMillis(ticks_millis_t timeStamp) { return ::timeSinceMillis(millis(), timeStamp); }
    void reset(void) { _ticks = 0; };

    void setMillis(ticks_millis_t now) { _ticks = now; }
    void incMillis(ticks_millis_t advance) { _ticks += advance; }

private:
    ticks_millis_t _ticks;
};

/**
 * A delay class that does nothing.
 * In the AVR simulator, delays using millis() take a very long time. Using this class makes it possible to step through the code.
 */
class NoOpDelay {
public:
    void seconds(uint16_t seconds) { millis(seconds << 10); }
    void millis(uint32_t millis) {}
    void microseconds(uint32_t micros) {}
};

/*
class MockDelay {
public:
    MockDelay() {}
    void seconds(uint16_t seconds){
        baseticks.advance(ticks_millis_t(1000) * ticks_millis_t(seconds));
    }
    void millis(uint16_t millis){
        baseticks.advance(millis);
    }
    void microseconds(uint32_t micros) {}
};

*/
