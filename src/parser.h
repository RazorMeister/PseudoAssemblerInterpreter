/*****************************************************
 * parsers.h                                         *
 *                                                   *
 * Parser header file.                               *
 * Declare functions for parsers.c file.             *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#ifndef parser_h
#define parser_h

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "structures.h"
#include "interface.h"

Operation *parse(Command commands[]);

FILE *openFile();

int isCommand(char *word, Command command[]);

void checkLabelValidity(char *word, int lineNum);

Operation *checkWord(char *word, int index, int *status, int lineNum, Operation *operations, bool *inDataSection, Command commands[]);

bool isValidRegister(char *param);

void addParam(Operation *operation, char *word);

void checkParams(Operation *operation);

void validateOperation(Operation *operation);

#endif
