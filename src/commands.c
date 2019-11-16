/*****************************************************
 * commands.c                                        *
 *                                                   *
 * Commands file.                                    *
 * Add all commands to structure,                    *
 * implements functions to commands with             *
 * some helpers.                                     *
 *                                                   *
 * All commands: (count: 20)                         *
 *  DS                                               *
 *  DC                                               *
 *  A                                                *
 *  AR                                               *
 *  S                                                *
 *  SR                                               *
 *  M                                                *
 *  MR                                               *
 *  D                                                *
 *  DR                                               *
 *  C                                                *
 *  CR                                               *
 *  J                                                *
 *  JZ                                               *
 *  JP                                               *
 *  JN                                               *
 *  L                                                *
 *  LA                                               *
 *  LR                                               *
 *  ST                                               *
 *  WR                                               *
 *  READ                                             *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#include "commands.h"

/*
 * To int.
 *
 * Convert char to int.
 */
int toInt(char *word)
{
    return atoi(word);
}

/*
 * Check var existence.
 *
 * Check if specific variable (by its name) exists.
 */
void checkVarExistence(Var *vars, char *label, int lineNum)
{
    if (searchVar(vars, label) == NULL)
        logError("The provided variable is not initialized", 10, lineNum);
}

/*
 * Get var value.
 *
 * Get value var, check if it is table.
 */
int *getVar(Var *vars, int *varsTab, Reg reg[], char *label, int lineNum)
{
    if (strchr(label, '('))
    {
        if (isdigit(label[0]))
        {
            char c, shift[MAX_WORD_SIZE], pointer[MAX_WORD_SIZE];
            int i = 0, index = 0, numShift = 0, numPointer = 0;
            bool inPointer = false, isValid = false;
            unsigned shiftInt, pointerInt;

            while ((c = label[i++]) != '\0')
            {
                if (isdigit(c) && !inPointer)
                {
                    shift[numShift++] = c;
                }
                else if(c == '(')
                {
                    shift[numShift] = '\0';
                    inPointer = true;
                }
                else if (c == ')')
                {
                    isValid = true;
                    pointer[numPointer] = '\0';
                    break;
                }
                else if(inPointer)
                    pointer[numPointer++] = c;
            }

            if (!isValid)
                logError("Wrong name of variable", 11, lineNum);
            else if(numPointer == 0 && numShift == 0)
                logError("Incorrect syntax", 12, lineNum);
            else
            {

                shiftInt = toInt(shift);
                pointerInt = toInt(pointer);

                if (shiftInt % 4 != 0)
                    logError("Invalid value of shift", 13, lineNum);
                else if (pointerInt < 0 || pointerInt > 14)
                    logError("Wrong register", 9, lineNum);
                else if (pointerInt != 14 && reg[pointerInt].value % 4 != 0)
                    logError("Invalid value of address", 14, lineNum);

                shiftInt /= 4;
            }

            if (pointerInt == 14)
                return &varsTab[shiftInt];
            else
                return &(varsTab[reg[pointerInt].value/4 + shiftInt]);
        } // shift as number
        else
        {
            char c, shift[MAX_WORD_SIZE], pointer[MAX_WORD_SIZE];
            int i = 0, index = 0, numShift = 0, numPointer = 0;
            bool inPointer = false, isValid = false;
            unsigned shiftInt, pointerInt;
            Var *shiftVar;

            while ((c = label[i++]) != '\0')
            {
                if (!inPointer && c != '(')
                {
                    shift[numShift++] = c;
                }
                else if(c == '(')
                {
                    shift[numShift] = '\0';
                    inPointer = true;
                }
                else if (c == ')')
                {
                    isValid = true;
                    pointer[numPointer] = '\0';
                    break;
                }
                else if(inPointer)
                    pointer[numPointer++] = c;
            }

            if (!isValid)
                logError("Wrong name of variable", 11, lineNum);
            else if(numPointer == 0 && numShift == 0)
                logError("Incorrect syntax", 12, lineNum);
            else
            {
                checkVarExistence(vars, shift, lineNum);

                shiftVar = searchVar(vars, shift);

                shiftInt = shiftVar->index;
                pointerInt = toInt(pointer);

                if (pointerInt < 0 || pointerInt > 14)
                    logError("Wrong register", 9, lineNum);
                else if (pointerInt != 14 && reg[pointerInt].value % 4 != 0)
                    logError("Invalid value of address", 14, lineNum);
            }

            if (pointerInt == 14)
                return &varsTab[shiftInt];
            else
                return &varsTab[reg[pointerInt].value/4 + shiftInt];
        }// shift as var
    }   // address
    else
    {
        if (isdigit(label[0]))
        {
            int addr = toInt(label);

            if (addr % 4 != 0)
                logError("Invalid value of address", 14, lineNum);

            addr /= 4;
            return &varsTab[addr];
        } // direct address
        else
        {
            checkVarExistence(vars, label, lineNum);
            return &varsTab[searchVar(vars, label)->index];
        } // var with label
    }   // single label
}

