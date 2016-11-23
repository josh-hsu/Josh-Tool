/*
 * fgo_zb551kl_script.h
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
 * Pre-setup
 */
struct ScreenPoint homePageButtonPoint = {{108, 876, SO_Portrait}, {0x01, 0x3F, 0xA4, 0x00}};
struct ScreenPoint agreementButtonPoint = {{171, 769, SO_Portrait}, {0x10, 0x10, 0x10, 0x00}};

/*
 * intro
 */
struct ScreenPoint skipButtonPoint = {{682, 1176, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
struct ScreenPoint confirmSkipButtonPoint = {{165, 796, SO_Portrait}, {0x00, 0x00, 0x00, 0x00}};

/*
 * Teaching battle
 */
struct ScreenPoint drawCardButtonPoint = {{151, 1127, SO_Portrait}, {0x01, 0xD0, 0xF1, 0x00}};
struct ScreenPoint card1ButtonPoint = {{217, 128, SO_Portrait}, noColor};
struct ScreenPoint card2ButtonPoint = {{213, 382, SO_Portrait}, noColor};
struct ScreenPoint card3ButtonPoint = {{210, 640, SO_Portrait}, noColor};
//repeat draw
//select card
//repeat draw
//select card

//repeat draw
struct ScreenPoint touchDismissButtonPoint = {{439, 636, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
//select card

//repeat draw
struct ScreenPoint royalCardButtonPoint = {{507, 416, SO_Portrait}, noColor};
//select card1 and 2

/*
 * Intro 1 - before zone F
 */
//wait for skip
//wait for confirm

/*
 * Create character
 */
struct ScreenPoint touchNameButtonPoint = {{368, 911, SO_Portrait}, {0x30, 0x30, 0x30, 0x00}};
//send input word and return, in case return dosent work
struct ScreenPoint returnButtonPoint = {{564, 1143, SO_Portrait}, noColor};
//confirm name 
struct ScreenPoint confirmNameButtonPoint = {{158, 828, SO_Portrait}, {0x00, 0x00, 0x00, 0x00}};

/*
 * Intro 2 - ready to go in zone F
 */
//wait for skip
//wait for confirm

/*
 * Zone F, not confirmed coordination X-A
 */
struct ScreenPoint enterZone1ButtonPoint = {{351, 666, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
struct ScreenPoint enterZone1SubButtonPoint = {{501, 853, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
//wait for skip
//wait for confirm
//wait for draw
//draw card

//wait for text to use skill
struct ScreenPoint skillTutorTextPoint = {{357, 622, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
//tap skill(do not use this skill button color)
struct ScreenPoint skill1ButtonPoint = {{147, 161, SO_Portrait}, {0x6C, 0x28, 0x06, 0x00}};
struct ScreenPoint useSkillButtonPoint = {{298, 835, SO_Portrait}, {0xFB, 0xFB, 0xFB, 0x00}};
struct ScreenPoint skillTargetButtonPoint = {{270, 637, SO_Portrait}, {0x1A, 0x1A, 0x1A, 0x00}};
//wait for draw
//tap speed up battle
struct ScreenPoint speedUpButtonPoint = {{662, 1127, SO_Portrait}, {0xFA, 0xF9, 0xF9, 0x00}};
//draw

//wait for draw
//draw

//battle finished
struct ScreenPoint bondButtonPoint = {{535, 104, SO_Portrait}, {0xE9, 0xB7, 0x23, 0x00}};
struct ScreenPoint expButtonPoint = {{517, 656, SO_Portrait}, {0xE6, 0xB4, 0x30, 0x00}};
struct ScreenPoint nextButtonPoint = {{39, 1106, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};

//wait for skip
//wait for confirm
//get magic stone
struct ScreenPoint magicStoneButtonPoint = {{197, 712, SO_Portrait}, {0xFF, 0xCD, 0x00, 0x00}};


/*
 * Zone F, not confirmed coordination X-B
 */
struct ScreenPoint enterZone2ButtonPoint = {{359, 667, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
struct ScreenPoint enterZone2SubButtonPoint = {{506, 848, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
//wait for skip
//wait for confirm
//wait for draw
//draw
//wait for tap moster
struct ScreenPoint tapMosterTextPoint = {{646, 247, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
struct ScreenPoint tapMosterButtonPoint = {{401, 297, SO_Portrait}, noColor};
//wait for draw
//draw
//repeat till bone
//tap till nextButtonPoint

//wait for skip
//wait for confirm
//get magic stone

/*
 * First 10 draw
 */
struct ScreenPoint menuButtonPoint = {{31, 1145, SO_Portrait}, {0x00, 0x00, 0x01, 0x00}};
struct ScreenPoint summonButtonPoint = {{75, 536, SO_Portrait}, {0x31, 0x42, 0xB5, 0x00}};
struct ScreenPoint tenDrawButtonPoint = {{148, 612, SO_Portrait}, {0xF4, 0xF3, 0xDD, 0x00}};
struct ScreenPoint confirmDrawButtonPoint = {{163, 824, SO_Portrait}, {0x00, 0x00, 0x00, 0x00}};

//tap tap tap until next
struct ScreenPoint summonDoneButtonPoint = {{46, 1127, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};

//tap summon to return menu
struct ScreenPoint summonDoneSummonButtonPoint = {{59, 759, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
//tap menuButtonPoint
//tap team
struct ScreenPoint teamButtonPoint = {{87, 182, SO_Portrait}, {0x30, 0x41, 0xB4, 0x00}};
struct ScreenPoint partyButtonPoint = {{557, 918, SO_Portrait}, {0x45, 0x45, 0x45, 0x00}};
struct ScreenPoint member2ButtonPoint = {{416, 318, SO_Portrait}, {0xE4, 0xE4, 0xE4, 0x00}};
struct ScreenPoint memberTutorTextPoint = {{419, 581, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
struct ScreenPoint selectServantButtonPoint = {{448, 364, SO_Portrait}, noColor};
struct ScreenPoint partyConfigDoneButtonPoint = {{54, 1183, SO_Portrait}, {0x27, 0x2F, 0x66, 0x00}};
struct ScreenPoint partyCloseButtonPoint = {{684, 102, SO_Portrait}, {0x29, 0x33, 0x68, 0x00}};
struct ScreenPoint teamCloseButtonPoint = {{683, 95, SO_Portrait}, {0x1D, 0x25, 0x51, 0x00}};

/*
 * Zone F, not confirmed coordination X-C
 */
struct ScreenPoint enterZone3ButtonPoint = {{350, 674, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
struct ScreenPoint enterZone3SubButtonPoint = {{513, 853, SO_Portrait}, {0xFD, 0xFD, 0xFD, 0x00}};
struct ScreenPoint selectSupportTextPoint = {{325, 715, SO_Portrait}, {0xFF, 0xFF, 0xFF, 0x00}};
struct ScreenPoint selectSupportButtonPoint = {{460, 289, SO_Portrait}, noColor};
struct ScreenPoint questStartButtonPoint = {{55, 1183, SO_Portrait}, {0x34, 0x3F, 0x71, 0x00}};

//wait for skip
//wait for confirm skip

//battle standard

//wait for bond

//tap until next

struct ScreenPoint applyFriendButtonPoint = {{104, 301, SO_Portrait}, {0xB4, 0xB4, 0xB4, 0x00}};

//wait for skip
//wait for confirm

//dump 87


/*
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
struct ScreenPoint ButtonPoint = {{, , SO_Portrait}, {0x, 0x, 0x, 0x00}};
*/
