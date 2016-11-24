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

#ifndef _SETTINGSERVICE_H_
#define _SETTINGSERVICE_H_

enum Settings {
	SETTING_RING_TONE_1,
	SETTING_RING_TONE_1_TYPE,
	SETTING_RING_TONE_2,
	SETTING_RING_TONE_2_TYPE,
	SETTING_RING_TONE_3,
	SETTING_RING_TONE_3_TYPE,
	BATTLE_2X_ENABLE,
	BATTLE_AUTO_ENABLE,
	BATTLE_CHANGE_HERO,
	BATTLE_STAGE,
	BATTLE_NORMAL_TEAM,
	BATTLE_NORMAL_ROUND,
	BATTLE_NORMAL_SKILL_1,
	BATTLE_NORMAL_SKILL_2,
	BATTLE_NORMAL_SKILL_3,
	BATTLE_GOLD_TEAM,
	BATTLE_GOLD_SKILL_1,
	BATTLE_GOLD_SKILL_2,
	BATTLE_GOLD_SKILL_3,
	BATTLE_SCREEN_DISABLE,
	BATTLE_TOUCH_DISABLE,
	BATTLE_VIBRATE_ENABLE,
	BATTLE_SENSOR_DISABLE,
	SETTINGS_MAX
};

class SettingService {
  public:
	SettingService();

	bool GetBool(Settings setting, bool default_value);
	int GetInt(Settings setting, int default_value);
	char* GetString(Settings setting, const char* default_value);

	void PlaySound(int type);

	//FGO specific function
	void KillFGO(void);
	void StartFGO(void);
	void SaveGameData(const char* root);

  private:
	void PrepareSettings(void);
	void GetNameFromSetting(Settings setting, char* file_name);
	
};

#endif
