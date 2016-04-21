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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <linux/input.h>
#include <cutils/atomic.h>
#include <cutils/log.h>

#include "InputDevice.h"
#include "AppOutput.h"

static pthread_mutex_t pipe_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t pending_flush_mutex = PTHREAD_MUTEX_INITIALIZER;
static int pending_flush = 0;
static int pipe_fds[2];

InputDevice::InputDevice(char* device_name, device_callbacks* dc, void* is) :
	mDeviceFd(-1),
	mPipeActive(false),
	mPendingFlush(0),
	mDeviceInitialized(false),
	mStartPolling(false) {

	int ret;
	mDeviceName = device_name;
	mDeviceCallbacks = dc;
	mInputService = is;

	LOGD("%s has been injected, now doing initialization.\n", device_name);
	ret = Init();
	if (ret < 0) {
		LOGE("%s cannot be opened.\n", mDeviceName);
	}
}

int InputDevice::Init()
{
	mDeviceFd = OpenInputDevice(mDeviceName);
	if (mDeviceFd < 0) {
		LOGE("Cannot open this device\n");
		return -1;
	}

	mDeviceInitialized = true;

	return 0;
}

int InputDevice::StartPolling(void)
{
	if (!mDeviceInitialized) {
		LOGE("this device %s has never initialized.\n", mDeviceName);
		return -1;
	} else if (mStartPolling) {
		LOGE("device %s is already polling.\n", mDeviceName);
		return -2;
	}

	mStartPolling = true;
	pthread_create(&mPollThread, NULL, DevicePollThreadStartRoutine, this);
	return 0;
}

int InputDevice::StopPolling(void)
{
	mStartPolling = false;
	/* TODO: waiting for thread to stop */
	return 0;
}

void* InputDevice::DevicePollThreadStartRoutine(void* data) {
    reinterpret_cast<InputDevice*>(data)->PollLoop();
    return NULL;
}

void InputDevice::PollLoop(void) {
	struct input_event event;
	sensors_event_t event_in_progress;
	int ret;
	int current_count = 0;
	char buf = 0;
	bool event_read_in_progress = false;
	char data[60];

	if (mDeviceFd < 0)
		return;

	if (!mPipeActive && SetupPipes()) {
		LOGE("ERROR setting up pipe");
		return;
	}

	// Do event listen and dispatch
	while(mStartPolling) {
        struct pollfd poll_fds[2] = {{ pipe_fds[0], POLLIN, 0 }, { mDeviceFd, POLLIN, 0 }};
        ret = poll(poll_fds, 2, -1);
        if (poll_fds[0].revents & POLLIN) {
            /* flush event came in, read to clear pipe */
            read(pipe_fds[0], &buf, 1);

            /* process any and all pending flush completion event */
            pthread_mutex_lock(&pending_flush_mutex);

            /* if we're in the middle of receiving a event, add completion to the front of the queue */
            if (event_read_in_progress) {
                /* store event when read in progress */
            }

            pthread_mutex_unlock(&pending_flush_mutex);

            if (!event_read_in_progress)
                 return;

            continue;
        } else if (poll_fds[1].revents & POLLIN) {
		    ret = read(mDeviceFd, &event, sizeof(event));
        } else {
            LOGE("%s: wrong event.\n", mDeviceName);
            continue;
        }

		if (event.type < EV_MAX) {
            event_read_in_progress = true;
			/* callback here */
			mDeviceCallbacks->event_handler(mDeviceCallbacks, this, mInputService, event.type, event.code, event.value);
			event_read_in_progress = false;
        }
	}

	if (!mStartPolling) {
		LOGW("mStartPolling not set, stopping looper.");
	}
}

int InputDevice::SetupPipes()
{
    int flags;
    if (mDeviceFd < 0)
        return -EINVAL;

    if (!mPipeActive) {
        // Set tilt event file to be non-blocking
        flags = fcntl(mDeviceFd, F_GETFL, 0);
        fcntl(mDeviceFd, F_SETFL, flags | O_NONBLOCK);

        // Setup the read-interrupt mechanism
        if (pipe(pipe_fds) == -1) {
            printf("ERROR !  pipe() failed, err = %d", errno);
            return(-errno);
        }
        mPipeActive = true;
    }

    return 0;
}

int InputDevice::OpenInputDevice(const char *sensor_device_name)
{
	char *filename;
	int fd;
	DIR *dir;
	struct dirent *de;
	char name[80];
	char devname[256];
	#define INPUT_DIR           "/dev/input"

	dir = opendir(INPUT_DIR);
	if (dir == NULL)
		return -1;

	strcpy(devname, INPUT_DIR);
	filename = devname + strlen(devname);
	*filename++ = '/';

	while ((de = readdir(dir))) {
		if (de->d_name[0] == '.' &&
		    (de->d_name[1] == '\0' ||
		     (de->d_name[1] == '.' && de->d_name[2] == '\0')))
			continue;
		strcpy(filename, de->d_name);
		fd = open(devname, O_RDONLY);
		if (fd < 0)
			continue;

		if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
			name[0] = '\0';
		}

		if (!strcmp(name, sensor_device_name)) {
			LOGD("devname is %s \n", devname);
		} else {
			close(fd);
			continue;
		}
		closedir(dir);

		return fd;

	}
	closedir(dir);

	return -1;
}
