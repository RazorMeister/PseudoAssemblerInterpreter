/*****************************************************
 * parsers.c                                         *
 *                                                   *
 * Parser file.                                      *
 * Parse file with pseudo assembler code and          *
 * operations to tab of structures.                  *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#include "parser.h"

/*
 * Parse.
 *
 * Main function to parse file with pseudo assembler code in it.
 */
Operation *parse(Command commands[])
{
    FILE *file;
    char c, currentWord[MAX_WORD_SIZE];
    int currentWordIndex = 0, lineNum = 1, commandStatus = 0; /* commandStatus: 0 - label, 1 - command, 2 - params */
    bool inDataSection = true;
    Operation *operations = NULL;

    if (DEBUG) printHeader("Parsing file", 0, 0);

    file = openFile();

    while ((c = getc(file)) != EOF)
	{
		if (c == ' ' || c == '\t')
		{
		    if (currentWordIndex != 0)
            {
                operations = checkWord(currentWord, currentWordIndex, &commandStatus, lineNum, operations, &inDataSection, commands);
                currentWordIndex = 0;
                commandStatus++;
            }
		}
		else if (c == '\n')
		{
		    if (currentWordIndex != 0 || commandStatus  > 0)
            {
                operations = checkWord(currentWord, currentWordIndex, &commandStatus, lineNum, operations, &inDataSection, commands);
                currentWordIndex = 0;
                commandStatus = 0;

                validateOperation(getLastOperation(operations));
            }

            if (DEBUG)
                printf("\n");

            lineNum++;
		}
		else
		    currentWord[currentWordIndex++] = c;
	}

	if (currentWordIndex != 0)
    {
        operations = checkWord(currentWord, currentWordIndex, &commandStatus, lineNum, operations, &inDataSection, commands);
        validateOperation(getLastOperation(operations));
    }

    fclose(file);

    return operations;
}

/*
 * Open file.
 *
 * Open file with pseudo assembler code.
 */
FILE *openFile()
{
    FILE *file;

	if ((file = fopen(FILE_NAME, "r")) == NULL)
	    logError("Cannot open the file", 1, 0);
}

/*
 * Is command.
 *
 * Check if word is a command.
 * Return 0 - not command | 1 - data section command | 2 - program section command.
 */
int isCommand(char *word, Command commands[])
{
    int i;

    for (i=0; i<COMMANDS_NUM; i++)
    {
        if (strcmp(word, commands[i].name) == 0)
        {
            if (commands[i].dataSection)
                return 1;
            else
                return 2;
        }
    }

    return 0;
}

/*
 * Get command wsk.
 *
 * Return pointer to command by its name.
 */
Command *getCommandWsk(char *word, Command commands[])
{
    int i;

    for (i=0; i<COMMANDS_NUM; i++)
        if (strcmp(word, commands[i].name) == 0)
            return &commands[i];

    return NULL;
}

/*
 * Check label validity.
 *
 * Check if label is valid (not contains special characters).
 */
void checkLabelValidity(char *word, int lineNum)
{
    int i = 0, j = 0;
    const char forbidden[5] = {'*', '(', ')', '/', '\\', '\0'};

    while (word[i++] != '\0')
    {
        j = 0;

        if (i-1 == 0 && isdigit(word[i-1]))
            logError("The name of the label cannot start with a number", 5, lineNum);

        while (forbidden[j++] != '\0')
            if (word[i-1] == forbidden[j-1])
                logError("The label contains forbidden characters", 6, lineNum);
    }
}

/*
 * Check word.
 *
 * Check word (if it's command, add new operation etc.).
 */
