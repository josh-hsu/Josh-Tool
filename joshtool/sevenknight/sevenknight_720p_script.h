/*
 * sevenknight_zb551kl_script.h
 * 
 * This header is suitable for all 720x1280 display Android devices.
 */

#include "../libjoshgame/libjoshgame.h"

#define NO_COLOR_T	0x1C
/* ------------------------------------
 * Remember the color is b g r t format
 * ------------------------------------ */
struct ScreenColor noColor = {0x00, 0x00, 0x00, NO_COLOR_T};


/*
 * Server related points
 */
struct ScreenPoint disconnectedButtonPoint = {{194, 526, SO_Portrait}, {0x6B, 0x61, 0x31, 0x00}};
struct ScreenPoint backButtonTopLeftPoint  = {{686,  74, SO_Portrait}, {0xC5, 0x3D, 0x08, 0x00}};

/*
 * Info related points
 * 
 *   Aragon
 *   Picture       Name              Title
 *                Content
 *                       Option A       Option B
 */

struct ScreenPoint infoNameFieldColorPoint = {{18, 402, SO_Portrait}, {0x8C, 0x69, 0x29, 0x00}};
struct ScreenPoint infoOptionAButtonPoint  = {{60, 832, SO_Portrait},  {0xB5, 0x89, 0x21, 0x00}};
struct ScreenPoint infoOptionBButtonPoint  = {{68, 1061, SO_Portrait}, {0xFF, 0xAE, 0x10, 0x00}};
struct ScreenPoint infoDragonHasSpownedPoint = {{19, 110, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};

/*
 * Home related points
 */
struct ScreenPoint homeAdvantureButtonPoint = {{61, 1181, SO_Portrait}, {0xF7, 0xDA, 0xA4, 0x00}};
struct ScreenPoint homeBattleButtonPoint = {{57, 999, SO_Portrait}, {0xF7, 0xCE, 0x9C, 0x00}};
struct ScreenPoint homeDetectPoint = {{635, 337, SO_Portrait}, {0x6B, 0x69, 0x6B, 0x00}};


/*
 * Game Field related points
 */
struct ScreenPoint fieldNormalButtonPoint   = {{291, 463, SO_Portrait}, {0xDC, 0xB8, 0x71, 0x00}};
struct ScreenPoint fieldDailyButtonPoint    = {{291, 463, SO_Portrait}, {0xDC, 0xB8, 0x71, 0x00}};
struct ScreenPoint fieldDragonButtonPoint   = {{291, 463, SO_Portrait}, {0xDC, 0xB8, 0x71, 0x00}};
struct ScreenPoint fieldInfinityButtonPoint = {{291, 463, SO_Portrait}, {0xDC, 0xB8, 0x71, 0x00}};
struct ScreenPoint fieldArenaButtonPoint;
struct ScreenPoint fieldGuildButtonPoint;
struct ScreenPoint fieldSiegeButtonPoint;

// Normal game related points
struct ScreenPoint fieldNewAdvantureButtonPoint = {{76, 1100, SO_Portrait}, {0x94, 0x6D, 0x39, 0x00}};
struct ScreenPoint fieldAsterStateButtonPoint   = {{64, 1212, SO_Portrait}, {0x94, 0x61, 0x42, 0x00}};

/*
 * Battle related points
 */
// Hero changing points
/*
 *        hero1   |
 *        hero2   |    H1     H2     H3      H4
 *   P    hero3   |
 *        hero4   |    H5     H6     H7      H8
 *                |
 */
struct ScreenPoint prepareChangingHeroButtonPoint = {{36, 237, SO_Portrait}, {0xCE, 0x96, 0x7B, 0x00}};
struct ScreenPoint heroInChangingScreenPoint = {{563, 204, SO_Portrait}, {0xEF, 0xCE, 0xA4, 0x00}};
struct ScreenPoint hero1L1CheckPoint = {{491, 326, SO_Portrait}, {0xEF, 0xC6, 0x21, 0x00}};
struct ScreenPoint hero2L1CheckPoint = {{364, 326, SO_Portrait}, {0xFF, 0xC6, 0x21, 0x00}};
struct ScreenPoint hero3L1CheckPoint = {{238, 326, SO_Portrait}, {0xF7, 0xA6, 0x08, 0x00}};

struct ScreenPoint hero1SelectPoint  = {{486, 523, SO_Portrait}, noColor};
struct ScreenPoint hero2SelectPoint  = {{478, 721, SO_Portrait}, noColor};
struct ScreenPoint hero3SelectPoint  = {{478, 928, SO_Portrait}, noColor};
struct ScreenPoint hero4SelectPoint  = {{473, 1125, SO_Portrait}, noColor};
struct ScreenPoint hero5SelectPoint  = {{186, 523, SO_Portrait}, noColor};
struct ScreenPoint hero6SelectPoint  = {{190, 718, SO_Portrait}, noColor};
struct ScreenPoint hero7SelectPoint  = {{184, 918, SO_Portrait}, noColor};
struct ScreenPoint hero8SelectPoint  = {{195, 1122, SO_Portrait}, noColor};

struct ScreenPoint heroL1CheckPoints[3] = {hero1L1CheckPoint, hero2L1CheckPoint, hero3L1CheckPoint};
struct ScreenPoint heroSelectPoints[8] = {hero1SelectPoint, hero2SelectPoint, hero3SelectPoint,
	hero4SelectPoint, hero5SelectPoint, hero6SelectPoint, hero7SelectPoint, hero8SelectPoint};
struct ScreenPoint heroSelectFailPoint = {{445, 347, SO_Portrait}, {0xFF, 0xEB, 0xAD, 0x00}};
struct ScreenPoint heroPagechangePoint = {{91, 1119, SO_Portrait}, {0xF7, 0xE7, 0x84, 0x00}};

struct ScreenPoint heroLowToHighPoint = {{600, 1176, SO_Portrait}, {0xCE, 0x9A, 0x84, 0x00}};
struct ScreenPoint heroHighToLowPoint = {{611, 1175, SO_Portrait}, {0xD6, 0x9A, 0x84, 0x00}};

// Prepare state
struct ScreenPoint prepareGoldenHouseButtonPoint = {{74, 910,  SO_Portrait}, {0x6B, 0x69, 0x9C, 0x00}};
struct ScreenPoint prepareNormalButtonPoint      = {{64, 1005, SO_Portrait}, {0xFF, 0xFF, 0xA4, 0x00}};
/* following color is non choose */
struct ScreenPoint team1ChooseButtonPoint = {{628, 213, SO_Portrait}, {0x42, 0x2D, 0x11, 0x00}};
struct ScreenPoint team2ChooseButtonPoint = {{628, 343, SO_Portrait}, {0x42, 0x35, 0x11, 0x00}};
struct ScreenPoint team3ChooseButtonPoint = {{627, 475, SO_Portrait}, {0x42, 0x2C, 0x19, 0x00}};
struct ScreenPoint teamChooseButtonPoint[3] = {team1ChooseButtonPoint, team2ChooseButtonPoint, team3ChooseButtonPoint};
/* exceptions */
struct ScreenPoint prepareFailNoBlueKeyPoint = {{396, 479, SO_Portrait}, {0xCE, 0x31, 0x08, 0x00}}; //Tower
struct ScreenPoint prepareFailNoSilverKeyPoint; // Normal

// In battle state
struct ScreenPoint battleBarPoint = {{692, 541, SO_Portrait}, {0xF7, 0xE3, 0x19, 0x00}};
struct ScreenPoint battleNormalPauseButtonPoint = {{607, 1170, SO_Portrait}, {0xBD, 0xAA, 0x63, 0x00}};
struct ScreenPoint battleNormalMessageButtonPoint = {{599, 1218, SO_Portrait}, {0x4A, 0x20, 0x10, 0x00}};
struct ScreenPoint battlePauseButtonPoint = {{671, 1149, SO_Portrait}, {0xB5, 0x9A, 0x52, 0x00}};
struct ScreenPoint battleMessageButtonPoint = {{681, 1233, SO_Portrait}, {0xD6, 0xC2, 0x7B, 0x00}};
struct ScreenPoint battleAutoOnPoint = {{73, 165, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
struct ScreenPoint battleAutoOffPoint = {{64, 160, SO_Portrait}, {0xBD, 0xA6, 0x7B, 0x00}};
struct ScreenPoint battleX2OffPoint = {{53, 60, SO_Portrait}, {0xBD, 0xAA, 0x7B, 0x00}};

