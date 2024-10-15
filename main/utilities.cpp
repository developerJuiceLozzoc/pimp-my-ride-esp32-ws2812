/*
 * utilities.cpp
 *
 *  Created on: Sep 25, 2024
 *      Author: conner.maddalozzo@gmail.com
 */

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
#include <math.h>
#include <type_traits>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "hal/color_types.h"
#include "lwip/timeouts.h"
#include "sdkconfig.h"
#include "WS2812.hpp"
#include "esp_random.h"
#include "utilities.h"

/**
 * @brief Return an index associated with row and column.
 *
 * every other column is flipped so the row needs to be inverted in such a fashion
 * than added up in another fashion and finally returned as a lump sum.
 *

 * @param [in] row ranges from 0-7 and is the height
 * @param [in] col 0 - 32 and is the how far left right
 */
uint32_t adjustedIndexForRowAndColumn(uint32_t row, uint32_t col) {
   int const height = 8;
   if (col % 2 == 0) {
        // Even columns: top to bottom
        return col * height + row;
    } else {
        // Odd columns: bottom to top
        return col * height + (height - 1 - row);
    }
}


void setPurpleColorForIndex(uint16_t * color, int index) {
	uint16_t purpleColors[numPurpleColors][3] = {
    {270, 255, 10},  // Violet
    {240, 200, 15},  // Purple
    {260, 255, 8},   // Deep Purple
    {260, 150, 20},  // Lavender
    {280, 255, 12}   // Plum
    /*
    {310, 128, 160}, // Violet
        {260, 128, 240}, // Light Purple
    {290, 128, 200}, // Soft Purple

    {320, 128, 140}, // Dark Violet
        {270, 128, 255}, // Bright Purple
    {280, 128, 220}, // Medium Purple

    {330, 128, 120}, // Orchid
    {340, 128, 100}, // Plum
    {270, 100, 255}, // Fuchsia
    {260, 100, 240}, // Magenta
    {280, 100, 220}, // Amethyst
    {290, 100, 200}, // Heather
    {300, 100, 180}, // Purple Heart
    {310, 100, 160}, // Electric Purple
    {320, 100, 140}, // Purple Mountain Majesty
    {330, 100, 120}, // Medium Orchid
    {340, 100, 100}, // Wisteria
    {350, 100, 80},  // Thistle
    */
};
	if ((index < 0) || (index > (numPurpleColors - 1))) {
		color[0] = purpleColors[0][0];
		color[1] = purpleColors[0][1];
		color[2] = purpleColors[0][2];
	} else {
		color[0] = purpleColors[index][0];
		color[1] = purpleColors[index][1];
		color[2] = purpleColors[index][2];
	}
}


void setGreenColorForIndex(uint16_t * color, int index) {
	    uint16_t greenColors[numGreenColors][3] = {
    {120, 255, 10},  // Dark Green
    {350, 100, 8},
    {260, 100, 24},   // Lime Green
    {130, 150, 7},   // Pastel Green
    {140, 100, 6}    // Spring Green
    /*
    {160, 128, 160}, // Medium Green
    {170, 128, 140}, // Sea Green
    {180, 128, 120}, // Mint Green
    {190, 128, 100}, // Pale Green
    {100, 128, 80},  // Moss Green
    {120, 100, 255}, // Neon Green
    {110, 100, 240}, // Chartreuse
    {130, 100, 220}, // Lawn Green
    {140, 100, 200}, // Green Yellow
    {150, 100, 180}, // Olive Green
    {160, 100, 160}, // Jungle Green
    {170, 100, 140}, // Forest Green
    {180, 100, 120}, // Fern Green
    {190, 100, 100}, // Tea Green
    {100, 100, 80}   // Sage Green
    */
};
	if ((index < 0) || (index > (numGreenColors - 1))){
		color[0] = greenColors[0][0];
		color[1] = greenColors[0][1];
		color[2] = greenColors[0][2];
	} else {
		color[0] = greenColors[index][0];
		color[1] = greenColors[index][1];
		color[2] = greenColors[index][2];
	}
}

