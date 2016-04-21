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

#include "SevenKnight.h"
#include "sevenknight_720p_script.h"

SevenKnight::SevenKnight() {

	/* initialize of CoreThread */
	mNextRegionBattleThread = new CoreThread(keep_next_region_battle, 0, corethread_jobdone_callback);
	mGoldenHouseThread = new CoreThread(keep_golden_house_battle, 1, corethread_jobdone_callback);
	mArenaBattleThread = new CoreThread(keep_arena_battle, 2, corethread_jobdone_callback);
	mChangeHeroThread = new CoreThread(keep_change_hero, 4, corethread_jobdone_callback);

	/* initialize of game jobs */
	mGameJob[0].jobThread = mNextRegionBattleThread;
	mGameJob[0].jobName = "normal_game";
	mGameJob[1].jobThread = mGoldenHouseThread;
	mGameJob[1].jobName = "golden_house_game";
	mGameJob[2].jobThread = mArenaBattleThread;
	mGameJob[2].jobName = "arena_game";
	mGameJob[3].jobThread = NULL;
	mGameJob[3].jobName = "dragon_battle";
	mGameJob[4].jobThread = mChangeHeroThread;
	mGameJob[4].jobName = "change_hero";
	
	/* initial GameJobMap */
	mJobIdd[0].index = 0;
	mJobIdd[0].jobName = "normal_game";
	mJobIdd[1].index = 1;
	mJobIdd[1].jobName = "golden_house_game";
	mJobIdd[2].index = 2;
	mJobIdd[2].jobName = "arena_game";
	mJobIdd[3].index = 3;
	mJobIdd[3].jobName = "dragon_battle";
	mJobIdd[4].index = 4;
	mJobIdd[4].jobName = "change_hero";
	mGameJobMap.jobCount = SEVEN_KNIGHT_JOB;
	mGameJobMap.jobIdd = mJobIdd;
}

bool SevenKnight::IsIndexLegal(int index)
{
	if (index >= SEVEN_KNIGHT_JOB || index < 0)
		return false;

	return true;
}

GameJobMap* SevenKnight::GetJobMap(void)
{
	return &mGameJobMap;
}

GameJob* SevenKnight::GetJobByIndex(int index)
{
	if (IsIndexLegal(index))
		return &mGameJob[index];
	else
		return NULL;
}

void SevenKnight::StartJob(GameJob* job)
{
	pre_battle_setup();
	job->jobThread->Start(CT_FOREVER);
}

void SevenKnight::StartJob(int index)
{
	if (IsIndexLegal(index)) {
		pre_battle_setup();
		GetJobByIndex(index)->jobThread->Start(CT_FOREVER);
	} else
		LOGE("Index %d of game is not legal\n", index);
}

void SevenKnight::StopJob(GameJob* job)
{
	job->jobThread->Stop();
	post_battle_setup();
}

void SevenKnight::StopJob(int index)
{
	if (IsIndexLegal(index)) {
		GetJobByIndex(index)->jobThread->Stop();
		post_battle_setup();
	} else
		LOGE("Index %d of game is not legal\n", index);
}

void SevenKnight::StopAllJobs(void)
{
	for(int i = 0; i < SEVEN_KNIGHT_JOB; i++) {
		StopJob(i);
	}
	post_battle_setup();
}

CoreThreadStatus SevenKnight::GetJobStatus(GameJob* job)
{
	return job->jobThread->GetStatus();
}

CoreThreadStatus SevenKnight::GetJobStatus(int index)
{
	if (IsIndexLegal(index))
		return GetJobByIndex(index)->jobThread->GetStatus();

	return CTS_MAX;
}

void corethread_jobdone_callback(void)
{
	LOGD("CoreThread job is done.\n");
	post_battle_setup();
}

