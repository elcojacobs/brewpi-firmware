/*
 * Copyright 2018 BrewPi B.V.
 *
 * This file is part of BrewBlox.
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

#include <catch.hpp>

#include "BrewBloxTestBox.h"
#include "blox/DS2413Block.h"
#include "cbox/CboxPtr.h"
#include "cbox/DataStreamIo.h"
#include "proto/test/cpp/DS2413.test.pb.h"
#include <sstream>

SCENARIO("A DS2413 Block")
{
    WHEN("a DS2413 object is created")
    {
        BrewBloxTestBox testBox;
        using commands = cbox::Box::CommandID;

        testBox.reset();

        testBox.put(commands::CREATE_OBJECT);
        testBox.put(cbox::obj_id_t(100));
        testBox.put(uint8_t(0xFF));
        testBox.put(DS2413Block::staticTypeId());

        auto message = blox::DS2413();
        message.set_address(12345678);

        testBox.put(message);

        testBox.processInput();
        CHECK(testBox.lastReplyHasStatusOk());

        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(100));

        auto decoded = blox::DS2413();
        testBox.processInputToProto(decoded);

        THEN("The returned protobuf data is as expected")
        {
            CHECK(testBox.lastReplyHasStatusOk());

            // the value is invalid and therefore added to stripped fields to distinguish from value zero
            CHECK(decoded.ShortDebugString() == "address: 12345678");
        }

        THEN("The writable settings match what was sent")
        {
            auto lookup = brewbloxBox().makeCboxPtr<DS2413Block>(100);
            auto devicePtr = lookup.lock();
            REQUIRE(devicePtr);
            CHECK(devicePtr->get().getDeviceAddress() == 12345678);

            AND_THEN("The values that are not writable are unchanged")
            {
                // CHECK(sensorPtr->get().value() == temp_t(0));
                // CHECK(sensorPtr->get().valid() == false);
            }
        }
    }
}
