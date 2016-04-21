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

#ifndef _CORETHREAD_H_
#define _CORETHREAD_H_

#include <pthread.h>

enum CoreThreadType {
	CT_ONE_TIME,
	CT_FOREVER,
	CT_CONDITIONAL_STOP,
	CT_MAX
};

enum CoreThreadStatus {
	CTS_PREPARED,
	CTS_STARTING,
	CTS_RUNNING,
	CTS_TERMINATED,
	CTS_MAX
};

class CoreThread {
  public:
	CoreThread(void *(*func)(void* data), int hd);
	CoreThread(void *(*func)(void* data), int hd, void (*cb_func)(void));

	void Start(CoreThreadType type);
	int Pause();
	int Resume();
	int Stop();
	CoreThreadStatus GetStatus(void);

  private:
	pthread_t mThreadId;
	void *(*mFunction)(void* data);
	void (*mJobDoneCallback)(void);
	int mHandle;
	CoreThreadType mThreadType;
	CoreThreadStatus mCurrentStatus;

	static void* MainThreadRoutine(void* data);
};

#endif
