/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "app_task.h"

#include "battery.h"
#include "buzzer.h"

#include "app/matter_init.h"
#include "app/task_executor.h"
#include "board/board.h"
#include "board/led_widget.h"

#ifdef CONFIG_CHIP_OTA_REQUESTOR
#include "dfu/ota/ota_util.h"
#endif

#ifdef CONFIG_MCUMGR_TRANSPORT_BT
#include "dfu/smp/dfu_over_smp.h"
#endif


#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <app/server/OnboardingCodesUtil.h>
#include <app/server/Server.h>
#include <app/util/binding-table.h>
#include <controller/InvokeInteraction.h>

#include <platform/CHIPDeviceLayer.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(app, CONFIG_CHIP_APP_LOG_LEVEL);

using namespace ::chip;
using namespace ::chip::DeviceLayer;
using namespace ::chip::app;

#define APPLICATION_BUTTON_MASK DK_BTN2_MSK

namespace
{
enum class FunctionTimerMode { kDisabled, kFactoryResetTrigger, kFactoryResetComplete };
enum class LedState { kAlive, kAdvertisingBle, kConnectedBle, kProvisioned };

#if CONFIG_AVERAGE_CURRENT_CONSUMPTION <= 0
#error Invalid CONFIG_AVERAGE_CURRENT_CONSUMPTION value set
#endif

constexpr size_t kMeasurementsIntervalMs = 3000;
constexpr uint8_t kWindowCoveringControllerEndpointId = 1;

constexpr uint8_t kPowerSourceEndpointId = 0;
constexpr int16_t kMinimalOperatingVoltageMv = 3200;
constexpr int16_t kMaximalOperatingVoltageMv = 4050;
constexpr int16_t kWarningThresholdVoltageMv = 3450;
constexpr int16_t kCriticalThresholdVoltageMv = 3250;
constexpr uint8_t kMinBatteryPercentage = 0;
/* Value is expressed in half percent units ranging from 0 to 200. */
constexpr uint8_t kMaxBatteryPercentage = 200;
/* Battery capacity in uAh */
constexpr uint32_t kBatteryCapacityUaH = 1350000;
/* Average device current consumption in uA */
constexpr uint32_t kDeviceAverageCurrentConsumptionUa = CONFIG_AVERAGE_CURRENT_CONSUMPTION;
/* Fully charged battery operation time in seconds */
constexpr uint32_t kFullBatteryOperationTime = kBatteryCapacityUaH / kDeviceAverageCurrentConsumptionUa * 3600;
/* It is recommended to toggle the signalled state with 0.5 s interval. */
constexpr size_t kIdentifyTimerIntervalMs = 500;

k_timer sMeasurementsTimer;
k_timer sIdentifyTimer;

//const device *sBme688SensorDev = DEVICE_DT_GET_ONE(bosch_bme680);

Identify sIdentifyWindowCoveringController = { chip::EndpointId{ kWindowCoveringControllerEndpointId }, AppTask::OnIdentifyStart,
				  AppTask::OnIdentifyStop, Clusters::Identify::IdentifyTypeEnum::kAudibleBeep };

} /* namespace */

void AppTask::OnIdentifyStart(Identify *)
{
	Nrf::PostTask(
		[] { Nrf::GetBoard().GetLED(Nrf::DeviceLeds::LED2).Blink(Nrf::LedConsts::kIdentifyBlinkRate_ms); });
	k_timer_start(&sIdentifyTimer, K_MSEC(kIdentifyTimerIntervalMs), K_MSEC(kIdentifyTimerIntervalMs));
}

void AppTask::OnIdentifyStop(Identify *)
{
	k_timer_stop(&sIdentifyTimer);
	BuzzerSetState(false);
}

void AppTask::IdentifyTimerHandler()
{
	BuzzerToggleState();
}