/*
 * Operate on reg.
 *
 * Make arithmetic operation on reg.
 */
int operateOnReg(Reg reg[], int regNum, char type, int value, int stateValue, int lineNum)
{
    int regValue;

    switch (type)
    {
        case '+':
            regValue = reg[regNum].value + value;
            break;
        case '-':
            regValue = reg[regNum].value - value;
            break;
        case '*':
            regValue = reg[regNum].value * value;
            break;
        case '/':
            if (value == 0) return 3; /* Division by 0 */
            regValue = reg[regNum].value / value;
            break;
    }

    reg[regNum].value = regValue;
    reg[regNum].isEmpty = false;
    regChanged = regNum;

    return getState(regValue);
}

/*
 * Get state.
 *
 * Return state (0, 1, 2) number by output.
 */
int getState(int output)
{
    if (output > 0)
        return 1;
    else if (output == 0)
        return 0;
    else
        return 2;
}

/*
 * Find operation.
 *
 * Find operation with specific label.
 */
Operation *findOperation(Operation *currentOp, char *label)
{
    Operation *op = currentOp;

    while(op != NULL)
    {
        if (!op->commandWsk->dataSection && op->isLabel && strcmp(op->label, label) == 0)
            return op;

        op = op->prev;
    }

    op = currentOp->next;
    while(op != NULL)
    {
        if (!op->commandWsk->dataSection && op->isLabel &&  strcmp(op->label, label) == 0)
            return op;

        op = op->next;
    }

    logError("Bad label on jump", 15, currentOp->lineNum);
}

/*
 * DS command executive function.
 *
 * Define variable without initial value.
 */
Operation *execDS(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    int i = 0, typeIndex = 0, numElements = 0;
    char c, type[MAX_WORD_SIZE], elements[MAX_WORD_SIZE];
    bool inElements = false;
    unsigned int elementsNum = 1;

    while ((c = op->param1[i++]) != '\0')
    {
        if (isdigit(c))
        {
            inElements = true;
            elements[numElements++] = c;
        }
        else if (c == '*')
        {
            if (!inElements)
                logError("No number of items", 16, op->lineNum);
            else
            {
                elements[numElements] = '\0';
                elementsNum = toInt(elements);

                if (elementsNum == 0)
                    logError("The number of items cannot be zero", 17, op->lineNum);

                inElements = false;
            }
        }
        else if (inElements)
            logError("Invalid variable type", 18, op->lineNum);
        else if (c != ' ' && c != '\t')
            type[typeIndex++] = c;
    }

    type[typeIndex] = '\0';

    if (strcmp(type, "INTEGER") != 0)
        logError("Invalid variable type", 18, op->lineNum);
    else
        *vars = setVar(*vars, varsTab, op->label, elementsNum, NULL, -1);

    return op->next;
}

/*
 * DC command executive function.
 *
 * Define variable with initial value.
 */
