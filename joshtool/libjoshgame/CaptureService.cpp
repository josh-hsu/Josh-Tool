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

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

#include "CaptureService.h"
#include "AppOutput.h"

#define INTERNAL_DUMP_FILE	"/data/joshtool/internal.dump"
#define INTERNAL_DUMP		20
#define SCREEN_WIDTH		720
#define SCREEN_HEIGHT		1280

CaptureService::CaptureService()
{
	LOGD("CaptureService has been created.\n");
}


void CaptureService::DumpScreen(const char* filename)
{
	char cap_command[200];
	snprintf(cap_command, 200, "screencap %s", filename);
	std::system(cap_command);
}

void CaptureService::DumpScreen(void)
{
	std::system("screencap " INTERNAL_DUMP_FILE);
}

void CaptureService::GetColorOnDumpInternal(ScreenColor* sc, const char* filename, ScreenCoord* coord)
{
	FILE* dumpFile = fopen(INTERNAL_DUMP_FILE, "r+");
	int offset = 0;
	int bpp = 4;

	if (dumpFile == NULL) {
		LOGE("CaptureService: cannot open dumpfile %s", filename);
		return;
	}

	if (coord->orien == SO_Portrait)
		offset = (SCREEN_WIDTH * coord->y + coord->x) * bpp;
	else if (coord->orien == SO_Landscape)
		offset = (SCREEN_WIDTH * coord->x + (SCREEN_WIDTH - coord->y)) * bpp;

	fseek(dumpFile, offset, SEEK_SET);
	fgets((char*)sc, 4, dumpFile);
	fclose(dumpFile);
}

/* This should be called after DumpScreen, otherwise you will get old dumps */
void CaptureService::GetColor(ScreenColor* sc, ScreenCoord* coord)
{
	GetColorOnDumpInternal(sc, INTERNAL_DUMP_FILE, coord);
}

void CaptureService::GetColorOnScreen(ScreenColor* sc, ScreenCoord* coord)
{
	DumpScreen();
	GetColor(sc, coord);
}

int CaptureService::WaitOnColor(ScreenColor* sc, ScreenCoord* coord, int thres)
{
	ScreenColor currentColor;
	LOGD("CaptureService: now busy waiting for (%d,%d) turn into %x,%x,%x\n",
		coord->x, coord->y, sc->r, sc->g, sc->b);

	while(thres-- > 0) {
		GetColorOnScreen(&currentColor, coord);
		if (ColorCompare(sc, &currentColor)) {
			LOGD("CaptureService: Matched! \n");
			return 0;
		} else {
			usleep(100*1000);
		}
	}

	return -1;
}

int CaptureService::WaitOnColor(ScreenPoint* sp, int thres)
{
	return WaitOnColor(&sp->color, &sp->coord, thres);
}

bool CaptureService::ColorIs(ScreenPoint* point)
{
	ScreenColor currentColor;
	GetColorOnScreen(&currentColor, &point->coord);
	return ColorCompare(&currentColor, &point->color);
}

int CaptureService::WaitOnColorAsync(ScreenColor* sc, ScreenCoord* coord, int thres)
{
	return 0;
}

int CaptureService::WaitOnColorNotEqual(ScreenColor* sc, ScreenCoord* coord, int thres)
{
	ScreenColor currentColor;
	LOGD("CaptureService: now busy waiting for (%d,%d) is not %x,%x,%x\n",
		coord->x, coord->y, sc->r, sc->g, sc->b);

	while(thres-- > 0) {
		GetColorOnScreen(&currentColor, coord);
		if (!ColorCompare(sc, &currentColor)) {
			LOGD("CaptureService: Matched! \n");
			return 0;
		} else {
			usleep(100);
		}
	}

	return -1;
}

bool CaptureService::ColorCompare(ScreenColor *src, ScreenColor *dest)
{
	if((src->r == dest->r) &&
	   (src->g == dest->g) &&
	   (src->b == dest->b)) {
		return true;
	}

	return false;
}
