/*
 * Copyright (C) 2016 Josh Tool Project
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

#include "FGO.h"
#include "fgo_720p_script.h"

char mCurrentRoot[70];

FGO::FGO() {

	/* initialize of CoreThread */
	mMainThread = new CoreThread(keep_main, 0, corethread_jobdone_callback);
	mXAThread = new CoreThread(keep_xb, 0, corethread_jobdone_callback);

	/* initialize of game jobs */
	mGameJob[0].jobThread = mMainThread;
	mGameJob[0].jobName = "main_job";
	mGameJob[1].jobThread = mXAThread;
	mGameJob[1].jobName = "main_job";

	/* initial GameJobMap */
	mJobIdd[0].index = 0;
	mJobIdd[0].jobName = "job_0";
	mJobIdd[1].index = 1;
	mJobIdd[1].jobName = "job_1";
	mGameJobMap.jobCount = FGO_JOB;
	mGameJobMap.jobIdd = mJobIdd;
}

bool FGO::IsIndexLegal(int index)
{
	if (index >= FGO_JOB || index < 0)
		return false;

	return true;
}

GameJobMap* FGO::GetJobMap(void)
{
	return &mGameJobMap;
}

GameJob* FGO::GetJobByIndex(int index)
{
	if (IsIndexLegal(index))
		return &mGameJob[index];
	else
		return NULL;
}

void FGO::StartJob(GameJob* job)
{
	job->jobThread->Start(CT_FOREVER);
}

void FGO::StartJob(int index)
{
	if (IsIndexLegal(index)) {
		GetJobByIndex(index)->jobThread->Start(CT_FOREVER);
	} else
		LOGE("Index %d of game is not legal", index);
}

void FGO::StopJob(GameJob* job)
{
	job->jobThread->Stop();
}

void FGO::StopJob(int index)
{
	if (IsIndexLegal(index)) {
		GetJobByIndex(index)->jobThread->Stop();
	} else
		LOGE("Index %d of game is not legal", index);
}

void FGO::StopAllJobs(void)
{
	return;
	for(int i = 0; i < FGO_JOB; i++) {
		StopJob(i);
	}
}

CoreThreadStatus FGO::GetJobStatus(GameJob* job)
{
	return job->jobThread->GetStatus();
}

CoreThreadStatus FGO::GetJobStatus(int index)
{
	if (IsIndexLegal(index))
		return GetJobByIndex(index)->jobThread->GetStatus();

	return CTS_MAX;
}

void corethread_jobdone_callback(void)
{
	LOGD("CoreThread job is done.");
}

static void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

/*=====================
 * Common function
 *===================== */
/*
 * check for skip
 */
int check_for_skip(void)
{
	int max_skip_check = 5;

check_again:
	if (mCaptureService->WaitOnColor(&skipButtonPoint, 20) < 0) {
		LOGW("skip not found.");
		goto error;
	}
	LOGD("skip found.");

	if (mCaptureService->WaitOnColorKindOf(&skipTextPoint, 15, 0x10) < 0) {
		if (max_skip_check-- > 0) {
			LOGW("skip text not found, check again.");
			goto check_again;
		} else {
			LOGW("can not find skip text in max check time.");
			goto error;
		}
	}
	LOGD("skip text found, can press it");

press_again:
	max_skip_check = 3;
	mInputService->TapOnScreen(&skipButtonPoint.coord, 3);
	mInputService->TapOnScreen(&skipButtonPoint.coord, 3);

	if (mCaptureService->WaitOnColor(&confirmSkipButtonPoint, 10) < 0) {
		if (max_skip_check-- > 0) {
			LOGW("confirm skip button not found, press skip again.");
			goto press_again;
		} else {
			LOGW("skip confirm not found.");
			goto error;
		}
	}
	sleep(2);
	mInputService->TapOnScreen(&confirmSkipButtonPoint.coord, 4);
	mInputService->TapOnScreen(&confirmSkipButtonPoint.coord, 4);
	LOGD("confirm pressed.");
	return 0;

error:
	return -1;
}

/*
 * battle once with session
 * session:
 *    0: no session
 *    1: draw and tap then select card
 *    2: draw and use royal card
 *    3: draw and tap speed up
 *    4: draw and tap monster then select card
 */
