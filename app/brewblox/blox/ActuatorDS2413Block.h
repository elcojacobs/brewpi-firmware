#pragma once

#include "ActuatorDS2413.h"
#include "ActuatorDigitalConstrained.h"
#include "ActuatorDigitalConstraintsProto.h"
#include "FieldTags.h"
#include "blox/Block.h"
#include "blox/DS2413Block.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/ActuatorDS2413.pb.h"
#include <cstdint>

class ActuatorDS2413Block : public Block<BrewbloxOptions_BlockType_ActuatorDS2413> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<DS2413> hwDevice;
    ActuatorDS2413 actuator;
    ActuatorDigitalConstrained constrained;

public:
    ActuatorDS2413Block(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , hwDevice(objects)
        , actuator(hwDevice.lockFunctor())
        , constrained(actuator)
    {
    }
    virtual ~ActuatorDS2413Block() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_ActuatorDS2413 message = blox_ActuatorDS2413_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &message, blox_ActuatorDS2413_fields, blox_ActuatorDS2413_size);

        if (result == cbox::CboxError::OK) {
            hwDevice.setId(message.hwDevice);
            actuator.channel(DS2413::Pio(message.channel));
            actuator.invert(message.invert);
            setDigitalConstraints(message.constrainedBy, constrained, objectsRef);
            constrained.state(ActuatorDigital::State(message.state));
        }

        return result;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorDS2413 message = blox_ActuatorDS2413_init_zero;
        FieldTags stripped;

        auto state = actuator.state();
        if (state == ActuatorDigital::State::Unknown) {
            stripped.add(blox_ActuatorDS2413_state_tag);
        } else {
            message.state = blox_AD_State(actuator.state());
        }

        message.hwDevice = hwDevice.getId();
        message.channel = blox_ActuatorDS2413_Channel(actuator.channel());
        message.invert = actuator.invert();
        getDigitalConstraints(message.constrainedBy, constrained);

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 1);
        return streamProtoTo(out, &message, blox_ActuatorDS2413_fields, blox_ActuatorDS2413_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        return streamTo(out);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        actuator.update();
        constrained.update(now);
        return now + 1000;
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_ActuatorDS2413) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<ActuatorDigitalConstrained>()) {
            // return the member that implements the interface in this case
            ActuatorDigitalConstrained* ptr = &constrained;
            return ptr;
        }
        return nullptr;
    }

    ActuatorDS2413& getPin()
    {
        return actuator;
    }

    ActuatorDigitalConstrained& getConstrained()
    {
        return constrained;
    }
};