Operation *execDC(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    int i = 0, numIndex = 0, numInt, typeIndex = 0, numElements = 0;
    char c, num[MAX_WORD_SIZE], *numPointer, type[MAX_WORD_SIZE], elements[MAX_WORD_SIZE];
    bool inElements = false, inNum = false, validType = false;
    unsigned int elementsNum = 1;

    while ((c = op->param1[i++]) != '\0')
    {
        if (isdigit(c) && !inNum)
        {
            inElements = true;
            elements[numElements++] = c;
        }
        else if (c == '*')
        {
            if (!inElements)
                logError("No number of items", 16, op->lineNum);
            else
            {
                elements[numElements] = '\0';
                elementsNum = toInt(elements);

                if (elementsNum == 0)
                    logError("The number of items cannot be zero", 17, op->lineNum);

                inElements = false;
            }
        }
        else if (inElements)
            logError("Invalid variable type", 18, op->lineNum);
        else if (c == '(')
        {
            type[typeIndex] = '\0';
            inNum = true;
        }
        else if (c == ')')
        {
            if (strcmp(type, "INTEGER") == 0)
                validType = true;

            inNum = false;

        }
        else if (inNum)
            num[numIndex++] = c;
        else if (c != ' ' && c != '\t')
            type[typeIndex++] = c;
    }



    if (!validType)
        logError("Invalid variable type", 18, op->lineNum);
    else if (numIndex == 0)
        logError("The variable value is missing", 19, op->lineNum);
    else
    {
        num[numIndex++] = '\0';
        numPointer = malloc(numIndex * sizeof(char));
        strcpy(numPointer, num);
        numInt = atoi(numPointer);

        *vars = setVar(*vars, varsTab, op->label, elementsNum, numInt, -1);
    }

    return op->next;
}

/*
 * A command executive function.
 *
 * Add register (param1) to var (param) and save to register.
 */
Operation *execA(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = operateOnReg(reg, toInt(op->param1), '+', *getVar(*vars, *varsTab, reg, op->param2, op->lineNum), *statusReg, op->lineNum);

    return op->next;
}

/*
 * AR command executive function.
 *
 * Add register1 (param1) to register2 (param2) and save to register1.
 */
Operation *execAR(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = operateOnReg(reg, toInt(op->param1), '+', reg[toInt(op->param2)].value, *statusReg, op->lineNum);

    return op->next;
}

/*
 * S command executive function.
 *
 * Subtract var (param2) from register (param1) and save to register.
 */
Operation *execS(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = operateOnReg(reg, toInt(op->param1), '-', *getVar(*vars, *varsTab, reg, op->param2, op->lineNum), *statusReg, op->lineNum);

    return op->next;
}

/*
 * SR command executive function.
 *
 * Subtract register2 (param2) from register1 (param1) and save to register1.
 */
Operation *execSR(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = operateOnReg(reg, toInt(op->param1), '-', reg[toInt(op->param2)].value, *statusReg, op->lineNum);

    return op->next;
}

/*
 * M command executive function.
 *
 * Multiply register (param1) by var (param2) and save to register.
 */
Operation *execM(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = operateOnReg(reg, toInt(op->param1), '*', *getVar(*vars, *varsTab, reg, op->param2, op->lineNum), *statusReg, op->lineNum);

    return op->next;
}

/*
 * MR command executive function.
 *
 * Multiply register1 (param1) by register2 (param2) and save to register1.
 */
Operation *execMR(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = operateOnReg(reg, toInt(op->param1), '*', reg[toInt(op->param2)].value, *statusReg, op->lineNum);

    return op->next;
}

/*
 * D command executive function.
 *
 * Divide register (param1) by var (param2) and save to register.
 */
Operation *execD(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = operateOnReg(reg, toInt(op->param1), '/', *getVar(*vars, *varsTab, reg, op->param2, op->lineNum), *statusReg, op->lineNum);

    return op->next;
}

/*
 * DR command executive function.
 *
 * Divide register1 (param1) by register2 (param2) and save to register1.
 */
