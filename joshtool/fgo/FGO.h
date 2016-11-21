/*
 * FGO.h
 */

#ifndef _FGO_H_
#define _FGO_H_

#include "../libjoshgame/libjoshgame.h"

/* battle functions */
void* keep_main(void* data);
void* keep_xa(void* data);
void* keep_xb(void* data);

void corethread_jobdone_callback(void);

extern SettingService* mSettingService;
extern InputService* mInputService;
extern CaptureService* mCaptureService;

#define FGO_JOB		2

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

class FGO {
  public:
	FGO();
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
	CoreThread* mMainThread;
	CoreThread* mXAThread;
	struct GameJob mGameJob[FGO_JOB];
	struct GameJobIdentity mJobIdd[FGO_JOB];
	struct GameJobMap mGameJobMap;
};

#endif