int battle_once(int session)
{
	if (session == 4) {
		sleep(5);
		if (mCaptureService->WaitOnColor(&tapMosterTextPoint, 10) < 0) {
			LOGW("cannot tap moster to switch them. do it anyway.");
		}
		mInputService->TapOnScreen(&tapMosterTextPoint.coord);
		sleep(1);
		mInputService->TapOnScreen(&tapMosterButtonPoint.coord);
	}

	if (mCaptureService->WaitOnColor(&drawCardButtonPoint, 80) < 0) {
		LOGW("draw card battle button not found.");
		goto error;
	}
	sleep(2); //fix for some inconsistency

	mInputService->TapOnScreen(&drawCardButtonPoint.coord, 13);
	sleep(3);

draw_card:
	//check if we did press the draw card button for once
	if (mCaptureService->ColorIs(&drawCardButtonPoint)) {
		LOGW("draw card battle button still here press it again for gods sake");
		mInputService->TapOnScreen(&drawCardButtonPoint.coord, 13);
		sleep(2);
	}

	if (session == 1) {
		if (mCaptureService->ColorIs(&touchDismissButtonPoint)) {
			sleep(2);
			mInputService->TapOnScreen(&touchDismissButtonPoint.coord);
		}
	} else if (session == 3) {
		sleep(2);
		mInputService->TapOnScreen(&speedUpButtonPoint.coord, 3);
	} else if (session == 4) {
		if (mCaptureService->WaitOnColor(&tapMosterTextPoint, 10) < 0) {
			LOGD("cannot find tap moster to switch them. we have done before.");
		} else {
			sleep(1);
			mInputService->TapOnScreen(&tapMosterTextPoint.coord);
			mInputService->TapOnScreen(&tapMosterButtonPoint.coord);
			session = 0;
			goto draw_card;
		}
	}

	sleep(2);

	if (session != 2) {
		mInputService->TapOnScreen(&card2ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card1ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card3ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card2ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card1ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card3ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card2ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card1ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card3ButtonPoint.coord, 4, 12);
	} else {
		mInputService->TapOnScreen(&royalCardButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card1ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card2ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&royalCardButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card1ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card2ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&royalCardButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card1ButtonPoint.coord, 4, 12);
		mInputService->TapOnScreen(&card2ButtonPoint.coord, 4, 12);
	}
	LOGD("battle once done");
	return 0;

error:
	return -1;
}

/*
 * standard battle and wait for match point
 */
int battle_till_finish(ScreenPoint* match)
{
	int battle_time_second = 180;
	int ret = 0;

	LOGD("battle till finish +++ ");
	while(battle_time_second > 0) {
		if (mCaptureService->ColorIs(&drawCardButtonPoint)) {
			//battle point
			sleep(1);
			mInputService->TapOnScreen(&drawCardButtonPoint.coord, 15);
			sleep(2);

			//check if we did press the draw card button for once
			if (mCaptureService->ColorIs(&drawCardButtonPoint)) {
				sleep(1);
				LOGW("draw card battle button still here press it again for gods sake");
				mInputService->TapOnScreen(&drawCardButtonPoint.coord, 13);
				sleep(2);
			}

			mInputService->TapOnScreen(&card1ButtonPoint.coord, 4, 12);
			sleep(1);
			mInputService->TapOnScreen(&card2ButtonPoint.coord, 4, 12);
			sleep(1);
			mInputService->TapOnScreen(&card3ButtonPoint.coord, 4, 12);
			sleep(1);
			mInputService->TapOnScreen(&card1ButtonPoint.coord, 4, 12);
			mInputService->TapOnScreen(&card2ButtonPoint.coord, 4, 12);
			mInputService->TapOnScreen(&card3ButtonPoint.coord, 4, 12);
			battle_time_second = 180;
		} else if (mCaptureService->ColorIs(match)) {
			goto battle_done;
		}
		battle_time_second--;
		sleep(1);
	}

	if (battle_time_second <= 0) {
		LOGW("Battle timeout!");
		return -1;
	}

battle_done:
	//matched!
	LOGD("dismiss battle result");
	battle_time_second = 12;
	while(!mCaptureService->ColorIs(&nextButtonPoint) && battle_time_second >0) {
		mInputService->TapOnScreen(&bondButtonPoint.coord, 40);
		sleep(1);
		battle_time_second--;
	}

	mInputService->TapOnScreen(&nextButtonPoint.coord, 5, 2);

	LOGD("battle till finished --- ");
	return 0;
}