Operation *execDR(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = operateOnReg(reg, toInt(op->param1), '/', reg[toInt(op->param2)].value, *statusReg, op->lineNum);

    return op->next;
}

/*
 * C command executive function.
 *
 * Compare register (param1) with var (param2).
 */
Operation *execC(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = getState(reg[toInt(op->param1)].value - *getVar(*vars, *varsTab, reg, op->param2, op->lineNum));

    return op->next;
}

/*
 * CR command executive function.
 *
 * Compare register1 (param1) with register2 (param2).
 */
Operation *execCR(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    *statusReg = getState(reg[toInt(op->param1)].value - reg[toInt(op->param2)].value);

    return op->next;
}

/*
 * JN command executive function.
 *
 * Jump to specific operation by label (param1).
 */
Operation *execJ(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    Operation *toReturn = findOperation(op, op->param1);

    return toReturn;
}

/*
 * JZ command executive function.
 *
 * Jump to specific operation by label (param1) if statusReg = 0.
 */
Operation *execJZ(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    Operation *toReturn = findOperation(op, op->param1);

    if (*statusReg == 0)
        return toReturn;
    else
        return op->next;
}

/*
 * JP command executive function.
 *
 * Jump to specific operation by label (param1) if statusReg = 1.
 */
Operation *execJP(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    Operation *toReturn = findOperation(op, op->param1);

    if (*statusReg == 1)
        return toReturn;
    else
        return op->next;
}

/*
 * JN command executive function.
 *
 * Jump to specific operation by label (param1) if statusReg = 2.
 */
Operation *execJN(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    Operation *toReturn = findOperation(op, op->param1);

    if (*statusReg == 2)
        return toReturn;
    else
        return op->next;
}

/*
 * L command executive function.
 *
 * Save var value (param2) to register (param1).
 */
Operation *execL(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    reg[toInt(op->param1)].value = *getVar(*vars, *varsTab, reg, op->param2, op->lineNum);
    reg[toInt(op->param1)].isEmpty = false;
    regChanged = toInt(op->param1);

    return op->next;
}

/*
 * LA command executive function.
 *
 * Save register2 value (param2) to register1 (param1). NOT FINISHED
 */
Operation *execLA(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    checkVarExistence(*vars, op->param2, op->lineNum);

    reg[toInt(op->param1)].value = searchVar(*vars, op->param2)->index*4;
    reg[toInt(op->param1)].isEmpty = false;
    regChanged = toInt(op->param1);

    return op->next;
}

/*
 * LR command executive function.
 *
 * Save register2 value (param2) to register1 (param1).
 */
Operation *execLR(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    reg[toInt(op->param1)].value = reg[toInt(op->param2)].value;
    reg[toInt(op->param1)].isEmpty = false;
    regChanged = toInt(op->param1);

    return op->next;
}

/*
 * ST command executive function.
 *
 * Save register value (param1) to var by label (param2).
 */
Operation *execST(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    int *varInt;

    varInt = getVar(*vars, *varsTab, reg, op->param2, op->lineNum);

     printf("VAR: %d | %d -> ", *varInt, (*varsTab)[52]);

    *varInt = reg[toInt(op->param1)].value;

    printf("VAR: %d | %d\n", *varInt, (*varsTab)[52]);

    return op->next;
}

/*
 * READ command executive function.
 *
 * Read from user value(s) and save to var (param1).
 */
Operation *execREAD(Reg reg[], Var **vars, int **varsTab, Operation *op, int *statusReg)
{
    Var *pointer;
    int i, value;

    checkVarExistence(*vars, op->param1, op->lineNum);

    pointer = foundVar(*vars, op->param1);

    printHeader("Enter variable", 0, 0);
    printf(BOLD MAGENTA_STRING("%s\n"), op->param1);

    if (pointer->elements == 1)
    {
        printf("Enter value | » ");
        scanf("%d", &value);
        setVar(*vars, varsTab, op->param1, 1, value, -1);
    }
    else
    {
        for (i = 0; i < pointer->elements; i++)
        {
            printf("Enter value for element nr: %d | » ", i+1);
            scanf("%d", &value);
            setVar(*vars, varsTab, op->param1, 1, value, i);
        }
    }

    clearScreen();

    return op->next;
}