void setRainColorForIndex(uint16_t * color, int index) {
	uint16_t blueColors[numRainColors][3] = {
    {210, 100, 8},  // Dark Navy Blue
    {220, 100, 7},  // Deep Blue
   {290, 100, 20}, // Heather  // Midnight Blue
    {200, 100, 5},  // Dark Sky Blue
   {290, 100, 20}, // Heather
	    /*
	    {195, 90, 160},  // Light Snow Blue
	    {210, 100, 140}, // Rainy Blue
	    {220, 110, 120}, // Light Rain Blue
	    {230, 120, 100}  // Misty Blue
	    */
	};

	if ((index < 0) || (index > (numRainColors - 1))){
		color[0] = blueColors[0][0];
		color[1] = blueColors[0][1];
		color[2] = blueColors[0][2];
	} else {
		color[0] = blueColors[index][0];
		color[1] = blueColors[index][1];
		color[2] = blueColors[index][2];
	}
}

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
) {
	int index = 0;
	uint16_t charColor[3]; 
	switch (character) {
		case '=':
		case 'D':
		case '8':
			charColor[0] = 290;
			charColor[1] = 255;
			charColor[2] = 6;
			break;
		default:
			charColor[0] = 30;
			charColor[1] = 200;
			charColor[2] = 15;
			break;		
	}
	 switch (character) {
		 case 'H':
		 	for(int i=0;i<6;i++){
				 if(column >= numColumns){
					 continue;
				 }
				 index = adjustedIndexForRowAndColumn(1 + i, column);
				 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				 if(column+3 >= numColumns){
					 continue;
				 }
				 index = adjustedIndexForRowAndColumn(1 + i, column + 3);
				 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 }
			 if (column+1 < numColumns) {
				 index = adjustedIndexForRowAndColumn(3, column+1);
				 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 }
			 if (column+2 < numColumns) {
				 index = adjustedIndexForRowAndColumn(3, column+2);
				 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
		 	break;
		 case 'A':
			 for(int i = 0; i < 4; i ++) {
				 if(column < numColumns) {
					  index = adjustedIndexForRowAndColumn(i+3, column);
					 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				 }
				 if(column + 3 < numColumns) {
					  index = adjustedIndexForRowAndColumn(i+3, column + 3);
					 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				 }
			 }
			 for(int i = 0; i < 2; i ++) {
				 if(column + 1 + i < numColumns) {
					  index = adjustedIndexForRowAndColumn(4, column+1+i);
					 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				 }
			 }
			 if(column+1 < numColumns) {
				 index = adjustedIndexForRowAndColumn(2, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 }
			  if(column+2 < numColumns) {
				 index = adjustedIndexForRowAndColumn(2, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				 index = adjustedIndexForRowAndColumn(1, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 }
			 break;
		 case 'P':
		 	if(column < numColumns) {
				 for(int row = 0; row < 6; row++) {
					index = adjustedIndexForRowAndColumn(row+1, column);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				 }
			 }
			 for(int col = 1; col < 3; col++) {
				if(column + col < numColumns) {
					index = adjustedIndexForRowAndColumn(1, column+col);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
					index = adjustedIndexForRowAndColumn(4, column+col);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				}
			}
			if (column+3 < numColumns) {
				for(int row = 2;  row < 4; row++) {
					index = adjustedIndexForRowAndColumn(row, column+3);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				}
			}
			 break;
		 case 'Y':
			 for(int row = 1; row < 3; row++) {
				if (column < numColumns) {
					index = adjustedIndexForRowAndColumn(row, column);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				}
				
				if (column+3 < numColumns) {
					index = adjustedIndexForRowAndColumn(row, column+3);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				}
			}
			 if (column < numColumns) {
				index = adjustedIndexForRowAndColumn(numRows-2, column);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			}
			
			
			if (column+1 < numColumns) {
				index = adjustedIndexForRowAndColumn(3, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(5, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			}
			if (column+2 < numColumns) {
				index = adjustedIndexForRowAndColumn(3, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(4, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			}
			 break;
		 case 'T':
			 for(int col=0; col< 2;col++){
				 if ((column + col*3) < numColumns) {
					index = adjustedIndexForRowAndColumn(1, column+ col*3);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
			 }
			  for(int row=0; row< 6;row++){
				 if (column+1 < numColumns) {
					index = adjustedIndexForRowAndColumn(1+row, column+1);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
				 if (column+2 < numColumns) {
					index = adjustedIndexForRowAndColumn(1+row, column+2);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
			 }
			 break;
		 case 'U':
		 	 for(int row=0; row< 5;row++){
				 if (column < numColumns) {
					index = adjustedIndexForRowAndColumn(1+row, column);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
				 if (column+3 < numColumns) {
					index = adjustedIndexForRowAndColumn(1+row, column+3);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
			 }
			 if (column+1 < numColumns) {
				index = adjustedIndexForRowAndColumn(numRows-2, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			 if (column+2 < numColumns) {
				index = adjustedIndexForRowAndColumn(numRows-2, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			 break;
		 case 'R':
		 	 for(int row=0; row< 6;row++){
				 if (column < numColumns) {
					index = adjustedIndexForRowAndColumn(1+row, column);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
			 }
			 if (column+1 < numColumns) {
				index = adjustedIndexForRowAndColumn(1, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(4, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			 if (column+2 < numColumns) {
				index = adjustedIndexForRowAndColumn(1, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(4, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				index = adjustedIndexForRowAndColumn(5, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			 if (column+3 < numColumns) {
				index = adjustedIndexForRowAndColumn(2, column+3);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(3, column+3);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				index = adjustedIndexForRowAndColumn(6, column+3);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
		 	break;
		 case 'S':
			  if (column < numColumns) {
					index = adjustedIndexForRowAndColumn(2, column);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
					index = adjustedIndexForRowAndColumn(3, column);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
					index = adjustedIndexForRowAndColumn(numRows - 2, column);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
			 if (column+1 < numColumns) {
				index = adjustedIndexForRowAndColumn(1, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(3, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				index = adjustedIndexForRowAndColumn(6, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			 if (column+2 < numColumns) {
				index = adjustedIndexForRowAndColumn(1, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(4, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				index = adjustedIndexForRowAndColumn(6, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			 if (column+3 < numColumns) {
				index = adjustedIndexForRowAndColumn(1, column+3);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(4, column+3);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				index = adjustedIndexForRowAndColumn(5, column+3);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			 break;
		 case '!':
			 for(int row = 0; row < 4; row++){
				 if(column + 1 < numColumns) {
					 index = adjustedIndexForRowAndColumn(1+row, column+1);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
				  if(column + 2 < numColumns) {
					 index = adjustedIndexForRowAndColumn(1+row, column+2);
					oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
				 }
			 }
		  	if(column + 1 < numColumns) {
				 index = adjustedIndexForRowAndColumn(numRows - 2, column+1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			  if(column + 2 < numColumns) {
				 index = adjustedIndexForRowAndColumn(numRows - 2, column+2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]); 
			 }
			 break;
		 case 'D':
			 for(int i=0;i<6;i++){
				 if(column >= numColumns){
					 continue;
				 }
				 index = adjustedIndexForRowAndColumn(1 + i, column);
				 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 }
			 if(column + 1 < numColumns) {
				index = adjustedIndexForRowAndColumn(1, column + 1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(numRows - 2, column + 1);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 }
			 if(column + 2 < numColumns) {
				index = adjustedIndexForRowAndColumn(1, column + 2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				index = adjustedIndexForRowAndColumn(numRows - 2, column + 2);
				oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 }
			 if(column + 3 < numColumns) {
				for(int i=0;i<4;i++){
				 index = adjustedIndexForRowAndColumn(2 + i, column+3);
				 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 	}
			 }
			 break;
		 case '=':
			 for(int i=0;i<5;i++){
				 if(i+column >= numColumns){
					 continue;
				 }
				 index = adjustedIndexForRowAndColumn(2, i+column);
				 if(shouldAnimateFunny) {
					index = adjustedIndexForRowAndColumn(1, i+column);
				 }
				 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
				 
				 index = adjustedIndexForRowAndColumn(numRows - 3, i+column);
				 if(shouldAnimateFunny) {
					 index = adjustedIndexForRowAndColumn(numRows - 2, i + column);
				 }
				 oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
			 }
			 break;
		 case '8':
			 shouldAnimateFunny = rand() % 2;
		 	for(int col	= 0; col < 4; col++){
				if(col+column >= numColumns){
					 continue;
				}
				switch(col) {
					case 1:
					case 2:
						index = adjustedIndexForRowAndColumn(0, col+column);
						oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
						index = adjustedIndexForRowAndColumn(numRows - 1, col+column);
						oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
						
						if(shouldAnimateFunny) {
							index = adjustedIndexForRowAndColumn(4, col+column);
						oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
						} else {
							index = adjustedIndexForRowAndColumn(3, col+column);
						oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
						}
						
						break;
					case 3:
					case 0:
						for(int row = 0; row < numRows; row++) {
							index = adjustedIndexForRowAndColumn(row, col+column);
							oWS2812->setHSBPixel(index, charColor[0], charColor[1], charColor[2]);
						}
						index = adjustedIndexForRowAndColumn(4, col+column);
						oWS2812->setHSBPixel(index, 0, 0, 0);
						break;
					default:
						break;
				}	 
			}
			break;
		 default:
		 	break;
	 }
 	}
 }



// Define an inline function
inline void setNextState(AnimationShouldStartState *state, int nextState, uint32_t *clicks) {
	*clicks = 0;
	oWS2812->clear();
	oWS2812->show();
	*state = static_cast<AnimationShouldStartState>(nextState);
}

void stateMachineForAnimationStateAndCurrentClicks(
	AnimationShouldStartState* state,
	uint32_t* clicks,
	bool* shouldKickOffAscii,
	uint8_t* surpriseClicks
) {
	bool shouldEndRegularily = false;
	switch(*state) {	
		case AnimationShouldStartState::None:
			setNextState(state, static_cast<int>(AnimationShouldStartState::HappyThursday), clicks);
			break;
        case AnimationShouldStartState::HappyThursday:
			shouldEndRegularily = *clicks == 130;
			if(shouldEndRegularily) {
				*shouldKickOffAscii = true;
				*surpriseClicks += 1;
			}
        	break;
        case AnimationShouldStartState::DickAnimation:
			shouldEndRegularily = *clicks == 60;
			if(shouldEndRegularily) {
				*shouldKickOffAscii = true;
			}
        	break;
        case AnimationShouldStartState::CozyTransitionWave:
			shouldEndRegularily = *clicks == 150;
        	break;

        case AnimationShouldStartState::Rainy:
        case AnimationShouldStartState::DNAPurpleGreenWavy:
			shouldEndRegularily = *clicks == 30;
        	break;
        case AnimationShouldStartState::PurpleCrazyWave:
			shouldEndRegularily = *clicks == 10;
        	break;
    }
    
    if(shouldEndRegularily) {
		if(*surpriseClicks == 3) {
			*surpriseClicks = 0;
			setNextState(state, static_cast<int>(AnimationShouldStartState::DickAnimation), clicks);
		} else {
			setNextState(state, static_cast<int>(*state) + 1, clicks);
		}
	}

}

uint8_t random(uint8_t min,uint8_t max) {
	if (min>max) {
		uint8_t swap;
		swap = min;
		min = max;
		max = swap;
	}
	return (uint8_t)(min + esp_random() % (max + 1 - min));
}


void makeItRain(uint32_t timeOffset) {
    oWS2812->clear();
    int index = 0;

    for (uint32_t col = 0; col < numColumns; col++) {
        // Calculate phase shift for this column based on col and timeOffset
        int phaseShift = (timeOffset + col * 2) % numRows; // Adjust the multiplier for different effects

        if (col % 2 == 1) {
            index = col * 8 + numRows - phaseShift;
        } else {
            index = col * 8 + phaseShift;
        }

	    if (index < GRID_PIXEL_COUNT) {
		    int randomIndex = timeOffset % numRainColors; // the colors re in random order but this ensures every color is used.
			uint16_t color[3] = {0 ,0, 0};
			setRainColorForIndex(color, randomIndex);
		    oWS2812->setHSBPixel(index, color[0], color[1], color[2]);
        }
    }
    oWS2812->show();
}

void randomGreenAndPurpleWavy(uint32_t timeOffset) {
    int const secondWaveOffset = 3;
    int randomIndex;
    int index = 0;
    int chunkedValue;
    double sine_value;
    uint16_t color[3] = {0 ,0, 0};
    
    oWS2812->clear();
    for (uint32_t col = 0; col < numColumns; col++) {
	    sine_value = sin((2 * PI * col) / numColumns + timeOffset); 
	    chunkedValue = (int)((sine_value + 1) * (numRows) / 2);  // Scale sine to [0, numRows-1]
	    index = adjustedIndexForRowAndColumn(chunkedValue, col);
		if (index < GRID_PIXEL_COUNT) {
			randomIndex = timeOffset % numPurpleColors; // the colors re in random order but this ensures every color is used.
			setPurpleColorForIndex(color, randomIndex);
		    oWS2812->setHSBPixel(index, color[0], color[1], color[2]);
		}
		
		sine_value = sin((2 * PI * col) / numColumns + timeOffset + secondWaveOffset); 
	    chunkedValue = (int)((sine_value + 1) * (numRows) / 2);  // Scale sine to [0, numRows-1]
	    index = adjustedIndexForRowAndColumn(chunkedValue, col);
		if (index < GRID_PIXEL_COUNT) {
			randomIndex = timeOffset % numGreenColors; // the colors re in random order but this ensures every color is used.
			setGreenColorForIndex(color, randomIndex);
		    oWS2812->setHSBPixel(index, color[0], color[1], color[2]);
		}
    }
    
    oWS2812->show();
}

void setBG(uint32_t color[]) {
    for (uint32_t index = 0; index < GRID_PIXEL_COUNT; index++) {
		oWS2812->setHSBPixel(index, color[0], color[1], color[2]/10);
    }
}

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
) {
    int index = 0;
    int chunkedValue;
    double sine_value;
    uint16_t *color;
    
    oWS2812->clear();
    
    if(backgroundToAnimate != NULL) {
		for(int i = 0; i < numColumns; i++) {
			for(int row = 0; row < 2; row ++) {
				oWS2812->setHSBPixel(adjustedIndexForRowAndColumn(row, i), backgroundToAnimate[0], backgroundToAnimate[1], backgroundToAnimate[2]);
			}
			for(int row = 0; row < 2; row ++) {
				oWS2812->setHSBPixel(adjustedIndexForRowAndColumn((numRows - 1) - row, i), backgroundToAnimate[0], backgroundToAnimate[1], backgroundToAnimate[2]);
			}
		}
	}
    

    for (uint32_t col = 0; col < numColumns; col++) {
	    sine_value = sin((2 * PI * col) / numColumns + timeOffset); 
	    chunkedValue = (int)((sine_value + 1) * (numRows) / 2);  // Scale sine to [0, numRows-1]
	    index = adjustedIndexForRowAndColumn(chunkedValue, col);
		if (index < GRID_PIXEL_COUNT) {
			color = colorForColumn[col];
		    oWS2812->setHSBPixel(index, color[0], color[1], color[2]);
		}
    }
    
    oWS2812->show();
}

/**
 * @brief to be called every so often to select the next state.
 *
 * every other column is flipped so the row needs to be inverted in such a fashion
 * than added up in another fashion and finally returned as a lump sum.
 *

 * @param [in] timeOffset ranges from 0-7 and is the height
 * @param [in] colorForColumn 1 color for every column,the size of numColumns
 */

bool colorIndexSwitcher(ColorState* state) {
	bool shouldEnd = false;
	switch(state->color) {
        case MyColorType::Rain:
        	shouldEnd = (state->index + 1 == numRainColors);
        	state->index = (state->index + 1) % numRainColors;
        	break;
        case MyColorType::Purple:
             shouldEnd = (state->index + 1 == numPurpleColors);
             state->index = (state->index + 1) % numPurpleColors;
			break;
        case MyColorType::Green:
            shouldEnd = (state->index + 1 == numGreenColors);
             state->index = (state->index + 1) % numGreenColors;
			break;
        case MyColorType::CloseToBlack:
	         state->index += 1;
	         shouldEnd = state->index == 1;
	         break;
        }
       return shouldEnd;
}

/*
sets the array of colors. which is size numColumns.
*/
void updateColorArrForEnum(
	AnimationShouldStartState animationState,
	ColorState *colorState,
	bool* shouldStart,
	uint16_t* numberColumnsTransitioned,
	uint16_t currentSinWaveColors[numColumns][3]
) {
	switch(colorState->color) {
		case MyColorType::Green:
			setGreenColorForIndex(currentSinWaveColors[*numberColumnsTransitioned], colorState->index);
			break;
		case MyColorType::Purple:
			setPurpleColorForIndex(currentSinWaveColors[*numberColumnsTransitioned], colorState->index);
			break;
		case MyColorType::Rain:
			setRainColorForIndex(currentSinWaveColors[*numberColumnsTransitioned], colorState->index);
			break;
		default:
			currentSinWaveColors[*numberColumnsTransitioned][0] = 45;
			currentSinWaveColors[*numberColumnsTransitioned][1] = 45;
			currentSinWaveColors[*numberColumnsTransitioned][2] = 45;
			break;
	}
}

void randomPurpleWave(uint32_t slowedTimeOffset) {
    int index, colorIndex = 0;
    int chunkedValue;
    double sine_value;
    
    oWS2812->clear();
    
	uint16_t color[3] = {0 ,0, 0};
    for (uint32_t col = 0; col < numColumns; col++) {
	    sine_value = sin((2 * PI * col) / numColumns + slowedTimeOffset); 
	    chunkedValue = (int)((sine_value + 1) * (numRows) / 2);  // Scale sine to [0, numRows-1]
	    index = adjustedIndexForRowAndColumn(chunkedValue, col);
		if (index < GRID_PIXEL_COUNT) {
			colorIndex = index % 21;
			setPurpleColorForIndex(color, colorIndex);
		    oWS2812->setHSBPixel(index, color[0], color[1], color[2]/10);
		}
    }
    
    oWS2812->show();
}

bool transitionIfColumnsReached(
	uint16_t* numberColumnsTransitioned,
	ColorState* colorState,
	uint16_t** backgroundColor
) {
	bool shouldTransitionColor = false;
	if (*numberColumnsTransitioned == numColumns - 1) {
		*numberColumnsTransitioned = 0;
		shouldTransitionColor = true;
		colorIndexSwitcher(colorState);
		if (shouldTransitionColor) {
			switch(colorState->color) {
                case MyColorType::Rain:
                	colorState->color = MyColorType::Green;
                	break;
                case MyColorType::Purple:
                    colorState->color = MyColorType::Rain;
					break;
                case MyColorType::Green:
                	colorState->color = MyColorType::Purple;
                	break;
                case MyColorType::CloseToBlack:
                	colorState->color = MyColorType::Rain;
					break;
            }
        }
	} else {
		*numberColumnsTransitioned = (*numberColumnsTransitioned + 1) % numColumns; // probably should just not call this function at this point
	}
	
	if(*backgroundColor != NULL && *numberColumnsTransitioned == numColumns / 4) {
		*backgroundColor = NULL;
	}
	return shouldTransitionColor;
}

void animateDickForOffset(uint16_t offset){
	oWS2812->clear();
	bool shouldWobble = (rand() % 15);
	setCharacterAtPosition(
		'D',
		offset,
		false
	);
	setCharacterAtPosition(
		'=',
		offset - 6,
		shouldWobble	
	);
	setCharacterAtPosition(
		'=',
		offset - 12,
		shouldWobble
	);
	shouldWobble = rand() % 2;
	setCharacterAtPosition(
		'8',
		offset - 18,
		shouldWobble
	);
	oWS2812->show();
}

void animateThursdayForOffset(uint16_t offset){
	int const len = 16;
	char sentence[] = {
		'H',
		'A',
		'P',
		'P',
		'Y',
		'T',
		'H',
		'U',
		'R',
		'S',
		'D',
		'A',
		'Y',
		'!',
		'=',
		'D'
	};
	oWS2812->clear();
	for(int i = len - 1; i >= 0; i--) {
		setCharacterAtPosition(
			sentence[i],
			offset - 6*(len - i),
			(rand()%20) == 0
		);
	}
	oWS2812->show();
	
}