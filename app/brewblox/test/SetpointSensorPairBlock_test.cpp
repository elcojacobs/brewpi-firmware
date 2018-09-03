/*
 * Copyright 2017 BrewPi
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

#include <catch.hpp>
#include <cstdio>
#include <iomanip>
#include <iostream>

#include "../BrewBlox.h"
#include "blox/SetPointSimpleBlock.h"
#include "blox/SetpointSensorPairBlock.h"
#include "blox/TempSensorMockBlock.h"
#include "cbox/Box.h"
#include "cbox/DataStream.h"
#include "cbox/DataStreamIo.h"
#include "cbox/Object.h"
#include "proto/test/cpp/SetPointSimple.test.pb.h"
#include "proto/test/cpp/SetpointSensorPair.test.pb.h"
#include "proto/test/cpp/TempSensorMock.test.pb.h"
#include "temperatureFormats.h"

class ProtoDataOut {
public:
    cbox::HexCrcDataOut& out;
    ProtoDataOut(cbox::HexCrcDataOut& target)
        : out(target)
    {
    }

    void put(::google::protobuf::Message& message)
    {
        for (auto& c : message.SerializeAsString()) {
            out.write(c);
        }
        out.write(0); // zero terminate protobuf message
    }
};

SCENARIO("A Blox SetpointSensorPair object can be created from streamed protobuf data")
{
    static auto& box = brewbloxBox();
    static auto& connSource = testConnectionSource();
    using commands = cbox::Box::CommandID;

    std::stringstream in, out, expected;

    // use some helpers to create the data commands
    cbox::OStreamDataOut inOs(in);
    cbox::BinaryToHexTextOut toHex(inOs);
    cbox::HexCrcDataOut inEncoder(toHex);
    ProtoDataOut inProto(inEncoder);

    connSource.add(in, out);

    auto clearStreams = [&in, &out, &expected]() {
        in.str("");
        in.clear();
        out.str("");
        out.clear();
        expected.str("");
        expected.clear();
    };

    // create mock sensor
    clearStreams();
    inEncoder.put(commands::CREATE_OBJECT);
    inEncoder.put(cbox::obj_id_t(100));
    inEncoder.put(uint8_t(0xFF));
    inEncoder.put(cbox::resolveTypeId<TempSensorMockBlock>());

    auto newSensor = blox::TempSensorMock();
    newSensor.set_value(temp_t(20.0).getRaw());
    newSensor.set_connected(true);
    inProto.put(newSensor);

    inEncoder.endMessage();

    box.hexCommunicate();
    CHECK(out.str().find("|00") != std::string::npos); // no errors

    // create setpoint
    clearStreams();
    inEncoder.put(commands::CREATE_OBJECT);
    inEncoder.put(cbox::obj_id_t(101));
    inEncoder.put(uint8_t(0xFF));
    inEncoder.put(cbox::resolveTypeId<SetPointSimpleBlock>());

    blox::SetPointSimple newSetPoint;
    newSetPoint.set_setting(temp_t(21.0).getRaw());
    inProto.put(newSetPoint);

    inEncoder.endMessage();

    box.hexCommunicate();
    CHECK(out.str().find("|00") != std::string::npos); // no errors

    // create pair
    clearStreams();
    inEncoder.put(commands::CREATE_OBJECT);
    inEncoder.put(cbox::obj_id_t(102));
    inEncoder.put(uint8_t(0xFF));
    inEncoder.put(cbox::resolveTypeId<SetpointSensorPairBlock>());

    blox::SetpointSensorPair newPair;
    newPair.set_sensorid(100);
    newPair.set_setpointid(101);
    inProto.put(newPair);

    inEncoder.endMessage();
    box.hexCommunicate();

    CHECK(out.str().find("|00") != std::string::npos); // no errors

    // read pair
    clearStreams();
    inEncoder.put(commands::READ_OBJECT);
    inEncoder.put(cbox::obj_id_t(102));

    inEncoder.endMessage();
    box.hexCommunicate();

    CHECK(out.str().find("|00") != std::string::npos); // no errors
    CHECK(out.str() == "");                            // no errors
}
