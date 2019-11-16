/*****************************************************
 * interface.h                                       *
 *                                                   *
 * Interface header file.                            *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#ifndef interface_h
#define interface_h

#include "windows.h"
#include <stdio.h>
#include <conio.h>

#include "structures.h"

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define ESC             "\x1b["

#define COLOR_RED       ESC "31m"
#define COLOR_GREEN     ESC "32m"
#define COLOR_YELLOW    ESC "33m"
#define COLOR_BLUE      ESC "34m"
#define COLOR_MAGENTA   ESC "35m"
#define COLOR_CYAN      ESC "36m"
#define COLOR_BLACK     ESC "30;1m"

#define BOLD            ESC "1m"
#define UNDERLINE       ESC "4m"
#define REVERSE         ESC "7m"

#define BG_RED          ESC "41;1m"
#define BG_WHITE        ESC "30;47m"

#define RESET           ESC "0m"

#define RED_STRING(string)       COLOR_RED string RESET
#define GREEN_STRING(string)     COLOR_GREEN string RESET
#define YELLOW_STRING(string)    COLOR_YELLOW string RESET
#define BLUE_STRING(string)      COLOR_BLUE string RESET
#define MAGENTA_STRING(string)   COLOR_MAGENTA string RESET
#define CYAN_STRING(string)      COLOR_CYAN string RESET

#define BG_RED_STRING(string)    BG_RED string RESET

#define BOLD_STRING(string)      BOLD string RESET
#define UNDERLINE_STRING(string) UNDERLINE string RESET

#define GO_TO(string, string2)   ESC string ";" string2 "H"

#define SAVE_POSITION            ESC "s"
#define RESTORE_POSITION         ESC "u"

#define SHIFT(string)            ESC string "C"

#define SHOW_OPERATIONS_START     2
#define SHOW_REGISTERS_START      65
#define SHOW_VARS_START           120

#define REGISTERS_ROWS           24

void initInterface();

void logError(char *errorInfo, int errorNum, int lineNum);

void clearScreen();

void readData();

void sayHello();

void printHeader(char *text, int startRow, int startCol);

void sayGoodbye(int opNum);

void nextOperation(Reg reg[], Var *vars, int *varsTab, int statusReg, Operation *operations, int currentLine, int opNum);

void printVars(Var *head, int *varTab);

void printOperationsStructure(Operation *head);

void printOperations(Operation *head, int currentLine);

void printRegs(Reg reg[], int statusReg);

char *shortenToSize(char *word, int maxSize);

void printCurrentOperation(Operation *operations, int currentLine, int opNum);

#endif
