/*
 * Copyright (C) 2016 The Josh Tool Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <sys/klog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>

#include <hardware/sensors.h>
#include <linux/input.h>
#include <cutils/atomic.h>
#include <cutils/log.h>
#include <poll.h>

#include "InputService.h"
#include "AppOutput.h"

/* TODO: this should be provided by devices.xml */
static int device_count = 2;
static char* device_name[2] = {/*"elan-touchscreen", "lid_input", */"qpnp_pon", "gpio-keys"};
static int mGameOrientation = SO_Landscape;
static int mScreenWidth = 720;
static int mScreenHeight = 1280;

static void dc_event_handler(device_callbacks* dc, void* self, void* caller, int event_type, int event_code, int event_data);
static device_callbacks mDeviceCallbacks = {
	.event_handler = dc_event_handler,
};

InputService::InputService() :
	mCallbacks(NULL),
	mTestInterval(10),
	mTestSelection(1) {

	LOGD("InputService instance is created. ");
	mCaptureService = new CaptureService();
}

void InputService::Init() {
	int ret = 0;

	/* if total devices is more than our acceptible devices, then ignore them */
	if (device_count > INPUT_DEVICE_MAX)
		device_count = INPUT_DEVICE_MAX;
	
	/* do initialize all input devices */
	for(int i = 0; i < device_count; i++) {
		mInputDevices[i] = new InputDevice(device_name[i], &mDeviceCallbacks, this);
		ret = mInputDevices[i]->StartPolling();
		if (ret < 0) {
			LOGW("%s: this device cannot be polled.", device_name[i]);
		}
	}

	LOGD("InputService is initialized.");
}

void InputService::SetCallback(input_callbacks* ic) {
	if (ic == NULL) {
		LOGE("Your callback function is null.");
		return;
	}
	mCallbacks = ic;
	mCallbacks->event_handler(mCallbacks, EVENT_REGISTERED);
}


int InputService::TouchOnScreen(int x, int y, int tx, int ty, InputType type) {
	char tap_command[100];

	switch (type) {
	case INPUT_TYPE_TAP:
		snprintf(tap_command, 100, "input tap %d %d", x, y);
		std::system(tap_command);
		break;
	case INPUT_TYPE_DOUBLE_TAP:
		snprintf(tap_command, 100, "input tap %d %d", x, y);
		std::system(tap_command);
		usleep(100 * 1000);
		std::system(tap_command);
		break;
	case INPUT_TYPE_TRIPLE_TAP:
		snprintf(tap_command, 100, "input tap %d %d", x, y);
		std::system(tap_command);
		usleep(100 * 1000);
		std::system(tap_command);
		usleep(100 * 1000);
		std::system(tap_command);
		break;
	case INPUT_TYPE_CONT_TAPS:
		break;
	case INPUT_TYPE_SWIPE:
		snprintf(tap_command, 100, "input swipe %d %d %d %d", x, y, tx, ty);
		std::system(tap_command);
		break;	
	default:
		LOGW("TouchOnScreen: type %d is invalid.", type);
	}
	return 0;
}

int InputService::TapOnScreen(ScreenCoord* coord1)
{
	char tap_command[100];

	if (mGameOrientation != coord1->orien)
		snprintf(tap_command, 100, "input tap %d %d", coord1->y, mScreenWidth - coord1->x);
	else
		snprintf(tap_command, 100, "input tap %d %d", coord1->x, coord1->y);

	std::system(tap_command);
	usleep(10);
	return 0;
}

int InputService::TapOnScreen(ScreenCoord* coord1, int drift)
{
	return TapOnScreen(coord1, drift, drift);
}

int InputService::TapOnScreen(ScreenCoord* coord1, int drift_x, int drift_y)
{
	char tap_command[100];
	int x_diff, y_diff;

	//reset random seed
	srand((unsigned)time(NULL));
	x_diff = rand() % (2 * drift_x);
	y_diff = rand() % (2 * drift_y);
	x_diff -= drift_x;
	y_diff -= drift_y;

	if (mGameOrientation != coord1->orien)
		snprintf(tap_command, 100, "input tap %d %d", coord1->y + y_diff, mScreenWidth - coord1->x + x_diff);
	else
		snprintf(tap_command, 100, "input tap %d %d", coord1->x + x_diff, coord1->y + y_diff);

	std::system(tap_command);
	usleep(10);
	return 0;
}

/*
 * Tap on screen amount of times until the color on the screen is not in point->color
 */
int InputService::TapOnScreenUntilColorChanged(ScreenPoint* point, int interval, int retry)
{
	if (point == NULL) {
		LOGW("InputService: null point.");
		goto not_found;
	}

	while(retry-- > 0) {
		TapOnScreen(&point->coord);
		usleep(interval * 10);
		if (mCaptureService->ColorIs(point)) {
			LOGD("InputService: color didn't change, try again? [%s]", retry > 0 ? "YES" : "NO");
		} else {
			LOGD("InputService: color changed. exiting..");
			return 0;
		}
	}

not_found:
	return -1;
}

