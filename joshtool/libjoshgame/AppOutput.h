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

#ifndef _APPOUTPUT_H_
#define _APPOUTPUT_H_

#include <cstdlib>
#include <ctime>

static inline void getCurrentTime(char* buf, int len) {
	time_t now = time(0);
	struct tm tstruct;
	tstruct = *localtime(&now);
	strftime(buf, len, "%Y-%m-%d.%X", &tstruct);
}

/*
 *  LOGI: log for display on screen, only one msg at one time.
 */
#define LOGI(...) \
	({ \
		char log[256]; \
		char logd[312]; \
		snprintf(log, 256, __VA_ARGS__); \
		snprintf(logd, 256, "echo \"%s\" > /data/joshtool/logi", log); \
		std::system(logd); \
	})

#define LOGE(...) \
	({ \
		char log[256]; \
		char logd[350]; \
		char timebuf[32]; \
		getCurrentTime(timebuf, 32); \
		snprintf(log, 256, __VA_ARGS__); \
		snprintf(logd, 350, "%s E: %s\n", timebuf, log); \
		fprintf(stderr, "%s", logd); \
		snprintf(logd, 256, "echo \"%s E: %s\" >> /data/joshtool/loga", timebuf, log); \
		std::system(logd); \
	})

#define LOGW(...) \
	({ \
		char log[256]; \
		char logd[350]; \
		char timebuf[32]; \
		getCurrentTime(timebuf, 32); \
		snprintf(log, 256, __VA_ARGS__); \
		snprintf(logd, 350, "%s W: %s\n", timebuf, log); \
		fprintf(stderr, "%s", logd); \
		snprintf(logd, 256, "echo \"%s W: %s\" >> /data/joshtool/loga", timebuf, log); \
		std::system(logd); \
	})

#define LOGD(...) \
	({ \
		char log[256]; \
		char logd[350]; \
		char timebuf[32]; \
		getCurrentTime(timebuf, 32); \
		snprintf(log, 256, __VA_ARGS__); \
		snprintf(logd, 350, "%s D: %s\n", timebuf, log); \
		fprintf(stderr, "%s", logd); \
		snprintf(logd, 256, "echo \"%s D: %s\" >> /data/joshtool/loga", timebuf, log); \
		std::system(logd); \
	})


#endif