void print_battle_state(void)
{
	if(mCaptureService->ColorIs(&battleAutoOnPoint)) {
		LOGD("Auto skill is ON.\n");
		LOGI("自動技能打開中");
		if (!mCoreService->GetBool(BATTLE_AUTO_ENABLE, false))
			mInputService->TapOnScreen(&battleAutoOnPoint.coord);
	} else if (mCaptureService->ColorIs(&battleAutoOffPoint)) {
		LOGD("Auto skill is OFF.\n");
		LOGI("自動技能關閉中");
		if (mCoreService->GetBool(BATTLE_AUTO_ENABLE, false))
			mInputService->TapOnScreen(&battleAutoOnPoint.coord);
	}

	if(mCaptureService->ColorIs(&battleX2OffPoint)) {
		LOGD("X2 has been disabled. Enable it now.\n");
		LOGI("二倍速關閉中");
		if (mCoreService->GetBool(BATTLE_2X_ENABLE, true))
			mInputService->TapOnScreen(&battleX2OffPoint.coord);
	} else {
		LOGD("X2 has been enabled.\n");
		LOGI("二倍速打開中");
	}
}

void pre_battle_setup(void)
{
	if (mCoreService->GetBool(BATTLE_SCREEN_DISABLE, false))
		mInputService->SetBacklight(0);

	if (mCoreService->GetBool(BATTLE_TOUCH_DISABLE, false))
		mInputService->ConfigTouchScreen(false);

	if (mCoreService->GetBool(BATTLE_SENSOR_DISABLE, true))
		mInputService->ConfigGyroSensor(false);
}

void post_battle_setup(void)
{
	mInputService->ConfigTouchScreen(true);
	mInputService->ConfigGyroSensor(true);
	mInputService->SetBacklight(12);
}

/*
 * If we are not in prepare screen, the following things could happen
 * 1. dragon shown up
 * 2. someone is level up
 */
bool checkForPrepareExceptions(ScreenPoint* buttonPoint)
{
	int max_error_tries = 6;

	while (!mCaptureService->ColorIs(buttonPoint) && (max_error_tries > 0)) {
		sleep(1); //delay for heated device
		// Dragon shows up
		if (mCaptureService->ColorIs(&infoDragonHasSpownedPoint)) {
			LOGD("dragon has spowned.\n");
			LOGI("出龍了");
			sleep(1);
			mInputService->TapOnScreen(&infoDragonHasSpownedPoint.coord);
			sleep(2);
			mInputService->TapOnScreen(&backButtonTopLeftPoint.coord);
			sleep(2);
			// If we level up
			if (mCaptureService->ColorIs(&infoNameFieldColorPoint))
				mInputService->TapOnScreen(&infoOptionBButtonPoint.coord);
			sleep(1);
			mInputService->TapOnScreen(&fieldNewAdvantureButtonPoint.coord);
		}

		// Someone level up
		if (mCaptureService->ColorIs(&infoNameFieldColorPoint)) {
			// press the continue button
			// TODO: support button A option
			LOGD("INFO has shown.\n");
			LOGI("訊息出現");
			sleep(2);
			mInputService->TapOnScreen(&infoOptionBButtonPoint.coord);
		}

		// Check if it is resolved
		if (mCaptureService->ColorIs(buttonPoint))
			return true;

		max_error_tries--;
	}

	// Final check
	if (mCaptureService->ColorIs(buttonPoint))
		return true;

	return false;
}

/*
 * This function changes hero from level 30 to level 1
 * and return 0 if everything done sucessfully.
 * It should be started at prepare page, otherwise return -1
 */
