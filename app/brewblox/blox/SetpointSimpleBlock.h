#pragma once

#include "Setpoint.h"
#include "Temperature.h"
#include "blox/Block.h"
#include "blox/FieldTags.h"
#include "proto/cpp/SetpointSimple.pb.h"

class SetpointSimpleBlock : public Block<BrewbloxOptions_BlockType_SetpointSimple> {
private:
    SetpointSimple setpoint;

public:
    SetpointSimpleBlock()
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_SetpointSimple newData = blox_SetpointSimple_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &newData, blox_SetpointSimple_fields, blox_SetpointSimple_size);
        if (result == cbox::CboxError::OK) {
            setpoint.valid(newData.enabled);
            setpoint.setting(cnl::wrap<temp_t>(newData.setpoint));
        }
        return result;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_SetpointSimple message = blox_SetpointSimple_init_zero;
        FieldTags stripped;

        message.enabled = setpoint.valid();
        message.setpoint = cnl::unwrap(setpoint.setting());

        if (message.enabled) {
            message.setting = message.setpoint;
        } else {
            stripped.add(blox_SetpointSimple_setting_tag);
        };

        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 1);

        return streamProtoTo(out, &message, blox_SetpointSimple_fields, blox_SetpointSimple_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        return streamTo(out);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_SetpointSimple) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<Setpoint>()) {
            // return the member that implements the interface in this case
            Setpoint* ptr = &setpoint;
            return ptr;
        }
        return nullptr;
    }

    SetpointSimple& get()
    {
        return setpoint;
    }
};