/*=====================
 * Sub functions
 *===================== */
/*
 * intro-presetup
 */
int intro_presetup(void)
{
	int max_wait_time = 250;

	while (!mCaptureService->ColorIs(&homePageButtonPoint)) {
		// if server cannot let us create new account delay a little bit
		if (mCaptureService->ColorIs(&serverErrorButtonPoint)) {
			LOGW("server error, wait for 15 ms");
			sleep(15);
			mInputService->TapOnScreen(&serverErrorButtonPoint.coord, 5);
			mSettingService->KillFGO();
			return -9;
		}
		sleep(1);
		max_wait_time--;
	}

	if (max_wait_time == 0) {
		LOGW("not in home page.");
		goto error;
	}
	sleep(2); //to prevent pressing button too early

	// tap on screen to create an id
	mInputService->TapOnScreen(&homePageButtonPoint.coord, 5);

	if (mCaptureService->WaitOnColor(&agreementButtonPoint, 50) < 0) {
		LOGW("agreement not shown.");
	} else {
		mInputService->TapOnScreen(&agreementButtonPoint.coord, 3);
	}

	return 0;

error:
	return -1;
}

/*
 * intro battle
 */
int intro_battle(void)
{
	int ret = 0;

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(40); //enter battle may take a long time

	ret = battle_once(0);
	if (ret < 0) goto error;

	ret = battle_once(0);
	if (ret < 0) goto error;

	ret = battle_once(0);
	if (ret < 0) goto error;

	ret = battle_once(1);
	if (ret < 0) goto error;

	ret = battle_once(2);
	if (ret < 0) goto error;

	sleep(30); //workaround for probably stucking
	ret = check_for_skip();
	if (ret < 0) goto error;

	return 0;
error:
	return -1;
}

int create_character(void)
{
	int ret;
	char name[8];

	gen_random(name, 7);

	if (mCaptureService->WaitOnColor(&touchNameButtonPoint, 50) < 0) {
		LOGW("name field button not found.");
		goto error;
	}
	mInputService->TapOnScreen(&touchNameButtonPoint.coord);
	sleep(3);
	mInputService->InputText(name);
	sleep(3);
	mInputService->TapOnScreen(&returnButtonPoint.coord);
	sleep(3);
	// confrim
	mInputService->TapOnScreen(&enterNameButtonPoint.coord);
	sleep(3);
	mInputService->TapOnScreen(&confirmNameButtonPoint.coord);

	ret = check_for_skip();
	if (ret < 0) goto error;

	return 0;

error:
	return -1;
}

/*
 * X - A
 */
int XA_battle(void)
{
	int ret = 0;
	int tap_count = 10;

tap_again:
	if (mCaptureService->WaitOnColorKindOf(&inZoneSelectionPoint, 90, 0x10) < 0) {
		LOGW("not in zone selection.");
		goto error;
	}
	sleep(1);
	mInputService->TapOnScreen(&enterZone1ButtonPoint.coord);
	sleep(2);

	if (!mCaptureService->ColorKindOf(&enterZone1SubButtonPoint, 0x10)) {
		LOGW("zone 1 sub button not found. tap zone again");
		if (tap_count-- > 0)
			goto tap_again;
		else
			goto error;
	}
	sleep(2);
	mInputService->TapOnScreen(&enterZone1SubButtonPoint.coord, 2, 5);
	sleep(5);

	ret = check_for_skip();
	if (ret < 0) goto error;

	ret = battle_once(0);
	if (ret < 0) goto error;
	sleep(20);

	// wait for skill tutor
	if (mCaptureService->WaitOnColor(&skillTutorTextPoint, 30) < 0) {
		LOGW("skill tutor button not found.");
		goto error;
	}
	sleep(1);
	mInputService->TapOnScreen(&skill1ButtonPoint.coord);
	sleep(2);
	mInputService->TapOnScreen(&useSkillButtonPoint.coord, 3);
	sleep(2);
	mInputService->TapOnScreen(&skillTargetButtonPoint.coord, 5);
	sleep(2);

	ret = battle_once(3);
	if (ret < 0) goto error;

	ret = battle_till_finish(&bondButtonPoint);
	if (ret < 0) goto error;

	ret = check_for_skip();
	if (ret < 0) goto error;

	// wait for magic stone
	if (mCaptureService->WaitOnColor(&magicStoneButtonPoint, 30) < 0) {
		LOGW("magic stone button not found.");
		goto error;
	}
	mInputService->TapOnScreen(&magicStoneButtonPoint.coord, 20);
	return 0;

error:
	return -1;
}

