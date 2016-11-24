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

#ifndef _CAPTURESERVICE_H_
#define _CAPTURESERVICE_H_

/*
 * Landscape view (xl, yl) v.s. Potraint view
 * 
 *   xl = yp
 *   yl = screen_width - xp
 */
enum ScreenOrientation {
	SO_Portrait,
	SO_Landscape,
	SO_Max
};

struct ScreenColor {
	unsigned char b;	/* blue */
	unsigned char g;	/* green */
	unsigned char r;	/* red */
	unsigned char t;	/* transparent */
};

struct ScreenCoord {
	int x;
	int y;
	ScreenOrientation orien;
};

struct ScreenPoint {
	ScreenCoord coord;
	ScreenColor color;
};

class CaptureService {
  public:
	CaptureService();

	/* get color on screen in Android WindowManager coordinate x, y */
	void GetColorOnScreen(ScreenColor* sc, ScreenCoord* coord);

	/* block waiting until the color on the screen (x,y) turn into */
	int WaitOnColor(ScreenColor* sc, ScreenCoord* coord, int thres);
	int WaitOnColor(ScreenPoint* sp, int thres);
	int WaitOnColorKindOf(ScreenPoint* sp, int check_time, int thres);
	int WaitOnColorKindOf(ScreenColor* sc, ScreenCoord* coord, int check_time, int thres);

	/* async waiting until the color on the screen (x,y) turn into */
	int WaitOnColorAsync(ScreenColor* sc, ScreenCoord* coord, int thres);

	/* block waiting until the color on the screen (x,y) turn into */
	int WaitOnColorNotEqual(ScreenColor* sc, ScreenCoord* coord, int thres);

	/* directly return if color on that point is correct */
	bool ColorIs(ScreenPoint* point);
	bool ColorKindOf(ScreenPoint* point, int thres);

	/* compare two colors, return true if two colors are the same */
	bool ColorCompare(ScreenColor *src, ScreenColor *dest);
	bool ColorCompare(ScreenColor *src, ScreenColor *dest, int thres);

	/* save screenshot */
	void SaveScreenshot(const char* filepath);

  //private:
	void DumpScreen(const char* filename);
	void DumpScreen(void);
	void GetColorOnDumpInternal(ScreenColor* sc, const char* filename, ScreenCoord* coord);
	void GetColor(ScreenColor* sc, ScreenCoord* coord);
};

#endif
