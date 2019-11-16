/*****************************************************
 * structures.h                                      *
 *                                                   *
 * Structures header file.                           *
 * Define all structures and functions to            *
 * manage list of vars. Also define cons.            *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#ifndef structures_h
#define structures_h

#include <stdio.h>
#include <stdbool.h>

#define MAX_WORD_SIZE 100

#define COMMANDS_NUM 21

bool DEBUG;
bool PRODUCTION;
char FILE_NAME[MAX_WORD_SIZE];
//char *FILE_NAME;

int regChanged;

typedef struct var {
    char label[MAX_WORD_SIZE];
    int index;
    int elements;
    struct var *prev, *next;
} Var;

typedef struct operation*(*function)(struct Reg**, Var**, int **, struct operation*, int *statusReg);

typedef struct command {
    char *name;

    int paramNum;   /* 1 OR 2 */

    bool dataSection;

    int regVarType; /* 0 - no reg vars, 1 - first reg var, 2 - 1&2 reg vars */

    function func;
} Command;

typedef struct operation {
    char fullLine[3 * MAX_WORD_SIZE];

    char label[MAX_WORD_SIZE];

    char command[MAX_WORD_SIZE];

    char paramsString[MAX_WORD_SIZE];
    char param1[MAX_WORD_SIZE];
    char param2[MAX_WORD_SIZE];
    int paramsNum;

    bool isLabel;
    bool isCommand;
    bool areParams;

    int lineNum;

    struct command *commandWsk;
    struct operation *prev, *next;
} Operation;

typedef struct reg {
    int value;
    bool isEmpty;
} Reg;

void initRegs(Reg reg[]);

Var *searchVar(Var *head, char *label);

Var *addVar(Var *head, char *label, int index, int elements);

Var *setVar(Var *head, int **varsTab, char *label, int elements, int value, int shift);

Var *getLastVar(Var *head);

Var *foundVar(Var *head, char *label);

Operation *addOperation(Operation *head, Operation op);

Operation *getLastOperation(Operation *head);

#endif
