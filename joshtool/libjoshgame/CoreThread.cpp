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

#include "CoreThread.h"
#include "AppOutput.h"

void thread_exit_handler(int sig)
{ 
    LOGD("Exiting..., this signal is %d ", sig);
    pthread_exit(0);
}

CoreThread::CoreThread(void *(*func)(void* data), int hd)
{
	mFunction = func;
	mHandle = hd;
	mCurrentStatus = CTS_PREPARED;
}

CoreThread::CoreThread(void *(*func)(void* data), int hd, void (*cb_func)(void))
{
	mFunction = func;
	mHandle = hd;
	mJobDoneCallback = cb_func;
	mCurrentStatus = CTS_PREPARED;
}

void* CoreThread::MainThreadRoutine(void* data)
{
	struct sigaction actions;
	CoreThread* tdata;

	tdata = reinterpret_cast<CoreThread*>(data);
	if (!tdata) {
		LOGE("Thread start failed, tdata is null.");
		goto out;
	}

	/* Register SIGUSR1 handler to exit thread */
	memset(&actions, 0, sizeof(actions)); 
	sigemptyset(&actions.sa_mask);
	actions.sa_flags = 0; 
	actions.sa_handler = thread_exit_handler;
	sigaction(SIGUSR1, &actions, NULL);

	/* starting mFunction */
	tdata->mFunction(NULL);

jobdone:
	/* job finished */
	tdata->mCurrentStatus = CTS_TERMINATED;
	if (tdata->mJobDoneCallback) {
		tdata->mJobDoneCallback();
	} else {
		LOGW("Job is done but callback is null.");
	}

out:
	return NULL;
}

void CoreThread::Start(CoreThreadType type)
{
	pthread_create(&mThreadId, NULL, MainThreadRoutine, this);
	mCurrentStatus = CTS_RUNNING;
	mThreadType = type;
}

int CoreThread::Pause()
{
	if (mThreadType == CT_ONE_TIME)
		return -1;

	return 0;
}

int CoreThread::Resume()
{
	if (mThreadType == CT_ONE_TIME)
		return -1;

	return 0;
}

int CoreThread::Stop()
{
	int ret = 0;
	if (pthread_kill(mThreadId, 0) != ESRCH) {
		ret = pthread_kill(mThreadId, SIGUSR1);
	} else {
		LOGW("Kill thread is not necessary => NOT RUNNING");
	}

	mCurrentStatus = CTS_TERMINATED;

	return ret;
}

CoreThreadStatus CoreThread::GetStatus(void)
{
	return mCurrentStatus;
}