int change_hero(void)
{
	int place_finish = 0; // now doing which hero changing
	int i = 0; // which hero should try
	int swipe_count = 3; // how many swipes should we try
	const int heros_in_page = 8;

	if (!mCaptureService->ColorIs(&prepareNormalButtonPoint)) {
		LOGW("not in the prepare view, changing hero failed.\n");
		LOGI("現在不是在準備畫面");
		return -1;
	}

	/* Entering changing hero screen */
	if (!mCaptureService->ColorIs(&prepareChangingHeroButtonPoint)) {
		LOGW("no changing hero button, changing hero failed.\n");
		LOGI("找不到英雄管理按鈕");
		return -1;
	}

	//mInputService->TapOnScreenUntilColorChanged(&prepareChangingHeroButtonPoint, 30, 5);
	mInputService->TapOnScreen(&prepareChangingHeroButtonPoint.coord);
	sleep(4);
	/* check if is in changing screen */
	if (mCaptureService->WaitOnColor(&heroInChangingScreenPoint, 10) < 0) {
		LOGW("cannot enter changing screen, changing hero failed.\n");
		LOGI("無法進入英雄管理畫面");
		//return -1;
	}

try_changing:
	/* try to refresh */
	sleep(2);
	mInputService->TapOnScreen(&heroLowToHighPoint.coord);
	sleep(1);
	mInputService->TapOnScreen(&heroLowToHighPoint.coord);
	sleep(1);
	if (!mCaptureService->ColorIs(&heroLowToHighPoint)) {
		mInputService->TapOnScreen(&heroLowToHighPoint.coord);
	}

	/* make all hero 1 ~ 3 off shelf */
	LOGD("now doing off shelf for those 3 heroes.\n");
	LOGI("下架中");
	sleep(2);
	mInputService->TapOnScreen(&hero1L1CheckPoint.coord);
	sleep(1);
	mInputService->TapOnScreen(&heroPagechangePoint.coord);
	sleep(2);
	mInputService->TapOnScreen(&hero2L1CheckPoint.coord);
	sleep(1);
	mInputService->TapOnScreen(&heroPagechangePoint.coord);
	sleep(2);
	mInputService->TapOnScreen(&hero3L1CheckPoint.coord);
	sleep(1);
	mInputService->TapOnScreen(&heroPagechangePoint.coord);
	sleep(2);

	/* now doing brute forcely place and try */
	LOGD("now doing brute forcely place and try.\n");
	LOGI("暴力法更換英雄開始 最多嘗試 %d 個英雄交換", swipe_count*heros_in_page);
	while(place_finish < 3 && swipe_count > 0) {
		while(!mCaptureService->ColorIs(&heroL1CheckPoints[place_finish])) {
			/* try 8 heroes */
			while(i < heros_in_page) {
				LOGD("Now trying i = %d\n", i);
				mInputService->TapOnScreen(&heroSelectPoints[i].coord);
				sleep(1);
				LOGD("Tap change %d\n", i);
				mInputService->TapOnScreen(&heroPagechangePoint.coord);
				sleep(1);
				LOGD("Tap hero %d\n", place_finish);
				mInputService->TapOnScreen(&heroL1CheckPoints[place_finish].coord);
				sleep(4);

				/* see if error happened */
				LOGD("Verifying i = %d\n", i);
				if (mCaptureService->ColorIs(&heroSelectFailPoint)) {
					LOGD("change hero %d failed, now try another %d.\n", place_finish, i);
					LOGI("第 %d 個英雄更換失敗，嘗試下個 %d", place_finish+1, i);
					// Tap anywhere to dismiss window
					mInputService->TapOnScreen(&heroL1CheckPoints[0].coord);
					sleep(1);
					i++;
  				} else if (mCaptureService->ColorIs(&heroL1CheckPoints[place_finish])) {
					LOGD("%i is okay, now i++\n", i);
					LOGI("已換上，且為LV1");
  					i++;
					goto hero_lv_check;
				} else {
					LOGD("%d is not LV1, next...\n", i);
					i++;
				}
			}

hero_lv_check:
			if (mCaptureService->ColorIs(&heroL1CheckPoints[place_finish])) {
				LOGD("change hero %d finished.\n", place_finish);
				LOGI("第 %d 個英雄更換完成", place_finish+1);
				place_finish++;
			} else if (i >= heros_in_page) { /* those 8 heroes are useless */
				LOGI("畫面上的八個英雄都不能用");
				mInputService->TouchOnScreen(1000, 540, 1000, 265, INPUT_TYPE_SWIPE);
				swipe_count--;
				i = 0;
			}

			if (place_finish >= 3)
				break;
		}
	}

	if (swipe_count > 0)
		return 0;

	return -1;
}