Operation *checkWord(char *word, int index, int *status, int lineNum, Operation *operations, bool *inDataSection, Command commands[])
{
    Operation *op;
    bool createNewOperation = false;
    int commandType, i;

    if (DEBUG)
        for (i=0; i<index; i++)
            printf("%c", word[i]);

    word[index] = '\0';

    if (*status == 0)
        createNewOperation = true;

    if (commandType = isCommand(word, commands))
    {
        if (DEBUG)
        {
            printf("\t| Is command");
            if (commandType == 1)
                printf(" (Data section)");
            else
                printf(" (Program section)");
        }

        if (*status == 0)
        {
            *status = 1;
        }
        else if (*status >= 2)
            logError("Command in the wrong place", 2, lineNum);

        if (*inDataSection && commandType == 2)
            *inDataSection = false;
        else if (!*inDataSection && commandType == 1)
            logError("Initialization of the variable in the program section", 4, lineNum);
    }
    else if(*status == 1)
        logError("No command in right place", 3, lineNum);
    else if(*status == 0)
        checkLabelValidity(word, lineNum);

    if (createNewOperation)
    {
        op = malloc(sizeof(Operation));
        op->lineNum = lineNum;
        strcpy(op->fullLine, word);

        if (*status == 0)
        {
            strcpy(op->label, word);
            op->isLabel = true;
            op->isCommand = false;
            op->areParams = false;
            op->paramsNum = 0;
        } /* Operation has label */
        else
        {
            op->isLabel = false;
            strcpy(op->command, word);
            op->isCommand = true;
            op->areParams = false;
            op->paramsNum = 0;
            op->commandWsk = getCommandWsk(word, commands);
        } /* Command */

        operations = addOperation(operations, *op);
        free(op);

        if (DEBUG) printf("\t| CREATED");
    }
    else
    {
        op = getLastOperation(operations);
        strcat(op->fullLine, " ");
        strcat(op->fullLine, word);

        if (*status == 1)
        {
            strcpy(op->command, word);
            op->isCommand = true;
            op->commandWsk = getCommandWsk(word, commands);

        } /* Command */
        else if (*status == 2)
        {
            strcpy(op->paramsString, word);
            op->areParams = true;
        } /* Params */
        else
        {
            strcat(op->paramsString, word);
        } /* More params */
    }

    if (DEBUG) printf("\t|  STATUS: %d\n", *status);

    return operations;
}

/*
 * Is valid register.
 *
 * Check if parameter is a valid register. (0 <= register <= 13)
 */
bool isValidRegister(char *param)
{
    if (param[1] == '\0' && isdigit(param[0]))
        return true;
    else if (param[2] == '\0' && isdigit(param[0]) && isdigit(param[1]) && param[0] - '0' == 1 && param[1] - '0' <= 3 && param[1] - '0' >= 0)
        return true;

    return false;
}

/*
 * Add param.
 *
 * Add parameter to operation structure.
 */
void addParam(Operation *operation, char *word)
{
    operation->paramsNum++;

    if (operation->paramsNum > operation->commandWsk->paramNum)
        logError("Wrong number of parameters", 8, operation->lineNum);

    if (operation->paramsNum == 1)
        strcpy(operation->param1, word);
    else
        strcpy(operation->param2, word);
}

/*
 * Check params.
 *
 * Check if operation with specific command has right number of parameters.
 */
void checkParams(Operation *operation)
{
    if (operation->paramsNum != operation->commandWsk->paramNum)
        logError("Wrong number of parameters", 8, operation->lineNum);
    else if (operation->paramsNum == 1)
        strcpy(operation->param2, "");

    if ((operation->commandWsk->regVarType == 1 && !isValidRegister(operation->param1)) ||
        (operation->commandWsk->regVarType == 2 && (!isValidRegister(operation->param1) || !isValidRegister(operation->param2))))
        logError("Wrong register", 9, operation->lineNum);
}

/*
 * Validate operation.
 *
 * Validate operation by checking its parameters.
 */
void validateOperation(Operation *operation)
{
    char c, word[MAX_WORD_SIZE];
    int i = 0, wordIndex = 0;

    if (!operation->areParams)
    {
        if (operation->commandWsk->paramNum > 0)
            logError("No parameters", 7, operation->lineNum);
    }
    else
    {
        /* Convert params to separated vars */
        while ((c = operation->paramsString[i++]) != '\0')
        {
            if (c == ',')
            {
                if (wordIndex > 0)
                {
                    word[wordIndex] = '\0';
                    addParam(operation, word);
                    wordIndex = 0;
                }
                else
                   addParam(operation, " ");
            }
            else
                word[wordIndex++] = c;
        }

        if (wordIndex > 0)
        {
            word[wordIndex] = '\0';
            addParam(operation, word);
            wordIndex = 0;
        }
        else
            addParam(operation, " ");

        checkParams(operation);
    }
}