/*
 * X -B
 */
int XB_battle(void)
{
	int ret = 0;
	int tap_count = 10;

tap_again:
	if (mCaptureService->WaitOnColorKindOf(&inZoneSelectionPoint, 90, 0x10) < 0) {
		LOGW("not in zone selection.");
		goto error;
	}
	sleep(1);
	mInputService->TapOnScreen(&enterZone2ButtonPoint.coord);
	sleep(2);

	if (!mCaptureService->ColorKindOf(&enterZone2SubButtonPoint, 0x10)) {
		LOGW("zone 2 sub button not found. tap zone again");
		if (tap_count-- > 0)
			goto tap_again;
		else
			goto error;
	}
	sleep(2);
	mInputService->TapOnScreen(&enterZone2SubButtonPoint.coord, 3);

	ret = check_for_skip();
	if (ret < 0) goto error;

	LOGD("battle once 0");
	ret = battle_once(0);
	if (ret < 0) goto error;
	sleep(18);

	LOGD("battle once 4");
	ret = battle_once(4);
	if (ret < 0) goto error;

	LOGD("battel till finish entering");
	ret = battle_till_finish(&bondButtonPoint);
	if (ret < 0) goto error;

	ret = check_for_skip();
	if (ret < 0) goto error;

	// wait for magic stone
	LOGD("waititng for magic stone");
	if (mCaptureService->WaitOnColor(&magicStoneButtonPoint, 30) < 0) {
		LOGW("magic stone button not found.");
		goto error;
	}
	mInputService->TapOnScreen(&magicStoneButtonPoint.coord, 20);
	return 0;

error:
	return -1;
}

/*
 * first 10 summon
 */
int first_ten_summon(void)
{
	int ret = 0;
	int max_round = 82000;
	char screenshot_path[250];

	snprintf(screenshot_path, 250, "%s/ten.png", mCurrentRoot);

	if (mCaptureService->WaitOnColor(&menuButtonPoint, 10) < 0) {
		LOGW("menuButtonPoint not found.");
		goto error;
	}
	mInputService->TapOnScreen(&menuButtonPoint.coord, 2);
	sleep(3);
	mInputService->TapOnScreen(&summonButtonPoint.coord, 3);
	sleep(6);
	mInputService->TapOnScreen(&tenDrawButtonPoint.coord, 4);
	sleep(3);
	mInputService->TapOnScreen(&confirmDrawButtonPoint.coord, 3);
	sleep(3);
	LOGD("start waiting for servant introduce");
	//wait for servant introduce themselve
	while(!mCaptureService->ColorIs(&summonDoneSummonButtonPoint) && (max_round > 0)) {
		mInputService->TapOnScreen(&summonDoneButtonPoint.coord, 10, 4);
		sleep(1);
		max_round--;
	}
	LOGD("end waiting");
	mCaptureService->SaveScreenshot(screenshot_path);
	sleep(1);
	mInputService->TapOnScreen(&summonDoneSummonButtonPoint.coord, 2);
	sleep(1);
	mInputService->TapOnScreen(&summonDoneSummonButtonPoint.coord, 2);
	sleep(3);

	mInputService->TapOnScreen(&menuButtonPoint.coord, 2);
	sleep(3);
	mInputService->TapOnScreen(&teamButtonPoint.coord, 4, 2);
	sleep(3);
	mInputService->TapOnScreen(&partyButtonPoint.coord, 4, 2);
	sleep(3);
	mInputService->TapOnScreen(&member2ButtonPoint.coord, 2);
	sleep(3);
	if (mCaptureService->WaitOnColor(&memberTutorTextPoint, 10) < 0) {
		LOGW("memberTutorTextPoint not found.");
		goto error;
	}
	mInputService->TapOnScreen(&selectServantButtonPoint.coord, 30);
	sleep(2);
	mInputService->TapOnScreen(&selectServantButtonPoint.coord, 3, 5);
	sleep(5);
	mInputService->TapOnScreen(&partyConfigDoneButtonPoint.coord, 2);
	sleep(10);
	mInputService->TapOnScreen(&partyCloseButtonPoint.coord, 2);
	sleep(3);
	mInputService->TapOnScreen(&teamCloseButtonPoint.coord, 2);
	sleep(3);
	return 0;
error:
	return -1;
}

