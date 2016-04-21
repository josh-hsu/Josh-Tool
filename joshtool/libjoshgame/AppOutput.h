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

#define LOGI(...) \
	({ \
		char log[256]; \
		char logd[312]; \
		snprintf(log, 256, __VA_ARGS__); \
		snprintf(logd, 256, "echo \"%s\" > /data/joshtool/logd", log); \
		std::system(logd); \
	})

#define LOGE(...) \
	({ \
		char log[256]; \
		char logd[312]; \
		printf("ERROR: " __VA_ARGS__); \
		snprintf(log, 256, __VA_ARGS__); \
		snprintf(logd, 256, "echo \"%s\" > /data/joshtool/loge", log); \
		std::system(logd); \
                snprintf(logd, 256, "echo \"%s\" >> /data/joshtool/loga", log); \
                std::system(logd); \
	})

#define LOGW(...) \
	({ \
		char log[256]; \
		char logd[312]; \
		printf("WARM: " __VA_ARGS__); \
		snprintf(log, 256, __VA_ARGS__); \
		snprintf(logd, 256, "echo \"%s\" > /data/joshtool/logw", log); \
		std::system(logd); \
                snprintf(logd, 256, "echo \"%s\" >> /data/joshtool/loga", log); \
                std::system(logd); \
	})

#define LOGD(...) \
        ({ \
                char log[256]; \
                char logd[312]; \
                printf("INFO: " __VA_ARGS__); \
                snprintf(log, 256, __VA_ARGS__); \
                snprintf(logd, 256, "echo \"%s\" > /data/joshtool/logi", log); \
                std::system(logd); \
                snprintf(logd, 256, "echo \"%s\" >> /data/joshtool/loga", log); \
                std::system(logd); \
        })



#endif
