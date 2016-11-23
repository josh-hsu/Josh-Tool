#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>

#include "SettingService.h"
#include "AppOutput.h"

SettingService::SettingService()
{
	LOGD("SettingService is preparing settings.");
}

void SettingService::PrepareSettings(void)
{

}

void SettingService::GetNameFromSetting(Settings setting, char* file_name)
{
	if (setting > SETTINGS_MAX)
		return;

	switch(setting) {
	case SETTING_RING_TONE_1:
		snprintf(file_name, 30, "ring_tone_1");
		break;
	case SETTING_RING_TONE_1_TYPE:
		snprintf(file_name, 30, "ring_tone_1_type");
		break;
	case SETTING_RING_TONE_2:
		snprintf(file_name, 30, "ring_tone_2");
		break;
	case SETTING_RING_TONE_2_TYPE:
		snprintf(file_name, 30, "ring_tone_2_type");
		break;
	case SETTING_RING_TONE_3:
		snprintf(file_name, 30, "ring_tone_3");
		break;
	case SETTING_RING_TONE_3_TYPE:
		snprintf(file_name, 30, "ring_tone_3_type");
		break;
	case BATTLE_2X_ENABLE:
		snprintf(file_name, 30, "battle_2x_enable");
		break;
	case BATTLE_AUTO_ENABLE:
		snprintf(file_name, 30, "battle_auto_enable");
		break;
	case BATTLE_CHANGE_HERO:
		snprintf(file_name, 30, "battle_change_hero");
		break;
	case BATTLE_STAGE:
		snprintf(file_name, 30, "battle_stage");
		break;
	case BATTLE_NORMAL_TEAM:
		snprintf(file_name, 30, "battle_normal_team");
		break;
	case BATTLE_NORMAL_ROUND:
		snprintf(file_name, 30, "battle_normal_round");
		break;
	case BATTLE_NORMAL_SKILL_1:
		snprintf(file_name, 30, "battle_normal_skill_1");
		break;
	case BATTLE_NORMAL_SKILL_2:
		snprintf(file_name, 30, "battle_normal_skill_2");
		break;
	case BATTLE_NORMAL_SKILL_3:
		snprintf(file_name, 30, "battle_normal_skill_3");
		break;
	case BATTLE_GOLD_TEAM:
		snprintf(file_name, 30, "battle_golden_team");
		break;
	case BATTLE_GOLD_SKILL_1:
		snprintf(file_name, 30, "battle_golden_skill_1");
		break;
	case BATTLE_GOLD_SKILL_2:
		snprintf(file_name, 30, "battle_golden_skill_2");
		break;
	case BATTLE_GOLD_SKILL_3:
		snprintf(file_name, 30, "battle_golden_skill_3");
		break;
	case BATTLE_SCREEN_DISABLE:
		snprintf(file_name, 30, "battle_screen_disable");
		break;
	case BATTLE_TOUCH_DISABLE:
		snprintf(file_name, 30, "battle_touch_disable");
		break;
	case BATTLE_VIBRATE_ENABLE:
		snprintf(file_name, 30, "battle_vibrate_enable");
		break;
	case BATTLE_SENSOR_DISABLE:
		snprintf(file_name, 30, "battle_sensor_disable");
		break;
	default:
		break;
	}
}

bool SettingService::GetBool(Settings setting, bool default_value)
{
	char filename[30];
	char filepath[80];
	char data[2];
	FILE* settingFile;

	GetNameFromSetting(setting, filename);
	snprintf(filepath, 80, "/data/joshtool/%s", filename);
	settingFile = fopen(filepath, "r+");

	if (settingFile == NULL) {
		LOGW("cannot open settingFile %s, using default.", filepath);
		return default_value;
	}

	fgets(data, 2, settingFile); //need include new line symbol
	fclose(settingFile);
	if (data[0] == '1')
		return true;

	return false;
}

int SettingService::GetInt(Settings setting, int default_value)
{
	char filename[30];
	char filepath[80];
	char data[8];
	FILE* settingFile;

	GetNameFromSetting(setting, filename);
	snprintf(filepath, 80, "/data/joshtool/%s", filename);
	settingFile = fopen(filepath, "r+");

	if (settingFile == NULL) {
		LOGW("cannot open settingFile %s", filepath);
		return default_value;
	}

	fgets(data, 8, settingFile);
	fclose(settingFile);

	/* remove the newline if any */
	if (data[strlen(data) - 1] == '\n' || data[strlen(data) - 1] == '\r')
		data[strlen(data) - 1] = '\0';

	return atoi(data);
}

void SettingService::PlaySound(int i)
{
	std::system("am start -a \"android.intent.action.VIEW\" -t \"audio/ogg\" -d \"file:///storage/sdcard0/Ringtones/hangouts_incoming_call.ogg\"");
}