/*
 * X - C1
 */
int XC1_battle(void)
{
	int ret = 0;
	int tap_count = 10;

tap_again:
	if (mCaptureService->WaitOnColorKindOf(&inZoneSelectionPoint, 90, 0x10) < 0) {
		LOGW("not in zone selection.");
		goto error;
	}
	sleep(1);
	mInputService->TapOnScreen(&enterZone3ButtonPoint.coord);
	sleep(2);

	if (!mCaptureService->ColorKindOf(&enterZone2SubButtonPoint, 0x10)) {
		LOGW("zone 3 sub button not found. tap zone again");
		if (tap_count-- > 0)
			goto tap_again;
		else
			goto error;
	}
	sleep(2);
	mInputService->TapOnScreen(&enterZone3SubButtonPoint.coord, 2);
	sleep(3);
	
	if (mCaptureService->WaitOnColor(&selectSupportTextPoint, 40) < 0) {
		LOGW("selectSupportTextPoint not found.");
		goto error;
	}
	LOGD("found select support.");
	mInputService->TapOnScreen(&selectSupportTextPoint.coord, 6);
	sleep(2);

	mInputService->TapOnScreen(&selectSupportButtonPoint.coord, 2);
	sleep(3);

	mInputService->TapOnScreen(&questStartButtonPoint.coord, 2);
	sleep(3);
	LOGD("quest start pressed.");

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(35);

	ret = battle_till_finish(&bondButtonPoint);
	if (ret < 0) goto error;
	sleep(3);

	LOGD("apply friend");
	mInputService->TapOnScreen(&applyFriendButtonPoint.coord, 3, 2);
	sleep(5);

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(20);

	//need change, sometime it will not show up
	if (mCaptureService->WaitOnColor(&welcomeCloseButtonPoint, 40) < 0) {
		LOGW("welcomeCloseButtonPoint not found.");
	}
	sleep(1);
	mInputService->TapOnScreen(&welcomeCloseButtonPoint.coord, 2);
	sleep(3);

	return 0;

error:
	return -1;
}

/*
 * X - C2
 */
int XC2_battle(int speed_up)
{
	int ret = 0;

	if (mCaptureService->WaitOnColorKindOf(&enterZone3SubButtonPoint, 30, 0x10) < 0) {
		LOGW("zone 3 sub not found.");
		goto error;
	}
	sleep(6);
	mInputService->TapOnScreen(&enterZone3SubButtonPoint.coord, 2);
	sleep(4);

	mInputService->TapOnScreen(&selectSupportButtonPoint.coord, 3);
	sleep(4);

	mInputService->TapOnScreen(&questStartButtonPoint.coord, 2);
	sleep(4);

	ret = check_for_skip();
	if (ret < 0) goto error;

	// need to speed up again
	if (speed_up == 1) {
		ret = battle_once(3);
		if (ret < 0) goto error;
	}

	ret = battle_till_finish(&bondButtonPoint);
	if (ret < 0) goto error;
	sleep(3);

	mInputService->TapOnScreen(&applyFriendButtonPoint.coord, 3);
	sleep(5);

	// second round has magic stone and info
	if (speed_up == 0) {
		ret = check_for_skip();

		// wait for magic stone
		if (mCaptureService->WaitOnColor(&magicStoneButtonPoint, 30) < 0) {
			LOGW("magic stone button not found.");
			goto error;
		}
		mInputService->TapOnScreen(&magicStoneButtonPoint.coord, 30);
		sleep(8);

		// tap info
		if (mCaptureService->WaitOnColor(&welcomeCloseButtonPoint, 10) < 0) {
			LOGW("welcomeCloseButtonPoint not found.");
		}
		mInputService->TapOnScreen(&welcomeCloseButtonPoint.coord, 3);
	}

	return 0;

error:
	return -1;
}

