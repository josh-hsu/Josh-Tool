/*
 * sevenknight.h
 */

#ifndef _SEVEN_KNIGHT_H_
#define _SEVEN_KNIGHT_H_

#include "../libjoshgame/libjoshgame.h"

/* battle functions */
void* keep_arena_battle(void* data);
void* keep_next_region_battle(void* data);
void* keep_golden_house_battle(void* data);
void* keep_change_hero(void* data);

int change_hero(void);
void pre_battle_setup(void);
void post_battle_setup(void);
void corethread_jobdone_callback(void);

extern CoreService* mCoreService;
extern InputService* mInputService;
extern CaptureService* mCaptureService;

#define SEVEN_KNIGHT_JOB	5

struct GameJob {
	CoreThread* jobThread;
	char* jobName;
};

struct GameJobIdentity {
	int index;
	char* jobName;
};

struct GameJobMap {
	GameJobIdentity* jobIdd;
	int jobCount;
};

class SevenKnight {
  public:
	SevenKnight();
	GameJobMap* GetJobMap(void);
	GameJob* GetJobByIndex(int index);
	GameJob* GetJobByName(const char* name);
	void StartJob(GameJob* job);
	void StartJob(int index);
	void StopJob(GameJob* job);
	void StopJob(int index);
	void StopAllJobs(void);
	bool IsIndexLegal(int index);
	CoreThreadStatus GetJobStatus(GameJob* job);
	CoreThreadStatus GetJobStatus(int index);
	
  private:
	CoreThread* mNextRegionBattleThread;
	CoreThread* mArenaBattleThread;
	CoreThread* mGoldenHouseThread;
	CoreThread* mChangeHeroThread;
	struct GameJob mGameJob[SEVEN_KNIGHT_JOB];
	struct GameJobIdentity mJobIdd[SEVEN_KNIGHT_JOB];
	struct GameJobMap mGameJobMap;
};

#endif
