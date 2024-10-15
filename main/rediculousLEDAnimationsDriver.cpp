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
#include "utilities.h"
#include "esp_random.h"



void ws2812_task() {
  srand(time(NULL)); // Seed only once, ideally at the start of your program
/*Initialize the stack memory for rest*/
	uint32_t ticks = 0;
	uint32_t stateMachineTicks = 0;
	AnimationShouldStartState state = AnimationShouldStartState::None;
	ColorState colorState = ColorState();
	colorState.index = 0;
	colorState.color = MyColorType::Purple;
	uint16_t* currentAnimatingBackground = NULL;
	bool animationBeginningToggle = false;
	uint16_t relativeOffsetForColorTransition = 0;
	uint16_t relativeOffsetForCharacters = 0;
	uint16_t colorArrForOneWave[numColumns][3];
	uint8_t shouldSurpriseUser = 0;
/*  
i would like to have 24 ticks per second. to achieve this
	24 ticks * 1 second
	1 second * 1000 ms
	
	x ms * 24 tick * 1 sec
	1 tick * 1 sec * 1000 ms
	 wait thats not right
*/
	uint16_t delayms= 100;
	oWS2812 = new WS2812(WS2812_GIPO,GRID_PIXEL_COUNT,0);
/*===================================*/
/* main run loop */
	while(1) {
		stateMachineForAnimationStateAndCurrentClicks(&state, &stateMachineTicks, &animationBeginningToggle, &shouldSurpriseUser);
		if(animationBeginningToggle) {
			relativeOffsetForCharacters = 0;
			animationBeginningToggle = false;
		}
		switch(state) {
			case AnimationShouldStartState::DickAnimation:
				animateDickForOffset(relativeOffsetForCharacters);
				relativeOffsetForCharacters += 1;
				// this will have a seperate tick
				break;
            case AnimationShouldStartState::HappyThursday:
            	animateThursdayForOffset(6*17 + numColumns - relativeOffsetForCharacters);
            	relativeOffsetForCharacters += 1;
            	break;
			case AnimationShouldStartState::Rainy:
				makeItRain(ticks);
				break;
			case AnimationShouldStartState::DNAPurpleGreenWavy:
				randomGreenAndPurpleWavy(ticks);
				break;
   			case AnimationShouldStartState::PurpleCrazyWave:
            case AnimationShouldStartState::None:
            	break;
            case AnimationShouldStartState::CozyTransitionWave:
           		flushWaveForTimeAndColor(
					   ticks, 
					   colorArrForOneWave,
					   currentAnimatingBackground
				);
        		if (ticks % 3 == 0) {
					updateColorArrForEnum(
						state, 
						&colorState,
						&animationBeginningToggle,
						&relativeOffsetForColorTransition,
						colorArrForOneWave
					);
					bool okay = transitionIfColumnsReached(
						&relativeOffsetForColorTransition,
						&colorState,
						&currentAnimatingBackground
					);
					if(okay) {
						currentAnimatingBackground = colorArrForOneWave[colorState.index];
						currentAnimatingBackground[2] /= 10; 
					}
				}
				break;
       }
                /* increment the ticks after a delay per tick*/
		vTaskDelay(delayms/portTICK_PERIOD_MS);
		stateMachineTicks++;
		ticks++;
		/*==========================================*/
	}
/*=============================*/
	vTaskDelete(NULL);
}