/*
 * take_out_all_box
 */
int take_out_all_box(void)
{
	int ret = 0;

	// need to be sure that we can take out things from box

	if (mCaptureService->WaitOnColorKindOf(&giftBoxButtonPoint, 40, 0x0A) < 0) {
		LOGW("gift box button not found.");
		goto error;
	}
	sleep(5);
	mInputService->TapOnScreen(&giftBoxButtonPoint.coord);

	if (mCaptureService->WaitOnColorKindOf(&takeOutAllButtonPoint, 40, 0x0A) < 0) {
		LOGW("takeOutAllButtonPoint not found.");
		goto error;
	}
	sleep(5);
	mInputService->TapOnScreen(&takeOutAllButtonPoint.coord);

	if (mCaptureService->WaitOnColorKindOf(&CloseBoxButtonPoint, 40, 0x0A) < 0) {
		LOGW("CloseBoxButtonPoint not found.");
		goto error;
	}
	sleep(5);
	mInputService->TapOnScreen(&CloseBoxButtonPoint.coord);
	sleep(10);
	return 0;
error:
	return -1;
}

/*
 * do_grand_summon
 */
int do_grand_summon(void)
{
	int ret = 0;
	int draw = 1;
	int checktime = 40; //only tap 40 times to check back button, skip it when the same draw
	char save_path[130];

	if (mCaptureService->WaitOnColorKindOf(&menu2ButtonPoint, 40, 0x09) < 0) {
		LOGW("menu 2 button not found.");
		goto error;
	}
	mInputService->TapOnScreen(&menu2ButtonPoint.coord);
	sleep(1);

	if (mCaptureService->WaitOnColorKindOf(&menuSummonButtonPoint, 10, 0x09) < 0) {
		LOGW("menuSummonButtonPoint not found.");
		goto error;
	}
	mInputService->TapOnScreen(&menuSummonButtonPoint.coord);
	sleep(1);

	if (mCaptureService->WaitOnColorKindOf(&cancelSummonTutorButtonPoint, 10, 0x09) < 0) {
		LOGW("cancelSummonTutorButtonPoint not found.");
		goto error;
	}
	mInputService->TapOnScreen(&cancelSummonTutorButtonPoint.coord);
	sleep(3);

	mInputService->TapOnScreen(&goToStoryButtonPoint.coord);
	sleep(3);

summon:
	checktime = 40;
	LOGD("Draw grand %d", draw);
	if (mCaptureService->WaitOnColorKindOf(&onTimeDrawButtonPoint, 10, 0x1B) < 0) {
		LOGW("onTimeDrawButtonPoint not found.");
		goto error;
	}
	sleep(1);
	mInputService->TapOnScreen(&onTimeDrawButtonPoint.coord);
	sleep(1);

	if (mCaptureService->WaitOnColorKindOf(&confirmGrandSummonButtonPoint, 10, 0x1B) < 0) {
		LOGW("confirmGrandSummonButtonPoint not found.");
		goto error;
	}
	sleep(1);
	mInputService->TapOnScreen(&confirmGrandSummonButtonPoint.coord);
	sleep(5);

	//wait for card show up
	while(checktime > 0) {
		if (mCaptureService->ColorKindOf(&equipShowButtonPoint, 0x0F)) {
			sleep(5);
			mInputService->TapOnScreen(&equipShowButtonPoint.coord);
			sleep(1);
			break;
		} else if (mCaptureService->ColorKindOf(&returnSummonButtonPoint, 0x04)) {
			//we may have a servant summoned
			sleep(5);
			if (mCaptureService->ColorKindOf(&equipShowButtonPoint, 0x0F))
				mInputService->TapOnScreen(&equipShowButtonPoint.coord);
			sleep(1);
			break;
		} else if (mCaptureService->ColorKindOf(&closeSummonButtonPoint, 0x0E)) {
			//this is duplicated card
			checktime = 0;
			break;
		} else {
			mInputService->TapOnScreen(&blindingTouchButtonPoint.coord);
			sleep(1);
			checktime--;
		}
	}

	//screenshot
	snprintf(save_path, 130, "%s/grand%d.png", mCurrentRoot, draw++);
	mCaptureService->SaveScreenshot(save_path);

	if (checktime <= 0) {
		LOGW("We may get a old card, dismiss it.");
	} else {
		mInputService->TapOnScreen(&returnSummonButtonPoint.coord);
	}

	if (draw > 2)
		return 0;

	sleep(5);
	goto summon;

error:
	return -1;
}