/*
 * Initialize all commands.
 *
 * Add to tab of Command structures all commands.
 */
void initCommands(Command commands[])
{
    int index = 0;

    Command newCommand;

    /* DS command */
    newCommand.name = "DS";
    newCommand.paramNum = 1;
    newCommand.dataSection = true;
    newCommand.regVarType = 0;
    newCommand.func = execDS;
    commands[index++] = newCommand;

    /* DC command */
    newCommand.name = "DC";
    newCommand.paramNum = 1;
    newCommand.dataSection = true;
    newCommand.regVarType = 0;
    newCommand.func = execDC;
    commands[index++] = newCommand;

    /* A command */
    newCommand.name = "A";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 1;
    newCommand.func = execA;
    commands[index++] = newCommand;

    /* AR command */
    newCommand.name = "AR";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 2;
    newCommand.func = execAR;
    commands[index++] = newCommand;

    /* S command */
    newCommand.name = "S";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 1;
    newCommand.func = execS;
    commands[index++] = newCommand;

    /* SR command */
    newCommand.name = "SR";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 2;
    newCommand.func = execSR;
    commands[index++] = newCommand;

    /* M command */
    newCommand.name = "M";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 1;
    newCommand.func = execM;
    commands[index++] = newCommand;

    /* MR command */
    newCommand.name = "MR";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 2;
    newCommand.func = execMR;
    commands[index++] = newCommand;

    /* D command */
    newCommand.name = "D";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 1;
    newCommand.func = execD;
    commands[index++] = newCommand;

    /* DR command */
    newCommand.name = "DR";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 2;
    newCommand.func = execDR;
    commands[index++] = newCommand;

    /* C command */
    newCommand.name = "C";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 1;
    newCommand.func = execC;
    commands[index++] = newCommand;

    /* CR command */
    newCommand.name = "CR";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 2;
    newCommand.func = execCR;
    commands[index++] = newCommand;

    /* J command */
    newCommand.name = "J";
    newCommand.paramNum = 1;
    newCommand.dataSection = false;
    newCommand.regVarType = 0;
    newCommand.func = execJ;
    commands[index++] = newCommand;

    /* JZ command */
    newCommand.name = "JZ";
    newCommand.paramNum = 1;
    newCommand.dataSection = false;
    newCommand.regVarType = 0;
    newCommand.func = execJZ;
    commands[index++] = newCommand;

    /* JP command */
    newCommand.name = "JP";
    newCommand.paramNum = 1;
    newCommand.dataSection = false;
    newCommand.regVarType = 0;
    newCommand.func = execJP;
    commands[index++] = newCommand;

    /* JN command */
    newCommand.name = "JN";
    newCommand.paramNum = 1;
    newCommand.dataSection = false;
    newCommand.regVarType = 0;
    newCommand.func = execJN;
    commands[index++] = newCommand;

    /* L command */
    newCommand.name = "L";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 1;
    newCommand.func = execL;
    commands[index++] = newCommand;

    /* LA command */
    newCommand.name = "LA";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 1;
    newCommand.func = execLA;
    commands[index++] = newCommand;

    /* LR command */
    newCommand.name = "LR";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 2;
    newCommand.func = execLR;
    commands[index++] = newCommand;

    /* ST command */
    newCommand.name = "ST";
    newCommand.paramNum = 2;
    newCommand.dataSection = false;
    newCommand.regVarType = 1;
    newCommand.func = execST;
    commands[index++] = newCommand;

    /* READ command */
    newCommand.name = "READ";
    newCommand.paramNum = 1;
    newCommand.dataSection = false;
    newCommand.regVarType = 0;
    newCommand.func = execREAD;
    commands[index++] = newCommand;
}