// COPIED FROM SWITCH
void AppTask::InitiateActionSwitch(Action action)
{
	Nrf::Matter::BindingHandler::BindingData *data =
		Platform::New<Nrf::Matter::BindingHandler::BindingData>();
	if (data) {
		data->EndpointId = kWindowCoveringControllerEndpointId;
		data->ClusterId = Clusters::WindowCovering::Id;
		data->InvokeCommandFunc = SwitchChangedHandler;
		switch (action) {
		case Action::PressButton:
			data->CommandId = Clusters::WindowCovering::Commands::UpOrOpen::Id;
			break;
		case Action::Rotate:
			data->CommandId = Clusters::WindowCovering::Commands::DownOrClose::Id;
			break;
		case Action::Idle:
			data->CommandId = Clusters::WindowCovering::Commands::DownOrClose::Id;
			break;
		case Action::Tap:
			data->CommandId = Clusters::WindowCovering::Commands::DownOrClose::Id;
			break;
		case Action::UpDown:
			data->CommandId = Clusters::WindowCovering::Commands::DownOrClose::Id;
			break;
		default:
			Platform::Delete(data);
			return;
		}
		data->IsGroup = Nrf::Matter::BindingHandler::IsGroupBound();
		Nrf::Matter::BindingHandler::RunBoundClusterAction(data);
	}
}

// COPIED FROM SWITCH
void AppTask::SwitchChangedHandler(const EmberBindingTableEntry &binding, OperationalDeviceProxy *deviceProxy,
				       Nrf::Matter::BindingHandler::BindingData &bindingData)
{
	//if (binding.type == EMBER_MULTICAST_BINDING && bindingData.IsGroup) {
	//	switch (bindingData.ClusterId) {
	//	case Clusters::OnOff::Id:
	//		OnOffProcessCommand(bindingData.CommandId, binding, nullptr, bindingData);
	//		break;
	//	case Clusters::LevelControl::Id:
	//		LevelControlProcessCommand(bindingData.CommandId, binding, nullptr, bindingData);
	//		break;
	//	default:
	//		ChipLogError(NotSpecified, "Invalid binding group command data");
	//		break;
	//	}
	//} else if (binding.type == EMBER_UNICAST_BINDING && !bindingData.IsGroup) {
	//	switch (bindingData.ClusterId) {
	//	case Clusters::OnOff::Id:
	//		OnOffProcessCommand(bindingData.CommandId, binding, deviceProxy, bindingData);
	//		break;
	//	case Clusters::LevelControl::Id:
	//		LevelControlProcessCommand(bindingData.CommandId, binding, deviceProxy, bindingData);
	//		break;
	//	default:
	//		ChipLogError(NotSpecified, "Invalid binding unicast command data");
	//		break;
	//	}
	//}
}

void AppTask::WindowCoveringActionEventHandler(){
    //EXAMPLE OF LOCK FUNCTIONALITY
        //if (BoltLockMgr().IsLocked()) {
	    //	BoltLockMgr().Unlock(BoltLockManager::OperationSource::kButton);
	    //} else {
	    //	BoltLockMgr().Lock(BoltLockManager::OperationSource::kButton);
	    //}
	//Instance().UpdateClustersState();
}

// COPIED FROM SWITCH
void AppTask::ButtonEventHandler(Nrf::ButtonState state, Nrf::ButtonMask hasChanged)
{
	if ((APPLICATION_BUTTON_MASK & state & hasChanged)) {
		Nrf::PostTask([] { WindowCoveringActionEventHandler(); });
	}
}

