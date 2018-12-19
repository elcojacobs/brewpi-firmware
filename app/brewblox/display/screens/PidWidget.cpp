/*
 * Copyright 2018 BrewPi B.V.
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

#include "PidWidget.h"
#include "BrewBlox.h"
#include "Temperature.h"
#include "d4d.hpp"
#include "future_std.h"
#include <algorithm>

PidWidget::PidWidget(WidgetWrapper& myWrapper, const cbox::obj_id_t& id)
    : WidgetBase(myWrapper)
    , labelRelations{wrapper.pObj(), nullptr}
    , inputTarget{
          {0, 12},              // D4D_POINT position
          {wrapper.cx / 2, 15}, // D4D_SIZE                              size;                 ///< Size of the object.
          0,                    // D4D_COOR                              radius;               ///< Object corners radius.
          nullptr,              // D4D_MARGIN*                           pMargin;              ///< Object inner margin.
          &inputTargetLabel,    // void*                                 pParam;               ///< The object depends parameters.
          &d4d_labelSysFunc,    // D4D_OBJECT_SYS_FUNCTION*              pObjFunc;             ///< The pointer on object system functions.
          nullptr,              // D4D_ON_USR_MSG                        OnUsrMessage;         ///< The pointer on user message.
          nullptr,              // D4D_OBJECT_USR_DATA                   userData;             ///< The pointer on user data container (user pointer and optionaly parent/children).
          labelRelations,       // D4D_OBJECT_RELATIONS                  pRelations;           ///< Relationship between the objects.
          0,                    ///< The initializations object flags.
          wrapper.scheme,       ///< Pointer on used color scheme.
          &inputTargetData,     ///< Pointer on runtime object data.
      }
    , inputValue{
          {wrapper.cx / 2, 12}, // D4D_POINT position
          {wrapper.cx / 2, 15}, // D4D_SIZE                              size;                 ///< Size of the object.
          0,                    // D4D_COOR                              radius;               ///< Object corners radius.
          nullptr,              // D4D_MARGIN*                           pMargin;              ///< Object inner margin.
          &inputValueLabel,     // void*                                 pParam;               ///< The object depends parameters.
          &d4d_labelSysFunc,    // D4D_OBJECT_SYS_FUNCTION*              pObjFunc;             ///< The pointer on object system functions.
          nullptr,              // D4D_ON_USR_MSG                        OnUsrMessage;         ///< The pointer on user message.
          nullptr,              // D4D_OBJECT_USR_DATA                   userData;             ///< The pointer on user data container (user pointer and optionaly parent/children).
          labelRelations,       // D4D_OBJECT_RELATIONS                  pRelations;           ///< Relationship between the objects.
          0,                    ///< The initializations object flags.
          wrapper.scheme,       ///< Pointer on used color scheme.
          &inputValueData,      ///< Pointer on runtime object data.
      }
    , outputTarget{
          {0, 35},              // D4D_POINT position
          {wrapper.cx / 2, 15}, // D4D_SIZE                              size;                 ///< Size of the object.
          0,                    // D4D_COOR                              radius;               ///< Object corners radius.
          nullptr,              // D4D_MARGIN*                           pMargin;              ///< Object inner margin.
          &outputTargetLabel,   // void*                                 pParam;               ///< The object depends parameters.
          &d4d_labelSysFunc,    // D4D_OBJECT_SYS_FUNCTION*              pObjFunc;             ///< The pointer on object system functions.
          nullptr,              // D4D_ON_USR_MSG                        OnUsrMessage;         ///< The pointer on user message.
          nullptr,              // D4D_OBJECT_USR_DATA                   userData;             ///< The pointer on user data container (user pointer and optionaly parent/children).
          labelRelations,       // D4D_OBJECT_RELATIONS                  pRelations;           ///< Relationship between the objects.
          0,                    ///< The initializations object flags.
          wrapper.scheme,       ///< Pointer on used color scheme.
          &outputTargetData,    ///< Pointer on runtime object data.
      }
    , outputValue{
          {wrapper.cx / 2, 35}, // D4D_POINT position
          {wrapper.cx / 2, 15}, // D4D_SIZE                              size;                 ///< Size of the object.
          0,                    // D4D_COOR                              radius;               ///< Object corners radius.
          nullptr,              // D4D_MARGIN*                           pMargin;              ///< Object inner margin.
          &outputValueLabel,    // void*                                 pParam;               ///< The object depends parameters.
          &d4d_labelSysFunc,    // D4D_OBJECT_SYS_FUNCTION*              pObjFunc;             ///< The pointer on object system functions.
          nullptr,              // D4D_ON_USR_MSG                        OnUsrMessage;         ///< The pointer on user message.
          nullptr,              // D4D_OBJECT_USR_DATA                   userData;             ///< The pointer on user data container (user pointer and optionaly parent/children).
          labelRelations,       // D4D_OBJECT_RELATIONS                  pRelations;           ///< Relationship between the objects.
          0,                    ///< The initializations object flags.
          wrapper.scheme,       ///< Pointer on used color scheme.
          &outputValueData,     ///< Pointer on runtime object data.
      }
    , icons{
          {0, wrapper.cy - 31}, // D4D_POINT position
          {wrapper.cx, 15},     // D4D_SIZE                              size;                 ///< Size of the object.
          0,                    // D4D_COOR                              radius;               ///< Object corners radius.
          nullptr,              // D4D_MARGIN*                           pMargin;              ///< Object inner margin.
          &iconsLabel,          // void*                                 pParam;               ///< The object depends parameters.
          &d4d_labelSysFunc,    // D4D_OBJECT_SYS_FUNCTION*              pObjFunc;             ///< The pointer on object system functions.
          nullptr,              // D4D_ON_USR_MSG                        OnUsrMessage;         ///< The pointer on user message.
          nullptr,              // D4D_OBJECT_USR_DATA                   userData;             ///< The pointer on user data container (user pointer and optionaly parent/children).
          labelRelations,       // D4D_OBJECT_RELATIONS                  pRelations;           ///< Relationship between the objects.
          0,                    ///< The initializations object flags.
          wrapper.scheme,       ///< Pointer on used color scheme.
          &iconsData,           ///< Pointer on runtime object data.
      }
    , lookup(brewbloxBox().makeCboxPtr<PidBlock>(id))
{
    wrapper.addChildren({&inputTarget, &inputValue, &outputTarget, &outputValue, &icons});
    wrapper.setEnabled(D4D_FALSE); // start widget disabled
}

void
PidWidget::drawPidRect(const fp12_t& v, D4D_COOR yPos)
{
    D4D_COOR middle = wrapper.x + wrapper.cx / 2;
    D4D_POINT startP;
    int8_t lenMax = int8_t(wrapper.cx / 2 - 5);

    int8_t lenP = int8_t(v) / 2;
    lenP = std::clamp(lenP, int8_t(-lenMax), lenMax);

    startP = {middle, wrapper.y + yPos};
    D4D_SIZE size = {lenP, 2};

    // clear
    D4D_FillRectXY(middle - lenMax, wrapper.y + yPos, middle + lenMax, wrapper.y + yPos + 2, wrapper.colorScheme.foreDis);
    // draw
    D4D_FillRect(&startP, &size, D4D_COLOR_WHITE);
}

void
PidWidget::drawPidRects(const Pid& pid)
{
    drawPidRect(pid.p(), 55);
    drawPidRect(pid.i(), 59);
    drawPidRect(pid.d(), 63);
}

void
PidWidget::update()
{
    if (auto ptr = lookup.const_lock()) {
        auto pid = ptr->get();
        auto inputLookup = ptr->getInputLookup();
        auto outputLookup = ptr->getOutputLookup();
        setConnected();
        auto input = inputLookup.const_lock();
        if (input && input->valueValid()) {
            setInputValue(temp_to_string(input->value(), 1).c_str());
        } else {
            setInputValue(nullptr);
        }
        if (input && input->settingValid()) {
            setInputTarget(temp_to_string(input->setting(), 1).c_str());
        } else {
            setInputTarget(nullptr);
        }

        auto output = outputLookup.const_lock();
        if (output && output->valueValid()) {
            setInputValue(temp_to_string(output->value(), 1).c_str());
        } else {
            setInputValue(nullptr);
        }
        if (output && output->settingValid()) {
            setInputTarget(temp_to_string(output->setting(), 1).c_str());
        } else {
            setInputTarget(nullptr);
        }

        drawPidRects(pid);

        char icons[2];

        /*switch (ptr->targetState()) {
        case ActuatorPwm::State::Inactive:
            icons[0] = 0x26;
            break;
        case ActuatorPwm::State::Active:
            icons[0] = 0x27;
            break;
        default:
            break;
        }*/
        icons[0]
            = 0x28;
        icons[1] = 0;
        setIcons(icons);
        return;
    }
    setDisconnected();
}
