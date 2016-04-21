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

#ifndef _INPUTDEVICE_H_
#define _INPUTDEVICE_H_

struct device_callbacks {
    /*
     * When polling event happened, this could be EV_SYN, EV_KEY,
     * EV_REL, EV_ABS, EV_MSC, EV_SW, EV_LED, EV_SND, EV_REP, EV_FF
     * EV_PWR or EV_FF_STATUS.
     * EV_MAX, EV_CNT can be useful.
     */
    void (*event_handler)(device_callbacks*, void* self, void* caller, int, int, int);
};


class InputDevice {
  public:
	InputDevice(char* device_name, device_callbacks* dc, void* is);
	int Init();
	int StopPolling(void);
	int StartPolling(void);

    int mDeviceFd;
    bool mPipeActive;
    char* mDeviceName;
    int mPendingFlush;
	int mPipeFd[2];
	bool mDeviceInitialized;
	bool mStartPolling;

  private:
	device_callbacks* mDeviceCallbacks;
	void* mInputService;
	pthread_t mPollThread;

	void InputEventListenerLoop(void);
	static void* DevicePollThreadStartRoutine(void* data);
	int SetupPipes();
	void PollLoop(void);
	int OpenInputDevice(const char *sensor_device_name);
};

#endif