int InputService::TapOnScreenUntilColorChangedTo(ScreenPoint* point,
		ScreenPoint* to, int interval, int retry)
{
	if ((point == NULL) || (to == NULL)) {
		LOGW("InputService: null points.");
		goto not_found;
	}

	while(retry-- > 0) {
		TapOnScreen(&point->coord);
		usleep(interval * 10);
		if (mCaptureService->ColorIs(to)) {
			LOGD("InputService: color changed to specific point. exiting..");
		} else {
			LOGD("InputService: color didn't change to specific point, try again? [%s]",
				retry > 0 ? "YES" : "NO");
			return 0;
		}
	}

not_found:
	return -1;
}

int InputService::TouchOnScreenAsync(int x, int y, int tx, int ty, InputType type) {
	//TODO: implement required
	TouchOnScreen(x, y, tx, ty, type);
	return 0;
}

// button key functions
int InputService::PressButtonKey(KeyType type) {
	LOGD("PressButtonKey for type %d currently not implemented.", type);
	return 0;
}

int InputService::PressButtonKeyAsync(KeyType type) {
	LOGD("PressButtonKeyAsync for type %d currently not implemented.", type);
	return 0;
}

// input text function
void InputService::InputText(char* text)
{
	char text_command[100];

	snprintf(text_command, 100, "input text %s", text);
	std::system(text_command);
	usleep(10);
}

// test functions
void InputService::TestInputService(void) {
	
}

void InputService::TestInputServiceSchedule(int interval) {
	mTestInterval = interval;
}

void InputService::TestInputServiceInternal(int interval, int selection, bool enable) {
	mTestInterval = interval;
	mTestSelection = selection;
	if (enable) {
		return;
	}
}

void InputService::ConfigTouchScreen(bool enable) {
	if (enable)
		std::system("echo 1 > /proc/touch-enable");
	else
		std::system("echo 0 > /proc/touch-enable");
}

void InputService::ConfigGyroSensor(bool enable) {
	if (enable)
		std::system("echo 1 > /proc/sensor-enable");
	else
		std::system("echo 0 > /proc/sensor-enable");
}

void InputService::SetBacklightLow(void) {
	std::system("echo 0 > /sys/class/leds/lcd-backlight/brightness");
}

void InputService::SetBacklight(int bl) {
	char cmd[128];
	snprintf(cmd, 128, "echo %d > /sys/class/leds/lcd-backlight/brightness", bl);
	std::system(cmd);
}

/*
 * Callback functions and event handle
 */
static void dc_event_handler(device_callbacks* dc, void* self, void* caller,
		int event_type, int event_code, int event_data) {
	InputService* inputService = reinterpret_cast<InputService*>(caller);
	InputDevice* inputDevice = reinterpret_cast<InputDevice*>(self);
	static time_t key_jiffies[3]; /* power key, volumn up, volumn down */
	time_t current_time;

	if (event_type != EV_SYN) {
		LOGD("device %s trigger event %d code %d data %d", inputDevice->mDeviceName, event_type, event_code, event_data);
		/* button pressed, save jiffies */
		if (event_code == KEY_POWER && event_data == 1)
			time(&key_jiffies[0]);
		else if (event_code == KEY_VOLUMEUP && event_data == 1)
			time(&key_jiffies[1]);
		else if (event_code == KEY_VOLUMEDOWN && event_data == 1)
			time(&key_jiffies[2]);

		if (event_code == KEY_VOLUMEDOWN && event_data == 0) {
			time(&current_time);
			if (difftime(current_time, key_jiffies[2]) >= 5)
				inputService->mCallbacks->event_handler(inputService->mCallbacks, EVENT_VOLUMN_DOWN_LONG_LONG_PRESSED);
			else if (difftime(current_time, key_jiffies[2]) >= 2)
				inputService->mCallbacks->event_handler(inputService->mCallbacks, EVENT_VOLUMN_DOWN_LONG_PRESSED);
			else
				inputService->mCallbacks->event_handler(inputService->mCallbacks, EVENT_VOLUMN_DOWN_PRESSED);
		} else if (event_code == KEY_VOLUMEUP && event_data == 0) {
			time(&current_time);
			if (difftime(current_time, key_jiffies[1]) >= 5)
				inputService->mCallbacks->event_handler(inputService->mCallbacks, EVENT_VOLUMN_UP_LONG_LONG_PRESSED);
			else if (difftime(current_time, key_jiffies[1]) >= 2)
				inputService->mCallbacks->event_handler(inputService->mCallbacks, EVENT_VOLUMN_UP_LONG_PRESSED);
			else
				inputService->mCallbacks->event_handler(inputService->mCallbacks, EVENT_VOLUMN_UP_PRESSED);
		} else if (event_code == KEY_POWER && event_data == 0) {
			time(&current_time);
			if (difftime(current_time, key_jiffies[0]) >= 2)
				inputService->mCallbacks->event_handler(inputService->mCallbacks, EVENT_POWER_KEY_LONG_PRESSED);
			else
				inputService->mCallbacks->event_handler(inputService->mCallbacks, EVENT_POWER_KEY_PRESSED);
		}
	}
}
