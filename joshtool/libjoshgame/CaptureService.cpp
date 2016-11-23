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
	LOGD("CaptureService has been created.");
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
	LOGD("CaptureService: now busy waiting for (%d,%d) turn into %x,%x,%x",
		coord->x, coord->y, sc->r, sc->g, sc->b);

	while(thres-- > 0) {
		GetColorOnScreen(&currentColor, coord);
		if (ColorCompare(sc, &currentColor)) {
			LOGD("CaptureService: Matched! ");
			return 0;
		} else {
			sleep(1);
		}
	}

	return -1;
}

int CaptureService::WaitOnColorKindOf(ScreenColor* sc,
				ScreenCoord* coord, int check_time, int thres)
{
	ScreenColor currentColor;
	LOGD("CaptureService: now busy waiting for (%d,%d) turn into %x,%x,%x",
		coord->x, coord->y, sc->r, sc->g, sc->b);

	while(check_time-- > 0) {
		GetColorOnScreen(&currentColor, coord);
		if (ColorCompare(sc, &currentColor, thres)) {
			LOGD("CaptureService: Matched! ");
			return 0;
		} else {
			sleep(1);
		}
	}

	return -1;
}

int CaptureService::WaitOnColor(ScreenPoint* sp, int thres)
{
	return WaitOnColor(&sp->color, &sp->coord, thres);
}

int CaptureService::WaitOnColorKindOf(ScreenPoint* sp, int check_time, int thres)
{
	return WaitOnColorKindOf(&sp->color, &sp->coord, check_time, thres);
}

bool CaptureService::ColorIs(ScreenPoint* point)
{
	ScreenColor currentColor;
	GetColorOnScreen(&currentColor, &point->coord);
	return ColorCompare(&currentColor, &point->color);
}

bool CaptureService::ColorKindOf(ScreenPoint* point, int thres)
{
	ScreenColor currentColor;
	GetColorOnScreen(&currentColor, &point->coord);
	return ColorCompare(&currentColor, &point->color, thres);
}

int CaptureService::WaitOnColorAsync(ScreenColor* sc, ScreenCoord* coord, int thres)
{
	return 0;
}

int CaptureService::WaitOnColorNotEqual(ScreenColor* sc, ScreenCoord* coord, int thres)
{
	ScreenColor currentColor;
	LOGD("CaptureService: now busy waiting for (%d,%d) is not %x,%x,%x",
		coord->x, coord->y, sc->r, sc->g, sc->b);

	while(thres-- > 0) {
		GetColorOnScreen(&currentColor, coord);
		if (!ColorCompare(sc, &currentColor)) {
			LOGD("CaptureService: Matched! ");
			return 0;
		} else {
			sleep(1);
		}
	}

	return -1;
}

bool CaptureService::ColorCompare(ScreenColor *src, ScreenColor *dest)
{
	return ColorCompare(src, dest, 0x00);
}

/*
 * ColorCompare
 * Compare two colors with error specify
 * TODO: no overflow checking
 */
bool CaptureService::ColorCompare(ScreenColor *src, ScreenColor *dest, int thres)
{
	int r_diff = abs(src->r - dest->r);
	int g_diff = abs(src->g - dest->g);
	int b_diff = abs(src->b - dest->b);

	if((r_diff <= thres) &&
	   (g_diff <= thres) &&
	   (b_diff <= thres)) {
		return true;
	}

	return false;
}
