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
#include <linux/input.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>

#include "libjoshgame/libjoshgame.h"
#include "sevenknight/SevenKnight.h"

bool capture_mode_enable = false;
static bool event_handle_in_progress = false;

CoreService* mCoreService;
InputService* mInputService;
CaptureService *mCaptureService;
SevenKnight *mSevenKnight;
GameJobMap *mSevenKnightJobMap;

/* callbacks */
void ic_error_handler(input_callbacks* ic, ErrorType err);
void ic_jobdone_handler(input_callbacks* ic, int jobid);
void ic_event_handler(input_callbacks* ic, CallbackEvent event);

void dump_screen(void);
void init(void);

static input_callbacks mInputCallbacks = {
	.error_handler   = ic_error_handler,
	.jobdone_handler = ic_jobdone_handler,
	.event_handler   = ic_event_handler,
};

void ic_error_handler(input_callbacks* ic, ErrorType err)
{
	if (ic != &mInputCallbacks) {
		LOGE("Callback mismatched! abort here.\n");
		return;
	}
	
	LOGW("InputService returns an error, type = %d\n", err);
}

void ic_jobdone_handler(input_callbacks* ic, int jobid)
{
	if (ic != &mInputCallbacks) {
		LOGE("Callback mismatched! abort here.\n");
		return;
	}
	
	if (jobid > 0) {
		LOGD("Job ID %d is finished.\n", jobid);
	}
}

void ic_event_handler(input_callbacks* ic, CallbackEvent event)
{
	int stage = 0;
	static bool enable_screen = true;

	if (ic != &mInputCallbacks) {
		LOGE("Callback mismatched! abort here.\n");
		return;
	}

	switch(event) {
	case EVENT_REGISTERED:
		LOGD("input service has registered.\n");
		break;
	case EVENT_UNREGISTERED:
		LOGD("EVENT_UNREGISTERED\n");
		break;
	case EVENT_VOLUMN_UP_PRESSED:
		LOGD("volumn up hit.\n");
		LOGI("音量上鍵觸發");
		if (capture_mode_enable) {
			LOGD("capture mode enabled, do dump screen\n");
			LOGI("擷取模式 長按下鍵取消");
			dump_screen();
			break;
		} else if (!event_handle_in_progress) {
			event_handle_in_progress = true;
			
			stage = mCoreService->GetInt(BATTLE_STAGE, -1);
			if (stage < 0 || stage >= mSevenKnightJobMap->jobCount) {
				LOGD("No game stage set ... use 0 instead. \n");
				LOGI("尚未設定遊戲場景，使用一般戰鬥");
				stage = 0;
			} else {
				LOGD("Going stage %d\n", stage);
			}

			if (mSevenKnight->GetJobStatus(stage) == CTS_RUNNING) {
				/* kill the thread */
				LOGD("Killing thread ... \n");
				LOGI("結束自動戰鬥");
				mSevenKnight->StopJob(stage);
			} else {
				LOGI("開始自動戰鬥");
				mSevenKnight->StartJob(stage);
			}
			event_handle_in_progress = false;
		}

		break;
	case EVENT_VOLUMN_DOWN_PRESSED:
		LOGD("volumn down hit.\n");
		LOGI("音量下鍵觸發開關螢幕");
		if (capture_mode_enable) {
			LOGD("capture mode enabled, do dump screen.\n");
			LOGI("擷取模式 長按下鍵取消");
			dump_screen();
		} else if (!event_handle_in_progress) {
			event_handle_in_progress = true;

			/* do thing here */
			if (enable_screen) {
				mInputService->SetBacklight(0);
				enable_screen = false;
			} else {
				mInputService->SetBacklight(12);
				enable_screen = true;
			}

			event_handle_in_progress = false;
		}

		break;
	case EVENT_VOLUMN_UP_LONG_PRESSED:
		LOGD("volumn up long pressed, changing hero triggerred.\n");
		LOGI("長按音量下 開關擷取模式");
		capture_mode_enable = !capture_mode_enable;
		break;
	case EVENT_VOLUMN_DOWN_LONG_PRESSED:
		LOGD("volumn down long pressed, trigger dump screen.\n");
		LOGI("長按音量上 開關擷取模式");
		capture_mode_enable = !capture_mode_enable;
		break;
	case EVENT_POWER_KEY_PRESSED:
		LOGD("power key hit.\n");
		mSevenKnight->StopAllJobs();
		mInputService->ConfigTouchScreen(true);
		break;
	default:
		LOGW("Unknown event %d\n", event);
		break;
	}
}

void dump_screen(void)
{
	static int i = 0;
	char filename[100];
	
	snprintf(filename, 100, "/data/joshtool/%d.dump", i++);
	mCaptureService->DumpScreen(filename);
}

void init(void)
{
	mInputService = new InputService();
	mInputService->Init();
	mInputService->SetCallback(&mInputCallbacks);

	mCaptureService = new CaptureService();
	mCoreService = new CoreService();

	mSevenKnight = new SevenKnight();
	mSevenKnightJobMap = mSevenKnight->GetJobMap();
}

void loop_forever(void)
{
	for(;;) {sleep(100);}
}

int main(int argc, char **argv)
{
	init();
	loop_forever();

	/* this should never happen .. */
	return 0;
}
