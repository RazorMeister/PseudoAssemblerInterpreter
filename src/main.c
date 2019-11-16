/*****************************************************
 * main.c                                            *
 *                                                   *
 * Main program file.                                *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS

/* Including header files */
#include "structures.h"
#include "interface.h"
#include "commands.h"
#include "parser.h"

/*
 * Main.
 *
 * Main function.
 */
int main()
{
    /* Variables declaration */
    Var *vars = NULL;
    Reg reg[14];
    int statusReg, *varsTab = NULL, i = 0, lastLine; /* statusReg: 0 - zero, 1 +, 2 -, 3 error */
    Command commands[COMMANDS_NUM];
    Operation *operations = NULL, *current;

    /* Initialize regs */
    initRegs(reg);

    /* Initialize interface */
    initInterface();

    /* Show first information */
    sayHello();

    /* Initialize all commands (save to Command structure tab) */
    initCommands(commands);

    /* Parse file, check correctness and save to Operation structure list */
	operations = parse(commands);

	/* Print all operations if DEBUG */
	printOperationsStructure(operations);

	/* Execute commands */
	current = operations;
	while (current != NULL)
    {
        lastLine = current->lineNum;
        current = current->commandWsk->func(reg, &vars, &varsTab, current, &statusReg);
        nextOperation(reg, vars, varsTab, statusReg, operations, lastLine, ++i);
    }

    /* Print regs, vars and operations */
    regChanged = -1;
    printRegs(reg, statusReg);
    printVars(vars, varsTab);
    printOperations(operations, 0);

    /* Show last info */
    sayGoodbye(i);

    /* Free dynamic variables */
    free(vars);
    free(varsTab);
    free(operations);

	return 0;
}