	/*
	 *  Skill     2    4    6    8    10
	 *            1    3    5    7    9
	 */
struct ScreenPoint battleSkill1  = {{50,  770, SO_Portrait}, noColor};
struct ScreenPoint battleSkill2  = {{150, 770, SO_Portrait}, noColor};
struct ScreenPoint battleSkill3  = {{50,  900, SO_Portrait}, noColor};
struct ScreenPoint battleSkill4  = {{150, 900, SO_Portrait}, noColor};
struct ScreenPoint battleSkill5  = {{50,  1000, SO_Portrait}, noColor};
struct ScreenPoint battleSkill6  = {{150, 1000, SO_Portrait}, noColor};
struct ScreenPoint battleSkill7  = {{50,  1115, SO_Portrait}, noColor};
struct ScreenPoint battleSkill8  = {{150, 1115, SO_Portrait}, noColor};
struct ScreenPoint battleSkill9  = {{50,  1220, SO_Portrait}, noColor};
struct ScreenPoint battleSkill10 = {{150, 1220, SO_Portrait}, noColor};
struct ScreenPoint battleSkill[10] = {battleSkill1, battleSkill2, battleSkill3, battleSkill4,
	battleSkill5, battleSkill6, battleSkill7, battleSkill8,
	battleSkill9, battleSkill10};


// Result state
// === Inifinity tower ===
struct ScreenPoint resultHeroButtonPoint     = {{44,  67,   SO_Portrait}, {0xF7, 0xCE, 0xAD, 0x00}};
struct ScreenPoint resultInfiniteButtonPoint = {{211, 1177, SO_Portrait}, {0xEF, 0xD2, 0x6B, 0x00}};
struct ScreenPoint resultInfiniteAgainButtonPoint = {{374, 1180, SO_Portrait}, {0x5A, 0x24, 0x10, 0x00}};

// === Normal game ===
struct ScreenPoint resultNormalAgainButtonPoint = {{512, 1149, SO_Portrait}, {0xF7, 0xD7, 0x6B, 0x00}};
struct ScreenPoint resultNormalNextRegionButtonPoint = {{383, 1152, SO_Portrait}, {0xAD, 0x81, 0x31, 0x00}};
struct ScreenPoint resultNormalMapButtonPoint = {{210, 1170, SO_Portrait}, {0xCE, 0xB2, 0x6B, 0x00}};
struct ScreenPoint resultNormalVillegeButtonPoint  = {{89,  1167, SO_Portrait}, {0x7B, 0x51, 0x10, 0x00}};
struct ScreenPoint resultNormalQuickAgainButtonPoint = {{94, 1016, SO_Portrait}, {0xB5, 0x7D, 0x3A, 0x00}};

