/*
 * Copyright 2015 BrewPi/Elco Jacobs.
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

#include <stdint.h>

#include "temperatureFormats.h"
#include "ActuatorInterfaces.h"
#include "ActuatorMutexGroupInterface.h"
#include "ControllerMixins.h"

/* A driver actuator to wrap a digital Actuator and block SetActive calls if the mutex group does does not honor the request
 */

class ActuatorMutexDriver final : public ActuatorDigital, public ActuatorMutexDriverMixin{
public:
    ActuatorMutexDriver(ActuatorDigital & target, ActuatorMutexGroupInterface & m);
    ~ActuatorMutexDriver() = default;


    virtual void accept(VisitorBase & v) override final{
    	v.visit(*this);
    }

    virtual update_t update(const update_t & t) override final {
        return target.update(t);
    }


    // To activate actuator, permission is asked from mutexGroup, false is always allowed
    // when priority not specified, default to highest priority
    virtual void setState(State state, int8_t priority = 127) override final;

    virtual State getState() const override final {
        return target.getState();
    }

private:
    ActuatorDigital & target;
    ActuatorMutexGroupInterface & mutexGroup;

friend class ActuatorMutexDriverMixin;
};



