/*****************************************************
 * commands.h                                        *
 *                                                   *
 * Commands header file.                             *
 * Declare some helper functions and                 *
 * functions to specific commands.                   *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#ifndef commands_h
#define commands_h

#include <stdbool.h>
#include <ctype.h>

#include "structures.h"
#include "interface.h"

int toInt(char *word);

void checkVarExistence(Var *vars, char *label, int lineNum);

int *getVar(Var *vars, int *varsTab, Reg reg[], char *label, int lineNum);

int operateOnReg(Reg reg[], int regNum, char type, int value, int stateValue, int lineNum);

int getState(int output);

Operation *findOperation(Operation *currentOp, char *label);

void initCommands(Command commands[]);

/* Commands functions */
Operation *execDS  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execDC  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execA   (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execAR  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execS   (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execSR  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execM   (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execMR  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execD   (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execDR  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execC   (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execCR  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execJ   (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execJZ  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execJP  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execJN  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execL   (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execLA  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execLR  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execST  (Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);
Operation *execREAD(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg);

#endif
