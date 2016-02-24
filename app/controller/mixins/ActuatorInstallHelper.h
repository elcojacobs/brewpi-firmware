/*
 * Copyright 2015 Matthew McGowan
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
#include "defaultDevices.h"
#include <boost/tti/has_member_data.hpp>

BOOST_TTI_HAS_MEMBER_DATA(target)


class ActuatorInstallHelperAdapter{
public:
    ActuatorInstallHelperAdapter() = default;
protected:
    ~ActuatorInstallHelperAdapter() = default; // should not be destructed through this base class

public:
    virtual Actuator * getBareActuator() = 0;
    virtual bool installActuatorFinalTarget(ActuatorDigital * a) = 0;
    virtual bool uninstallActuatorFinalTarget() = 0;
};

template< class T>
class ActuatorInstallHelper : public ActuatorInstallHelperAdapter, public T
{
public:
    ActuatorInstallHelper() = default;
protected:
    ~ActuatorInstallHelper() = default; // should not be destructed through this base class

private:
    // compile time constant to select the right implementation
    using has_target = typename has_member_data_target<T, ActuatorDigital*>::value;

    // Actuators driving a target recurse the pointer chain to find the bottom target
    Actuator * getBareActuatorImpl(boost::mpl::true_) {
        if( target->getBareActuator() == target){
            return target; // my target is bottom
        }
        else{
            return target->getBareActuator(); // my target is not bottom
        }
    }
    // Actuators not driving a target return themselves
    Actuator * getBareActuatorImpl(boost::mpl::false_) {
        return this;
    }

    // Swap out the last actuator in the chain for driver actuators
    bool installActuatorFinalTargetImpl(ActuatorDigital * a, boost::mpl::true_){
        if(target->getBareActuator() == target){
            // I am the lowest level driver. my target is the bottom target
            if(target == a){
                return false; // actuator was already installed
            }
            if(target != defaultActuator()){
                delete target; // target is only referenced here and should be deleted
            }
            target = a;
            return true; // installed new actuator
        }
        else{
            return target->installActuatorFinalTarget(a); // my target is not bottom
        }
    }

    // Actuators that do not have a target just return false
    bool installActuatorFinalTargetImpl(ActuatorDigital * a, boost::mpl::false_){
        return false;
    }

public:
    virtual Actuator * getBareActuator() final{
        return getBareActuatorImpl(has_target());
    }

    virtual bool installActuatorFinalTarget(ActuatorDigital * a) final{
        return installActuatorFinalTargetImpl(a, has_target());
    }

    virtual bool uninstallActuatorFinalTarget() final{
        return installActuatorFinalTarget(defaultActuator());
    }
};
