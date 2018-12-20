/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewBlox
 *
 * BrewBlox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewBlox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ActuatorAnalogConstrained.h"
#include "ActuatorDigitalConstrained.h"
#include "ActuatorPwm.h"
#include "Balancer.h"
#include "DigitalConstraints.pb.h"
#include "FixedPoint.h"
#include "OneWireDevice.h"
#include "Pid.h"
#include "ProcessValue.h"
#include "Setpoint.h"
#include "SetpointSensorPair.h"
#include "TempSensor.h"
#include "brewblox.pb.h"
#include "cbox/ObjectBase.h"

namespace cbox {

template <>
const obj_type_t
interfaceIdImpl<ProcessValue<safe_elastic_fixed_point<11, 12, int32_t>>>()
{
    return BrewbloxFieldOptions_InterfaceType_ProcessValueTypeId;
}

template <>
const obj_type_t
interfaceIdImpl<TempSensor>()
{
    return BrewbloxFieldOptions_InterfaceType_TempSensorTypeId;
}

template <>
const obj_type_t
interfaceIdImpl<SetpointSensorPair>()
{
    return BrewbloxFieldOptions_InterfaceType_SetpointSensorPairTypeId;
}

template <>
const obj_type_t
interfaceIdImpl<Setpoint>()
{
    return BrewbloxFieldOptions_InterfaceType_SetpointTypeId;
}

template <>
const obj_type_t
interfaceIdImpl<ActuatorAnalogConstrained>()
{
    return BrewbloxFieldOptions_InterfaceType_ActuatorAnalogTypeId;
}

template <>
const obj_type_t
interfaceIdImpl<ActuatorDigitalConstrained>()
{
    return BrewbloxFieldOptions_InterfaceType_ActuatorDigitalTypeId;
}

template <>
const obj_type_t
interfaceIdImpl<TimedMutex>()
{
    return BrewbloxFieldOptions_InterfaceType_MutexTypeId;
}

template <>
const obj_type_t
interfaceIdImpl<Balancer<blox_DigitalConstraint_mutex_tag>>()
{
    return BrewbloxFieldOptions_InterfaceType_BalancerTypeId;
}

template <>
const obj_type_t
interfaceIdImpl<OneWireDevice>()
{
    return BrewbloxFieldOptions_InterfaceType_OneWireDeviceTypeId;
}

} // end namespace cbox
