/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#pragma once


#include "binding/binding_handler.h"
#include "board/led_widget.h"
#include "board/board.h"

#include <atomic>
#include <app/util/basic-types.h>
#include <lib/core/CHIPError.h>
#include <app/clusters/identify-server/identify-server.h>
#include <platform/CHIPDeviceLayer.h>

class AppTask {
public:
	static AppTask &Instance()
	{
		static AppTask sAppTask;
		return sAppTask;
	};

	CHIP_ERROR StartApp();

	static void OnIdentifyStart(Identify *);
	static void OnIdentifyStop(Identify *);

	static void UpdateLedState();

    enum class Action : uint8_t {
	    PressButton, /* The button of the device is pressed. */
	    Rotate, /* The device is rotated. */
	    Tap, /* The device is tapped while placed on a flat surface. */
	    Idle, /* The device is placed on a flat surface. */
	    UpDown /* The device is moved in updown direction. */
	};

	void InitiateActionSwitch(Action action);
	static void SwitchChangedHandler(const EmberBindingTableEntry &binding,
					 chip::OperationalDeviceProxy *deviceProxy,
					 Nrf::Matter::BindingHandler::BindingData &bindingData);

    static void ButtonEventHandler(Nrf::ButtonState state, Nrf::ButtonMask hasChanged);
    static void WindowCoveringActionEventHandler();
private:
	CHIP_ERROR Init();

	void UpdatePowerSourceClusterState();

	static void MeasurementsTimerHandler();
	static void IdentifyTimerHandler();

	Nrf::LEDWidget *mRedLED;
	Nrf::LEDWidget *mGreenLED;
	Nrf::LEDWidget *mBlueLED;
};
