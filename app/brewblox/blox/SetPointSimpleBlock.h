#pragma once

#include "SetPoint.h"
#include "Temperature.h"
#include "blox/Block.h"
#include "proto/cpp/SetPointSimple.pb.h"

class SetPointSimpleBlock : public Block<blox_SetPointSimple_msgid> {
private:
    SetPointSimple setpoint;

public:
    SetPointSimpleBlock()
        : setpoint()
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_SetPointSimple newData;
        cbox::CboxError result = streamProtoFrom(dataIn, &newData, blox_SetPointSimple_fields, blox_SetPointSimple_size);
        if (result == cbox::CboxError::OK) {
            setpoint.write(temp_t_from_base(newData.setting));
        }
        return result;
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_SetPointSimple message;
        message.setting = to_base(setpoint.read());

        return streamProtoTo(out, &message, blox_SetPointSimple_fields, blox_SetPointSimple_size);
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
        if (iface == blox_SetPointSimple_msgid) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<SetPoint>()) {
            // return the member that implements the interface in this case
            SetPoint* ptr = &setpoint;
            return ptr;
        }
        return nullptr;
    }

    SetPointSimple& get()
    {
        return setpoint;
    }
};