/*
 * keep_main
 *
 * The main function of automation, it should be divided by a resonable
 * segments of jobs
 */
void* keep_main(void* data)
{
	int ret;
	char serial[20];
	char cmd[110];

	//clean log
	std::system("rm /data/joshtool/loga");

	while(true) {
		//make game data directory of this game
		getCurrentTime(serial, 20);
		snprintf(mCurrentRoot, 70, "/sdcard/fgo/%s", serial);
		LOGD("Create session folder %s", mCurrentRoot);
		snprintf(cmd, 110, "mkdir -p %s", mCurrentRoot);
		std::system(cmd);

try_create_account_again:
		//start game (you need to confirm game data has cleaned
		LOGD("Start FGO game");
		mSettingService->StartFGO();

		LOGD("Intro >>>>");
		ret = intro_presetup();
		if (ret == -1) goto done;
		if (ret == -9) goto try_create_account_again;

		//wait for all white screen dismissed
		sleep(8);
		LOGD("intro_battle >>>> ");
		ret = intro_battle();
		if (ret < 0) goto done;
		sleep(5);

		LOGD("create character >>>>");
		ret = create_character();
		if (ret < 0) goto done;
		sleep(10);

		LOGD("XA battle >>>> ");
		ret = XA_battle();
		if (ret < 0) goto done;
		sleep(5);

		LOGD("XB battle >>>> ");
		ret = XB_battle();
		if (ret < 0) goto done;
		sleep(5);

		LOGD("1st 10 summon >>>> ");
		ret = first_ten_summon();
		if (ret < 0) goto done;
		sleep(10);

		LOGD("XC-1 >>> ");
		ret = XC1_battle();
		if (ret < 0) goto done;
		sleep(15);

		LOGD("XC-2 >>> ");
		ret = XC2_battle(1);
		if (ret < 0) goto done;
		sleep(15);

		LOGD("XC-3 >>> ");
		ret = XC2_battle(0);
		if (ret < 0) goto done;
		sleep(12);

		LOGD("Take things out of box");
		ret = take_out_all_box();
		if (ret < 0) goto done;
		sleep(5);

		LOGD("Take things out of box");
		ret = do_grand_summon();
		if (ret < 0) goto done;
		sleep(5);

		LOGD("Save game data (deprecated) ");
		mSettingService->SaveGameData(mCurrentRoot);

		LOGD("Kill game, and rest for 10 seconds");
		mSettingService->KillFGO();
	}

done:
	mSettingService->PlaySound(1);
	return NULL;
}

void* keep_xa(void* data)
{
	int ret;
	ret = XA_battle();
	if (ret < 0) goto done;
done:
	mSettingService->PlaySound(1);
	return NULL;
}

void* keep_xb(void* data)
{
	int ret;
	char serial[20];
	char cmd[110];

	//make game data directory of this game
	getCurrentTime(serial, 20);
	snprintf(mCurrentRoot, 70, "/sdcard/fgo/%s", serial);
	LOGD("Create session folder %s", mCurrentRoot);
	snprintf(cmd, 110, "mkdir -p %s", mCurrentRoot);
	std::system(cmd);
	ret = do_grand_summon();
	if (ret < 0) goto done;

done:
	mSettingService->PlaySound(1);
	return NULL;
}



