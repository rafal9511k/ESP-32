/*
 * print.h
 *
 *  Created on: 30 mar 2020
 *      Author: root
 */

#ifndef MAIN_PRINT_H_
#define MAIN_PRINT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COLOR_RED		"31"
#define COLOR_GREEN		"32"
#define COLOR_YELLOW	"33"
#define COLOR_BLUE		"34"
#define COLOR_MAGENTA	"35"
#define COLOR_CYAN		"36"

#define Print_enable 0

// Print text in color
#define printC(COLOR, text, ...) printf("\033[0;" COLOR "m" text "\033[0m\n", ##__VA_ARGS__)

// Print text in color with function name
#define printCF(COLOR, text, ...) printf("\033[0;" COLOR "m" "%s" " : " text "\033[0m\n",__func__,  ##__VA_ARGS__)


#endif /* MAIN_PRINT_H_ */