void* keep_change_hero(void* data)
{
	int ret = change_hero();
	LOGI("change hero return %d", ret);
	return NULL;
}

void* keep_arena_battle(void* data)
{
	int ret = change_hero();
	LOGI("change hero return %d", ret);
	return NULL;
}

void* keep_next_region_battle(void* data)
{
	int ret = 0;
	int max_battle_round = 8;
	int battle_team = -1;
	int skill_1, skill_2, skill_3;

	/* Get default settings */
	battle_team = mCoreService->GetInt(BATTLE_NORMAL_TEAM, -1);
	skill_1 = mCoreService->GetInt(BATTLE_NORMAL_SKILL_1, 4);
	skill_2 = mCoreService->GetInt(BATTLE_NORMAL_SKILL_2, 2);
	skill_3 = mCoreService->GetInt(BATTLE_NORMAL_SKILL_3, 1);

	while(true) {
start_noraml:
		/* make it in prepare screen */
		if (!checkForPrepareExceptions(&prepareNormalButtonPoint)) {
			LOGW("Cannot enter back prepare screen.\n");
			LOGI("無法進入戰鬥準備畫面");
			goto fail_end_0;
		}

		LOGD("Now in prepare screen.\n");

		/* check if max battle is achieved then switch hero if enabled */
		if (max_battle_round <= 0) {
			/* changing hero if enabled */
			if (mCoreService->GetBool(BATTLE_CHANGE_HERO, false)) {
				LOGD("change hero enabled, checking ... \n");
				sleep(2); //Prevent laggy device crashed
				ret = change_hero();
				if (ret < 0) {
					LOGE("change hero failed. exiting...\n");
					goto fail_end_0;
				} else {
					max_battle_round = 8; // if no error, restore battle count
					LOGI("更換完成。回到前頁");
					mInputService->TapOnScreen(&backButtonTopLeftPoint.coord);
					goto start_noraml;
				}
			} else {
				LOGD("we have done. exiting...\n");
				goto fail_end_0;
			}
		}
		
		LOGD("   == %d / %d round start! ==   \n", 9 - max_battle_round, 8);
		LOGI("戰鬥 %d / %d 開始", 9 - max_battle_round, 8);

		/* check for battle team before started */
		if (battle_team > 0 && battle_team < 4) {
			LOGD("Select team %d as primary team.\n", battle_team);
			LOGI("選擇 %d 隊伍", battle_team);
			mInputService->TapOnScreen(&teamChooseButtonPoint[battle_team-1].coord);
		}

		/* Tap enter battle */
		mInputService->TapOnScreen(&prepareNormalButtonPoint.coord);
		ret = mInputService->TapOnScreenUntilColorChanged(&prepareNormalButtonPoint, 2, 5);
		if (ret < 0) {
			LOGW("Start battle failed, connection issue?\n");
			LOGI("開始戰鬥失敗-連線問題?");
			goto fail_end_0;
		}

		/* Check if we don't have key */
		if (mCaptureService->ColorIs(&prepareFailNoBlueKeyPoint)) {
			LOGW("No blue key to continue, exit looper.\n");
			goto fail_end_0;
		}

		ret = mCaptureService->WaitOnColor(&battlePauseButtonPoint.color, &battlePauseButtonPoint.coord, 20);
		if (ret < 0) {
			LOGW("Cannot enter battle screen. \n");
			LOGI("無法進入戰鬥畫面");
			goto fail_end_0;
		}

		LOGD("Start pressing skills.\n");
		LOGI("開始點技能");
		usleep(30);
		if (0 < skill_1 && skill_1 < 11)
			mInputService->TapOnScreen(&battleSkill[skill_1 - 1].coord);
		usleep(30);
		if (0 < skill_2 && skill_2 < 11)
			mInputService->TapOnScreen(&battleSkill[skill_2 - 1].coord);
		usleep(30);
		if (0 < skill_3 && skill_3 < 11)
			mInputService->TapOnScreen(&battleSkill[skill_3 - 1].coord);

		print_battle_state();
		
		/* wait until battle finished */
		LOGI("等待戰鬥結束");
wait_battle:
		ret = mCaptureService->WaitOnColorNotEqual(&battlePauseButtonPoint.color, &battlePauseButtonPoint.coord, 1000);

		if (ret < 0) {
			LOGW("Battle timeout. \n");
			LOGI("戰鬥逾時");
			goto fail_end_1;
		}

		/* This could be temprarely missing */
		sleep(3);
		if (mCaptureService->ColorIs(&battlePauseButtonPoint)) {
			goto wait_battle;
		}

		LOGD("Battle finished, waiting for result.\n");
		LOGI("戰鬥完成等待結果");

		ret = mCaptureService->WaitOnColor(&resultHeroButtonPoint.color, &resultHeroButtonPoint.coord, 500);
		if (ret < 0) {
			LOGW("Result doesn't show up... \n");
			LOGI("戰鬥結果未出現");
			goto fail_end_1;
		}

		LOGD("Result showed\n");
		LOGI("戰鬥結果出現");
		sleep(2); //for ZE500KL
		mInputService->TapOnScreen(&resultNormalAgainButtonPoint.coord);

		max_battle_round--;

		sleep(5);
	}

normal_end:
	return NULL;

fail_end_0:
	mCoreService->PlaySound(0);
	return NULL;

fail_end_1:
	mCoreService->PlaySound(1);
	return NULL;
}