void AppTask::UpdatePowerSourceClusterState()
{
	EmberAfStatus status;
	int32_t voltage = BatteryMeasurementReadVoltageMv();
	/* Value is expressed in half percent units ranging from 0 to 200. */
	uint8_t batteryPercentage;
	uint32_t batteryTimeRemaining;
	Clusters::PowerSource::PowerSourceStatusEnum batteryStatus;
	Clusters::PowerSource::BatChargeLevelEnum batteryChargeLevel;
	bool batteryPresent;
	Clusters::PowerSource::BatChargeStateEnum batteryCharged;

	if (voltage < 0) {
		voltage = 0;
		batteryPercentage = 0;
		batteryStatus = Clusters::PowerSource::PowerSourceStatusEnum::kUnavailable;
		batteryPresent = false;

		LOG_ERR("Battery level measurement failed %d", voltage);
	} else {
		batteryStatus = Clusters::PowerSource::PowerSourceStatusEnum::kActive;
		batteryPresent = true;
	}

	if (voltage <= kMinimalOperatingVoltageMv) {
		batteryPercentage = kMinBatteryPercentage;
	} else if (voltage >= kMaximalOperatingVoltageMv) {
		batteryPercentage = kMaxBatteryPercentage;
	} else {
		batteryPercentage = kMaxBatteryPercentage * (voltage - kMinimalOperatingVoltageMv) /
				    (kMaximalOperatingVoltageMv - kMinimalOperatingVoltageMv);
	}

	batteryTimeRemaining = kFullBatteryOperationTime * batteryPercentage / kMaxBatteryPercentage;

	if (voltage < kCriticalThresholdVoltageMv) {
		batteryChargeLevel = Clusters::PowerSource::BatChargeLevelEnum::kCritical;
	} else if (voltage < kWarningThresholdVoltageMv) {
		batteryChargeLevel = Clusters::PowerSource::BatChargeLevelEnum::kWarning;
	} else {
		batteryChargeLevel = Clusters::PowerSource::BatChargeLevelEnum::kOk;
	}

	if (BatteryCharged()) {
		batteryCharged = Clusters::PowerSource::BatChargeStateEnum::kIsCharging;
	} else {
		batteryCharged = Clusters::PowerSource::BatChargeStateEnum::kIsNotCharging;
	}

	status = Clusters::PowerSource::Attributes::BatVoltage::Set(kPowerSourceEndpointId, voltage);
	if (status != EMBER_ZCL_STATUS_SUCCESS) {
		LOG_ERR("Updating battery voltage failed %x", status);
	}

	status = Clusters::PowerSource::Attributes::BatPercentRemaining::Set(kPowerSourceEndpointId, batteryPercentage);
	if (status != EMBER_ZCL_STATUS_SUCCESS) {
		LOG_ERR("Updating battery percentage failed %x", status);
	}

	status = Clusters::PowerSource::Attributes::BatTimeRemaining::Set(kPowerSourceEndpointId, batteryTimeRemaining);
	if (status != EMBER_ZCL_STATUS_SUCCESS) {
		LOG_ERR("Updating battery time remaining failed %x", status);
	}

	status = Clusters::PowerSource::Attributes::BatChargeLevel::Set(kPowerSourceEndpointId, batteryChargeLevel);
	if (status != EMBER_ZCL_STATUS_SUCCESS) {
		LOG_ERR("Updating battery charge level failed %x", status);
	}

	status = Clusters::PowerSource::Attributes::Status::Set(kPowerSourceEndpointId, batteryStatus);
	if (status != EMBER_ZCL_STATUS_SUCCESS) {
		LOG_ERR("Updating battery status failed %x", status);
	}

	status = Clusters::PowerSource::Attributes::BatPresent::Set(kPowerSourceEndpointId, batteryPresent);
	if (status != EMBER_ZCL_STATUS_SUCCESS) {
		LOG_ERR("Updating battery present failed %x", status);
	}

	status = Clusters::PowerSource::Attributes::BatChargeState::Set(kPowerSourceEndpointId, batteryCharged);
	if (status != EMBER_ZCL_STATUS_SUCCESS) {
		LOG_ERR("Updating battery charge failed %x", status);
	}
}

