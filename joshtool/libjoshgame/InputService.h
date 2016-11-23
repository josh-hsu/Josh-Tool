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

#ifndef _INPUTSERVICE_H_
#define _INPUTSERVICE_H_

#include <pthread.h>
#include "InputDevice.h"
#include "CaptureService.h"

#define INPUT_DEVICE_MAX	7

enum InputType {
	INPUT_TYPE_TAP,
	INPUT_TYPE_DOUBLE_TAP,
	INPUT_TYPE_TRIPLE_TAP,
	INPUT_TYPE_CONT_TAPS,
	INPUT_TYPE_SWIPE,
	INPUT_TYPE_MAX
};

enum KeyType {
	KEY_TYPE_POWER,
	KEY_TYPE_VOLUMN_UP,
	KEY_TYPE_VOLUMN_DOWN,
	KEY_TYPE_HOME,
	KEY_TYPE_BACK,
	KEY_TYPE_FUNCTION_KEY,
	KEY_TYPE_MAX
};

enum CallbackEvent {
	EVENT_REGISTERED,
	EVENT_UNREGISTERED,
	EVENT_VOLUMN_UP_PRESSED,
	EVENT_VOLUMN_UP_DOUBLE_PRESSED,
	EVENT_VOLUMN_UP_LONG_PRESSED,
	EVENT_VOLUMN_UP_LONG_LONG_PRESSED,
	EVENT_VOLUMN_DOWN_PRESSED,
	EVENT_VOLUMN_DOWN_DOUBLE_PRESSED,
	EVENT_VOLUMN_DOWN_LONG_PRESSED,
	EVENT_VOLUMN_DOWN_LONG_LONG_PRESSED,
	EVENT_VOLUMN_ALL_PRESSED,
	EVENT_POWER_KEY_PRESSED,
	EVENT_POWER_KEY_LONG_PRESSED,
	EVENT_MAX
};

enum ErrorType {
	ERROR_MAX
};

struct input_callbacks {
    // InputService calls error_handler when a specfic job failed
    void (*error_handler)(input_callbacks*, ErrorType err);

    // Input job is done and callback is required
    void (*jobdone_handler)(input_callbacks*, int);

    // When all jobs are done or internal error occurred
    void (*event_handler)(input_callbacks*, CallbackEvent);
};

class InputService {
  public:
	InputService();
	InputDevice* mInputDevices[INPUT_DEVICE_MAX];
	input_callbacks* mCallbacks;

	void Init();
	void SetCallback(input_callbacks*);

	// Async job will return immediately with jobID, when the job is done
	// jobdone_handler will get called.

	// touch functions
	int TouchOnScreen(int x, int y, int tx, int ty, InputType type);
	int TouchOnScreenAsync(int x, int y, int tx, int ty, InputType type);
	int TapOnScreen(ScreenCoord* coord1);
	int TapOnScreen(ScreenCoord* coord1, int drift);
	int TapOnScreen(ScreenCoord* coord1, int drift_x, int drift_y);
	int TapOnScreenUntilColorChanged(ScreenPoint* point, int interval, int retry);
	int TapOnScreenUntilColorChangedTo(ScreenPoint* point,
			ScreenPoint* to, int interval, int retry);

	// button key functions
	int PressButtonKey(KeyType type);
	int PressButtonKeyAsync(KeyType type);

	// input text function
	void InputText(char* text);

	// test functions
	void TestInputService(void);
	void TestInputServiceSchedule(int interval);
	void ConfigTouchScreen(bool enable);  // device-specific function
	void ConfigGyroSensor(bool enable);   // device-specific function
	void SetBacklightLow(void);
	void SetBacklight(int bl);

  private:
    int mTestInterval;
    int mTestSelection;
    CaptureService* mCaptureService;

    void TestInputServiceInternal(int interval, int selection, bool enable);
};

#endif
