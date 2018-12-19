#pragma once

#include "ActuatorAnalogConstrained.h"
#include "ActuatorAnalogMock.h"
#include "blox/Block.h"
#include "blox/FieldTags.h"
#include "proto/cpp/ActuatorAnalogMock.pb.h"

class ActuatorAnalogMockBlock : public Block<blox_ActuatorAnalogMock_msgid> {
private:
    ActuatorAnalogMock actuator = ActuatorAnalogMock(0, 0, 100);
    ActuatorAnalogConstrained constrained = ActuatorAnalogConstrained(actuator);

public:
    ActuatorAnalogMockBlock()
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_ActuatorAnalogMock newData = blox_ActuatorAnalogMock_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &newData, blox_ActuatorAnalogMock_fields, blox_ActuatorAnalogMock_size);
        if (result == cbox::CboxError::OK) {
            constrained.setting(cnl::wrap<ActuatorAnalog::value_t>(newData.setting));
            actuator.minSetting(cnl::wrap<ActuatorAnalog::value_t>(newData.minSetting));
            actuator.maxSetting(cnl::wrap<ActuatorAnalog::value_t>(newData.maxSetting));
            actuator.minValue(cnl::wrap<ActuatorAnalog::value_t>(newData.minValue));
            actuator.maxValue(cnl::wrap<ActuatorAnalog::value_t>(newData.maxValue));
        }
        return result;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorAnalogMock message = blox_ActuatorAnalogMock_init_zero;
        FieldTags stripped;

        if (constrained.valueValid()) {
            message.value = cnl::unwrap(constrained.value());
        } else {
            stripped.add(blox_ActuatorAnalogMock_value_tag);
        }
        if (constrained.settingValid()) {
            message.setting = cnl::unwrap(constrained.setting());
        } else {
            stripped.add(blox_ActuatorAnalogMock_setting_tag);
        };

        message.minSetting = cnl::unwrap(actuator.minSetting());
        message.maxSetting = cnl::unwrap(actuator.maxSetting());
        message.minValue = cnl::unwrap(actuator.minValue());
        message.maxValue = cnl::unwrap(actuator.maxValue());

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 2);
        return streamProtoTo(out, &message, blox_ActuatorAnalogMock_fields, blox_ActuatorAnalogMock_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorAnalogMock message = blox_ActuatorAnalogMock_init_zero;
        message.setting = cnl::unwrap(constrained.setting());

        message.minSetting = cnl::unwrap(actuator.minSetting());
        message.maxSetting = cnl::unwrap(actuator.maxSetting());
        message.minValue = cnl::unwrap(actuator.minValue());
        message.maxValue = cnl::unwrap(actuator.maxValue());

        return streamProtoTo(out, &message, blox_ActuatorAnalogMock_fields, blox_ActuatorAnalogMock_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == blox_ActuatorAnalogMock_msgid) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<ActuatorAnalogConstrained>()) {
            // return the member that implements the interface in this case
            ActuatorAnalogConstrained* ptr = &constrained;
            return ptr;
        }
        if (iface == cbox::interfaceId<ProcessValue<ActuatorAnalog::value_t>>()) {
            // return the member that implements the interface in this case
            ProcessValue<ActuatorAnalog::value_t>* ptr = &actuator;
            return ptr;
        }
        return nullptr;
    }

    ActuatorAnalogMock& get()
    {
        return actuator;
    }
};
