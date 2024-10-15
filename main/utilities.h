/*
 * utilities.h
 *
 *  Created on: Sep 25, 2024
 *      Author: mrmad
 */

#ifndef MAIN_UTILITIES_H_
#define MAIN_UTILITIES_H_

#include <cstddef>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>
#include <sys/_stdint.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>


#include "esp_system.h"
#include "driver/gpio.h"
//#include "esp_log.h"
#include <math.h>
#include <type_traits>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "hal/color_types.h"
#include "lwip/timeouts.h"
#include "sdkconfig.h"

#include "WS2812.hpp"
#include "ws2812_task.h"

#include "esp_random.h"


#define GRID_PIXEL_COUNT 256
#define LINE_PIXEL_COUNT 144
#define TAG "WS2812_TASK"
#define PI 3.14159265358979323846
#define WS2812_GIPO GPIO_NUM_18
#define numPurpleColors 5
#define numGreenColors 5
#define numRainColors 5
#define numberBeforeSurpise = 1

#define numColumns 32
#define numRows 8


static WS2812 *oWS2812;

enum class AnimationShouldStartState {
	DickAnimation,
	HappyThursday,
    CozyTransitionWave,
    DNAPurpleGreenWavy,
    Rainy,
    PurpleCrazyWave,
    None
};

enum class StripAnimationState {
    Rain,
    Purple,
    Green,
    CloseToBlack
};

enum class MyColorType {
    Rain,
    Purple,
    Green,
    CloseToBlack
};

struct ColorState {
    MyColorType color;                    // Enum to represent color state
    uint16_t index;                           // 16-bit integer index
};



/**
 * @brief Return an index associated with row and column.
 *
 * every other column is flipped so the row needs to be inverted in such a fashion
 * than added up in another fashion and finally returned as a lump sum.
 *

 * @param [in] row ranges from 0-7 and is the height
 * @param [in] col 0 - 32 and is the how far left right
 */
uint32_t adjustedIndexForRowAndColumn(uint32_t row, uint32_t col);


void setPurpleColorForIndex(uint16_t * color, int index);
void setGreenColorForIndex(uint16_t * color, int index);
void setRainColorForIndex(uint16_t * color, int index);

/**
 * @brief Display a charcater that spans column...column+3
 *

 * @param [in] character one of a set that is curated below ranges from 0-7 and is the height
 * @param [in] column the offset. the 4 colummns that make a letter span from here to the right.(not to the right col...col+3)
 * this looks weird if you flip the LED upside down.
 */
 void setCharacterAtPosition(
	 char character,
	 uint8_t column,
	 bool shouldAnimateFunny
);
void stateMachineForAnimationStateAndCurrentClicks(
	AnimationShouldStartState* state,
	uint32_t* clicks,
	bool* shouldKickOffAscii,
	uint8_t* surpriseClicks
);

uint8_t random(uint8_t min,uint8_t max);
void makeItRain(uint32_t timeOffset);
void randomGreenAndPurpleWavy(uint32_t timeOffset);

void setBG(uint32_t color[]);

/**
 * @brief Return an index associated with row and column.
 *
 * every other column is flipped so the row needs to be inverted in such a fashion
 * than added up in another fashion and finally returned as a lump sum.
 *

 * @param [in] timeOffset ranges from 0-7 and is the height
 * @param [in] colorForColumn 1 color for every column,the size of numColumns
 */
void flushWaveForTimeAndColor(
	uint32_t timeOffset,
	uint16_t colorForColumn[numColumns][3],
	uint16_t *backgroundToAnimate
);

/**
 * @brief to be called every so often to select the next state.
 *
 * every other column is flipped so the row needs to be inverted in such a fashion
 * than added up in another fashion and finally returned as a lump sum.
 *

 * @param [in] timeOffset ranges from 0-7 and is the height
 * @param [in] colorForColumn 1 color for every column,the size of numColumns
 */

bool colorIndexSwitcher(ColorState* state);

/*
sets the array of colors. which is size numColumns.
*/
void updateColorArrForEnum(
	AnimationShouldStartState animationState,
	ColorState *colorState,
	bool* shouldStart,
	uint16_t* numberColumnsTransitioned,
	uint16_t currentSinWaveColors[numColumns][3]
);
void randomPurpleWave(uint32_t slowedTimeOffset);
bool transitionIfColumnsReached(
	uint16_t* numberColumnsTransitioned,
	ColorState* colorState,
	uint16_t** backgroundColor
);
void animateDickForOffset(uint16_t offset);
void animateThursdayForOffset(uint16_t offset);



#endif /* MAIN_UTILITIES_H_ */