void AppTask::UpdateLedState()
{
	if (!Instance().mGreenLED || !Instance().mBlueLED || !Instance().mRedLED) {
		return;
	}

	Instance().mGreenLED->Set(false);
	Instance().mBlueLED->Set(false);
	Instance().mRedLED->Set(false);

	switch (Nrf::GetBoard().GetDeviceState()) {
	case Nrf::DeviceState::DeviceAdvertisingBLE:
		Instance().mBlueLED->Blink(Nrf::LedConsts::StatusLed::Disconnected::kOn_ms,
					   Nrf::LedConsts::StatusLed::Disconnected::kOff_ms);
		break;
	case Nrf::DeviceState::DeviceDisconnected:
		Instance().mGreenLED->Blink(Nrf::LedConsts::StatusLed::Disconnected::kOn_ms,
					    Nrf::LedConsts::StatusLed::Disconnected::kOff_ms);
		break;
	case Nrf::DeviceState::DeviceConnectedBLE:
		Instance().mBlueLED->Blink(Nrf::LedConsts::StatusLed::BleConnected::kOn_ms,
					   Nrf::LedConsts::StatusLed::BleConnected::kOff_ms);
		break;
	case Nrf::DeviceState::DeviceProvisioned:
		Instance().mRedLED->Blink(Nrf::LedConsts::StatusLed::Disconnected::kOn_ms,
					  Nrf::LedConsts::StatusLed::Disconnected::kOff_ms);
		Instance().mBlueLED->Blink(Nrf::LedConsts::StatusLed::Disconnected::kOn_ms,
					   Nrf::LedConsts::StatusLed::Disconnected::kOff_ms);
		break;
	default:
		break;
	}
}

CHIP_ERROR AppTask::Init()
{

	/* Initialize Matter stack */
	ReturnErrorOnFailure(Nrf::Matter::PrepareServer());

	/* Set references for RGB LED */
	mRedLED = &Nrf::GetBoard().GetLED(Nrf::DeviceLeds::LED1);
	mGreenLED = &Nrf::GetBoard().GetLED(Nrf::DeviceLeds::LED2);
	mBlueLED = &Nrf::GetBoard().GetLED(Nrf::DeviceLeds::LED3);

	if (!Nrf::GetBoard().Init(ButtonEventHandler, UpdateLedState)) {
		LOG_ERR("User interface initialization failed.");
		return CHIP_ERROR_INCORRECT_STATE;
	}

	/* Register Matter event handler that controls the connectivity status LED based on the captured Matter network
	 * state. */
	ReturnErrorOnFailure(Nrf::Matter::RegisterEventHandler(Nrf::Board::DefaultMatterEventHandler, 0));

	//if (!device_is_ready(sBme688SensorDev)) {
	//	LOG_ERR("BME688 sensor device not ready");
	//	return chip::System::MapErrorZephyr(-ENODEV);
	//}


	int ret = BatteryMeasurementInit();
	if (ret) {
		LOG_ERR("Battery measurement init failed");
		return chip::System::MapErrorZephyr(ret);
	}

	ret = BatteryMeasurementEnable();
	if (ret) {
		LOG_ERR("Enabling battery measurement failed");
		return chip::System::MapErrorZephyr(ret);
	}

	ret = BatteryChargeControlInit();
	if (ret) {
		LOG_ERR("Battery charge control init failed");
		return chip::System::MapErrorZephyr(ret);
	}

	ret = BuzzerInit();
	if (ret) {
		LOG_ERR("Buzzer init failed");
		return chip::System::MapErrorZephyr(ret);
	}

	/* Initialize timers */
	k_timer_init(
		&sIdentifyTimer, [](k_timer *) { Nrf::PostTask([] { IdentifyTimerHandler(); }); }, nullptr);
	k_timer_start(&sMeasurementsTimer, K_MSEC(kMeasurementsIntervalMs), K_MSEC(kMeasurementsIntervalMs));

#ifdef CONFIG_MCUMGR_TRANSPORT_BT
	Nrf::GetDFUOverSMP().StartServer();
#endif

	return Nrf::Matter::StartServer();
}

CHIP_ERROR AppTask::StartApp()
{
    //Nrf::Matter::BindingHandler::Init();
	ReturnErrorOnFailure(Init());

	while (true) {
		Nrf::DispatchNextTask();
	}

	return CHIP_NO_ERROR;
}
