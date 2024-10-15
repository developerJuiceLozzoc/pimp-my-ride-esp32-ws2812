// ws2812_task.cpp
//
// Copyright (c) 2024 by Conner D Maddalozzo 
// conner.maddalozzo@gmail.com
//
//

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
#include "utilities.h"


#define TAG "WS2812_TASK"
#define numCarPurpleColors 7
#define numCarBabyPhatColors 5
#define numCarRainbowColors 7

 uint16_t carPurpleColors[numCarPurpleColors][3] = {
    {270, 255, 1},  // Violet
    {240, 200, 1},  // Purple
    {260, 255, 8},   // Deep Purple
    {260, 150, 2},  // Lavender
    {280, 255, 1},   // Plum,
    {270, 100, 1}, // Fuchsia
    {260, 100, 9}, // Magenta
};

uint16_t carRainbowColors[numCarRainbowColors][3] = {
    {0, 255, 1},    // Red (Hue = 0°)
    {43, 255, 1},   // Orange (Hue ≈ 43°)
    {85, 255, 1},   // Yellow (Hue = 85°)
    {128, 255, 1},  // Green (Hue = 128°)
    {170, 255, 1},  // Cyan (Hue = 170°)
    {213, 255, 1},  // Blue (Hue = 213°)
    {255, 255, 1}   // Violet (Hue = 255°)
};

uint16_t carPinkGoldColors[numCarBabyPhatColors][3] = {
    {330, 128, 22},  // Pink 1 (Hue ≈ 330°)
    {43, 204, 10},   // Gold 1 (Hue ≈ 43°)
    {350, 128, 10},  // Pink 2 (Hue ≈ 350°)
    {33, 204, 10},   // Gold 2 (Hue ≈ 33°)
    {315, 128, 10}   // Pink 3 (Hue ≈ 315°)
};

enum class CarColorState {
    SegmentedPurple,
    BabyPhat,
    SlowSwooshPurple,
    Rainbow
};

void setCarPurpleAtIndex(uint16_t * color, int index) {
	if ((index < 0) || (index > (numCarPurpleColors - 1))) {
		color[0] = carPurpleColors[0][0];
		color[1] = carPurpleColors[0][1];
		color[2] = carPurpleColors[0][2];
	} else {
		color[0] = carPurpleColors[index][0];
		color[1] = carPurpleColors[index][1];
		color[2] = carPurpleColors[index][2];
	}
}


void setCarBabyPhatAtIndex(uint16_t * color, int index) {
	if ((index < 0) || (index > (numCarBabyPhatColors - 1))) {
		color[0] = carPinkGoldColors[0][0];
		color[1] = carPinkGoldColors[0][1];
		color[2] = carPinkGoldColors[0][2];
	} else {
		color[0] = carPinkGoldColors[index][0];
		color[1] = carPinkGoldColors[index][1];
		color[2] = carPinkGoldColors[index][2];
	}
}

void setCarRainbowColorForIndex(uint16_t * color, int index) {
	if ((index < 0) || (index > (numCarRainbowColors - 1))) {
		color[0] = carRainbowColors[0][0];
		color[1] = carRainbowColors[0][1];
		color[2] = carRainbowColors[0][2];
	} else {
		color[0] = carRainbowColors[index][0];
		color[1] = carRainbowColors[index][1];
		color[2] = carRainbowColors[index][2];
	}
}

inline void setNextCarLightShowState(CarColorState *state, int nextState, uint32_t *clicks) {
	oWS2812->show();
	*state = static_cast<CarColorState>(nextState);
}

void mutateStateForClicks(
	CarColorState* state,
	uint32_t* ticks
) {	
	/*
	HACK: small number for testing. will have to update it to some 2 minute cycle.
	sunset: before i put it in my car
	*/
	int clicksPerCycle = 10 * 60;
    if((*ticks % clicksPerCycle == 0) && *ticks > 0) {
		setNextCarLightShowState(state, static_cast<int>(*state) + 1, ticks);
	}
}

void flushStrip(
	CarColorState state
) {
	uint16_t pixelSlut[3];
	uint8_t numberColors = 0;
	switch(state) {
        case CarColorState::SlowSwooshPurple:
        case CarColorState::SegmentedPurple:
	        numberColors = numCarPurpleColors;
	        break;
        case CarColorState::BabyPhat:
	        numberColors = numCarBabyPhatColors;
			break;
        case CarColorState::Rainbow:
	        numberColors = numCarRainbowColors;
            break;
   }
   
   	uint8_t colorsPerPixel = LINE_PIXEL_COUNT / numberColors;
   	for(int color = 0; color < numberColors; color++) {
		for(int indexOffset = 0; indexOffset < colorsPerPixel; indexOffset++) {
			switch(state) {
                case CarColorState::SegmentedPurple:
                	setCarPurpleAtIndex(
						pixelSlut,
						color
					);
                	break;
                case CarColorState::BabyPhat:
                	setCarBabyPhatAtIndex(
						pixelSlut,
						color
					);
                	break;
                case CarColorState::SlowSwooshPurple:
                	setCarPurpleAtIndex(
						pixelSlut,
						color
					);
                	break;
                case CarColorState::Rainbow:
                	setCarRainbowColorForIndex(
						pixelSlut,
						color
					);
                	break;
                  break;
            }
        
        	oWS2812->setHSBPixel(
				color * colorsPerPixel + indexOffset,
				 pixelSlut[0],
				 pixelSlut[1],
				 pixelSlut[2]
			);
        }	
	}
	
	oWS2812->show();
}

void ws2812_one_dimensional_strip_task() {
 	srand(time(NULL)); // Seed only once, ideally at the start of your program
/*Initialize the stack memory for rest*/
	uint32_t ticks = 0;
	CarColorState state = CarColorState::SegmentedPurple;
	uint16_t delayms= 100; // 10 ticks per second
	oWS2812 = new WS2812(WS2812_GIPO,LINE_PIXEL_COUNT,0);
	for(int color = 0; color < LINE_PIXEL_COUNT; color++) {
		oWS2812->setHSBPixel(
				color,
				255,
				 1,
				1
			);
	}
	oWS2812->show();
/*===================================*/
/* main run loop */
	while(1) {
		 mutateStateForClicks(
			 &state,
			 &ticks
		 );
		 flushStrip(
			 state
		);
        /* increment the ticks after a delay per tick*/
		vTaskDelay(delayms/portTICK_PERIOD_MS);
		ticks++;
		/*==========================================*/
	}
/*=============================*/
	vTaskDelete(NULL);
}