void* keep_golden_house_battle(void* data)
{
	while(true) {
		/* make it in prepare screen */
		if (!checkForPrepareExceptions(&prepareGoldenHouseButtonPoint)) {
			LOGW("Cannot enter back prepare screen.\n");
			LOGI("無法進入戰鬥準備畫面");
			goto fail_end_0;
		}

		LOGD("Now in prepare screen.\n");
		mInputService->TapOnScreen(&prepareGoldenHouseButtonPoint.coord);
		usleep(100);

		/* Check if we don't have key */
		if (mCaptureService->ColorIs(&prepareFailNoBlueKeyPoint)) {
			LOGW("No blue key to continue, exit looper.\n");
			goto fail_end_0;
		}

		LOGD("Start battling...\n");
		LOGI("戰鬥開始");
		mCaptureService->WaitOnColor(&battlePauseButtonPoint.color, &battlePauseButtonPoint.coord, 1000);
		LOGD("Start pressing skills.\n");
		LOGI("開始點技能");
		usleep(10);
		mInputService->TapOnScreen(&battleSkill[0].coord);
		usleep(10);
		mInputService->TapOnScreen(&battleSkill[1].coord);
		mCaptureService->WaitOnColorNotEqual(&battlePauseButtonPoint.color, &battlePauseButtonPoint.coord, 1000);
		LOGD("Battle finished, waiting for result.\n");
		LOGI("戰鬥完成等待結果");

		mCaptureService->WaitOnColor(&resultHeroButtonPoint.color, &resultHeroButtonPoint.coord, 500);
		LOGD("Result showed\n");
		LOGI("戰鬥結果出現");
		mInputService->TapOnScreen(&resultInfiniteAgainButtonPoint.coord);
		sleep(1);
	}

normal_end:
	return NULL;

fail_end_0:
	mCoreService->PlaySound(0);
	return NULL;

fail_end_1:
	mCoreService->PlaySound(1);
	return NULL;
}


