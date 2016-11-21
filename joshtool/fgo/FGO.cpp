/*
 * Copyright (C) 2016 ASUSTek Computer Inc.
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
		LOGE("Index %d of game is not legal\n", index);
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
		LOGE("Index %d of game is not legal\n", index);
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
	LOGD("CoreThread job is done.\n");
}

/*=====================
 * Common function
 *===================== */
/*
 * check for skip
 */
int check_for_skip(void)
{
	if (mCaptureService->WaitOnColor(&skipButtonPoint, 200) < 0) {
		LOGW("skip not found.\n");
		goto error;
	}

	sleep(7);
	if (mCaptureService->WaitOnColor(&skipButtonPoint, 200) < 0) {
		LOGW("skip not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&skipButtonPoint.coord);
	if (mCaptureService->WaitOnColor(&confirmSkipButtonPoint, 50) < 0) {
		LOGW("skip confirm not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&confirmSkipButtonPoint.coord);
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
		sleep(1);
		mInputService->TapOnScreen(&tapMosterTextPoint.coord);
		mInputService->TapOnScreen(&tapMosterButtonPoint.coord);
		sleep(2);
	}

	if (mCaptureService->WaitOnColor(&drawCardButtonPoint, 200) < 0) {
		LOGW("draw card battle button not found.\n");
		goto error;
	}

	mInputService->TapOnScreen(&drawCardButtonPoint.coord);
	sleep(2);
	if (session == 1) {
		if (mCaptureService->ColorIs(&touchDismissButtonPoint))
			mInputService->TapOnScreen(&touchDismissButtonPoint.coord);
		sleep(1);
	}

	if (session == 3) {
		sleep(1);
		mInputService->TapOnScreen(&speedUpButtonPoint.coord);
		sleep(2);
	}

	if (session == 4) {
		sleep(1);
		mInputService->TapOnScreen(&tapMosterTextPoint.coord);
		mInputService->TapOnScreen(&tapMosterButtonPoint.coord);
		sleep(2);
	}

	if (session != 2) {
		mInputService->TapOnScreen(&card1ButtonPoint.coord);
		mInputService->TapOnScreen(&card2ButtonPoint.coord);
		mInputService->TapOnScreen(&card3ButtonPoint.coord);
	} else {
		mInputService->TapOnScreen(&royalCardButtonPoint.coord);
		mInputService->TapOnScreen(&card1ButtonPoint.coord);
		mInputService->TapOnScreen(&card2ButtonPoint.coord);
	}
	return 0;
	
error:
	return -1;
}

/*
 * standard battle and wait for match point
 */
int battle_till_finish(ScreenPoint* match)
{
	int max_battle_round = 10;
	int ret = 0;

retry:
	while(!mCaptureService->ColorIs(match) && max_battle_round > 0) {
		if (mCaptureService->WaitOnColor(&drawCardButtonPoint, 100) < 0) {
			LOGW("draw card battle button not found.\n");
			sleep(5);
			max_battle_round--;
			goto retry;
		}
		mInputService->TapOnScreen(&drawCardButtonPoint.coord);
		sleep(2);

		mInputService->TapOnScreen(&card1ButtonPoint.coord);
		mInputService->TapOnScreen(&card2ButtonPoint.coord);
		mInputService->TapOnScreen(&card3ButtonPoint.coord);

		sleep(10);
		max_battle_round--;
	}

	if (max_battle_round <= 0)
		return -1;

	//matched!
	LOGD("dismiss battle result\n");
	max_battle_round = 8;
	while(!mCaptureService->ColorIs(&nextButtonPoint) && max_battle_round >0) {
		mInputService->TapOnScreen(&bondButtonPoint.coord);
		sleep(1);
		max_battle_round--;
	}

	mInputService->TapOnScreen(&nextButtonPoint.coord);

	LOGD("battle till finished\n");
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
	if (mCaptureService->WaitOnColor(&homePageButtonPoint, 50) < 0) {
		LOGW("not in home page.\n");
		goto error;
	}

	// tap on screen to create an id
	mInputService->TapOnScreen(&homePageButtonPoint.coord);

	if (mCaptureService->WaitOnColor(&agreementButtonPoint, 50) < 0) {
		LOGW("agreement not shown.\n");
	} else {
		mInputService->TapOnScreen(&agreementButtonPoint.coord);
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
	sleep(20);

	ret = battle_once(0);
	if (ret < 0) goto error;
	sleep(12);
	ret = battle_once(0);
	if (ret < 0) goto error;
	sleep(12);
	ret = battle_once(0);
	if (ret < 0) goto error;
	sleep(12);
	ret = battle_once(1);
	if (ret < 0) goto error;
	sleep(12);
	ret = battle_once(2);
	if (ret < 0) goto error;
	sleep(20);

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(13);
	
	return 0;
error:
	return -1;
}

int create_character(void)
{
	int ret;
	if (mCaptureService->WaitOnColor(&touchNameButtonPoint, 50) < 0) {
		LOGW("name field button not found.\n");
		goto error;
	}

	mInputService->TapOnScreen(&touchNameButtonPoint.coord);
	sleep(5);
	mInputService->InputText("MUMU");
	sleep(5);
	mInputService->TapOnScreen(&returnButtonPoint.coord);
	sleep(4);
	// confrim
	mInputService->TapOnScreen(&enterNameButtonPoint.coord);
	sleep(4);
	mInputService->TapOnScreen(&confirmNameButtonPoint.coord);

	sleep(10);
	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(20);

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

	if (mCaptureService->WaitOnColor(&enterZone1ButtonPoint, 10) < 0) {
		LOGW("zone 1 button not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&enterZone1ButtonPoint.coord);
	sleep(2);

	if (mCaptureService->WaitOnColor(&enterZone1SubButtonPoint, 10) < 0) {
		LOGW("zone 1 sub button not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&enterZone1SubButtonPoint.coord);
	sleep(5);

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(30);

	ret = battle_once(0);
	if (ret < 0) goto error;
	sleep(18);

	// wait for skill tutor
	if (mCaptureService->WaitOnColor(&skillTutorTextPoint, 10) < 0) {
		LOGW("zone 1 sub button not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&skill1ButtonPoint.coord);
	sleep(2);
	mInputService->TapOnScreen(&useSkillButtonPoint.coord);
	sleep(2);
	mInputService->TapOnScreen(&skillTargetButtonPoint.coord);
	sleep(2);

	ret = battle_once(3);
	if (ret < 0) goto error;
	sleep(18);

	ret = battle_till_finish(&bondButtonPoint);
	if (ret < 0) goto error;

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(24);

	// wait for magic stone
	if (mCaptureService->WaitOnColor(&magicStoneButtonPoint, 10) < 0) {
		LOGW("magic stone button not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&magicStoneButtonPoint.coord);
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

	if (mCaptureService->WaitOnColor(&enterZone2ButtonPoint, 40) < 0) {
		LOGW("zone 2 button not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&enterZone2ButtonPoint.coord);
	sleep(2);

	mInputService->TapOnScreen(&enterZone2SubButtonPoint.coord);
	sleep(9);

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(30);

	ret = battle_once(0);
	if (ret < 0) goto error;
	sleep(18);

	ret = battle_once(4);
	if (ret < 0) goto error;
	sleep(18);
	
	ret = battle_till_finish(&bondButtonPoint);
	if (ret < 0) goto error;

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(25);

	// wait for magic stone
	if (mCaptureService->WaitOnColor(&magicStoneButtonPoint, 10) < 0) {
		LOGW("magic stone button not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&magicStoneButtonPoint.coord);
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

	if (mCaptureService->WaitOnColor(&menuButtonPoint, 10) < 0) {
		LOGW("menuButtonPoint not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&menuButtonPoint.coord);
	sleep(3);
	mInputService->TapOnScreen(&summonButtonPoint.coord);
	sleep(5);
	mInputService->TapOnScreen(&tenDrawButtonPoint.coord);
	sleep(3);
	mInputService->TapOnScreen(&confirmDrawButtonPoint.coord);
	sleep(3);

	//wait for servant introduce themselve
	while(!mCaptureService->ColorIs(&summonDoneSummonButtonPoint) && max_round > 0) {
		mInputService->TapOnScreen(&summonDoneButtonPoint.coord);
		usleep(100 * 1000);
		max_round--;
	}
	mInputService->TapOnScreen(&summonDoneSummonButtonPoint.coord);
	sleep(3);

	mInputService->TapOnScreen(&menuButtonPoint.coord);
	sleep(3);
	mInputService->TapOnScreen(&teamButtonPoint.coord);
	sleep(3);
	mInputService->TapOnScreen(&partyButtonPoint.coord);
	sleep(3);
	mInputService->TapOnScreen(&member2ButtonPoint.coord);
	sleep(3);
	if (mCaptureService->WaitOnColor(&memberTutorTextPoint, 10) < 0) {
		LOGW("memberTutorTextPoint not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&selectServantButtonPoint.coord);
	sleep(2);
	mInputService->TapOnScreen(&selectServantButtonPoint.coord);
	sleep(5);
	mInputService->TapOnScreen(&partyConfigDoneButtonPoint.coord);
	sleep(10);
	mInputService->TapOnScreen(&partyCloseButtonPoint.coord);
	sleep(3);
	mInputService->TapOnScreen(&teamCloseButtonPoint.coord);
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

	if (mCaptureService->WaitOnColor(&enterZone3ButtonPoint, 40) < 0) {
		LOGW("zone 3 button not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&enterZone3ButtonPoint.coord);
	sleep(2);

	mInputService->TapOnScreen(&enterZone3SubButtonPoint.coord);
	sleep(3);
	
	if (mCaptureService->WaitOnColor(&selectSupportTextPoint, 40) < 0) {
		LOGW("selectSupportTextPoint not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&selectSupportTextPoint.coord);
	sleep(2);
	
	mInputService->TapOnScreen(&selectSupportButtonPoint.coord);
	sleep(3);
	
	mInputService->TapOnScreen(&questStartButtonPoint.coord);
	sleep(3);

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(35);

	ret = battle_till_finish(&bondButtonPoint);
	if (ret < 0) goto error;
	sleep(3);

	mInputService->TapOnScreen(&applyFriendButtonPoint.coord);
	sleep(5);

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(13);
	
	mInputService->TapOnScreen(&welcomeCloseButtonPoint.coord);
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

	mInputService->TapOnScreen(&enterZone3SubButtonPoint.coord);
	sleep(4);
	
	mInputService->TapOnScreen(&selectSupportButtonPoint.coord);
	sleep(4);
	
	mInputService->TapOnScreen(&questStartButtonPoint.coord);
	sleep(4);

	ret = check_for_skip();
	if (ret < 0) goto error;
	sleep(35);

	// need to speed up again
	if (speed_up == 1) {
		ret = battle_once(3);
		if (ret < 0) goto error;
		sleep(18);
	}

	ret = battle_till_finish(&bondButtonPoint);
	if (ret < 0) goto error;
	sleep(3);

	mInputService->TapOnScreen(&applyFriendButtonPoint.coord);
	sleep(5);

	// second round has magic stone and info
	if (speed_up == 0) {
		ret = check_for_skip();
		sleep(13);

		// wait for magic stone
		if (mCaptureService->WaitOnColor(&magicStoneButtonPoint, 10) < 0) {
			LOGW("magic stone button not found.\n");
			goto error;
		}
		mInputService->TapOnScreen(&magicStoneButtonPoint.coord);
		sleep(8);

		// tap info
		mInputService->TapOnScreen(&welcomeCloseButtonPoint.coord);
		sleep(3);
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

	if (mCaptureService->WaitOnColor(&giftBoxButtonPoint, 40) < 0) {
		LOGW("gift box button not found.\n");
		goto error;
	}
	mInputService->TapOnScreen(&takeOutAllButtonPoint.coord);
	sleep(2);
	
	mInputService->TapOnScreen(&CloseBoxButtonPoint.coord);
	sleep(2);
	return 0;
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

	ret = intro_presetup();
	if (ret < 0) goto done;

	//wait for all white screen dismissed
	sleep(15);

	ret = intro_battle();
	if (ret < 0) goto done;

	ret = create_character();
	if (ret < 0) goto done;
	sleep(12);
	
	ret = XA_battle();
	if (ret < 0) goto done;
	sleep(10);
	
	ret = XB_battle();
	if (ret < 0) goto done;
	sleep(10);
	
	ret = first_ten_summon();
	if (ret < 0) goto done;
	sleep(10);
	
	ret = XC1_battle();
	if (ret < 0) goto done;
	sleep(5);
	
	ret = XC2_battle(1);
	if (ret < 0) goto done;
	sleep(15);
	
	ret = XC2_battle(0);
	if (ret < 0) goto done;
	sleep(10);

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
	ret = XC1_battle();
	if (ret < 0) goto done;
	

done:
	mSettingService->PlaySound(1);
	return NULL;
}



