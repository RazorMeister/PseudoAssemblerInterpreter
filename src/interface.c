/*****************************************************
 * interface.c                                       *
 *                                                   *
 * Interface source file.                            *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#include "interface.h"

/*
 * Init interface.
 *
 * Initialize interface. Allow to change colors in console.
 */
void initInterface()
{
    keybd_event(VK_MENU,0x38,0,0);
    keybd_event(VK_RETURN,0x1c,0,0);
    keybd_event(VK_RETURN,0x1c,KEYEVENTF_KEYUP,0);
    keybd_event(VK_MENU,0x38,KEYEVENTF_KEYUP,0);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;

	GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    SetConsoleOutputCP(65001);
}

/*
 * Log error.
 *
 * Write info about occurred error (error number, line number, addition information).
 */
void logError(char *errorInfo, int errorNum, int lineNum)
{
    clearScreen();

    if (lineNum != 0)
        printf("\n" BG_RED_STRING("[ERROR %d] Line: %d | Info: %s") "\n", errorNum, lineNum, errorInfo);
    else
        printf("\n" BG_RED_STRING("[ERROR %d] Info: %s") "\n", errorNum, errorInfo);

    printf("\n\nPress any button to end the program.\n");
    getch();
    exit(errorNum);
}

/*
 * Clear screen.
 *
 * Clear console screen.
 */
void clearScreen()
{
    system("cls");
}

/*
 * Read data.
 *
 * Read data from user (File name, debug mode, production mode).
 */
void readData()
{
    char choice;

    printf("Type " BOLD CYAN_STRING("FILE NAME") "\n" BOLD RED_STRING(" » "));
    scanf("%s", FILE_NAME);

    while (strstr(FILE_NAME, ".txt") == NULL)
    {
        clearScreen();
        printf(BG_RED_STRING("File name must contain .txt") "\n");
        printf("Type " BOLD CYAN_STRING("FILE NAME") "\n" BOLD RED_STRING(" » "));
        scanf("%s", FILE_NAME);
    }

    do
    {
        clearScreen();
        printf("Turn " BOLD CYAN_STRING("DEBUG") " mode on? " BOLD GREEN_STRING("y") " | " BOLD RED_STRING("n") "\n");
        printf("Debug mode shows additional information.\n");
        printf(BOLD RED_STRING(" » "));
        scanf("%c", &choice);
    } while (choice != 'y' && choice != 'n');

    if (choice == 'y')
        DEBUG = true;
    else
        DEBUG = false;

    do
    {
        clearScreen();
        printf("Turn on " BOLD CYAN_STRING("PRODUCTION") " mode on? " BOLD GREEN_STRING("y") " | " BOLD RED_STRING("n") "\n");
        printf("In production mode each operation is executed step by step.\n");
        printf(BOLD RED_STRING(" » "));
        scanf("%c", &choice);
    } while (choice != 'y' && choice != 'n');

    if (choice == 'y')
        PRODUCTION = true;
    else
        PRODUCTION = false;
}

/*
 * Say hello.
 *
 * Show some information about program.
 */
void sayHello()
{
    readData();

    clearScreen();

    printHeader("Pseudo Assembler Interpreter", 0, 0);

    printf("The program is interpreting pseudo assembler code and executing it. \nIt also shows some errors when code is not correct.\n\n");

    if (PRODUCTION)
    {
        printf(BOLD GREEN_STRING("Manual") ": While executing commands you can type \n\t* `" BOLD_STRING("q") "` to exit the program,");
        printf("\n\t* " BOLD_STRING("any other") " to continue.\n\n");
    }

    printf(UNDERLINE BOLD_STRING("FILE NAME ") SHIFT("8") BOLD MAGENTA_STRING("%s") "\n", FILE_NAME);

    if (DEBUG) printf(UNDERLINE BOLD_STRING("DEBUG MODE ") SHIFT("7") BOLD GREEN_STRING("ON") "\n");
    else printf(UNDERLINE BOLD_STRING("DEBUG MODE ") SHIFT("7") BOLD RED_STRING("OFF") "\n");

    if (PRODUCTION) printf(UNDERLINE BOLD_STRING("PRODUCTION MODE ") SHIFT("2") BOLD GREEN_STRING("ON") "\n");
    else printf(UNDERLINE BOLD_STRING("PRODUCTION MODE ") SHIFT("2") BOLD RED_STRING("OFF") "\n");

    printf(BOLD "\nAuthor: \tTymoteusz Bartnik\n" RESET);
    printf(BOLD "Copyright: \t2019\n" RESET);

    printf("\n\nPress any button to continue...\n");

    getch();
    clearScreen();
}

/*
 * Print header.
 *
 * Style header text.
 */
void printHeader(char *text, int startRow, int startCol)
{
    int size = strlen(text) + 2;

    if (startRow == 0 && startCol == 0)
        printf(BOLD CYAN_STRING("┏" SHIFT("%d") "┓\n  %s  \n┗" SHIFT("%d") "┛\n") RESET, size, text, size);
    else
        printf(SAVE_POSITION GO_TO("%d", "%d") BOLD CYAN_STRING("┏" SHIFT("%d") "┓" GO_TO("%d", "%d") "  %s  " GO_TO("%d", "%d") "┗" SHIFT("%d") "┛") RESET RESTORE_POSITION,
               startRow-1, startCol, size, startRow, startCol, text, startRow+1, startCol, size);
}

/*
 * Say goodbye.
 *
 * Finish program with some info.
 */
void sayGoodbye(int opNum)
{
    printHeader("Finish Info", 0, 0);
    printf("\n" BOLD GREEN_STRING("The program ended successfully.\nNumber of performed operations: %d.") "\n", opNum);
    printf("\n\nPress any button to end the program.\n");
    getch();
}

/*
 * Next operation.
 *
 * Go to next operation after button pressed.
 */
void nextOperation(Reg reg[], Var *vars, int *varsTab, int statusReg, Operation *operations, int currentLine, int opNum)
{
    char c;

    if (PRODUCTION)
    {
        printRegs(reg, statusReg);
        printVars(vars, varsTab);
        printOperations(operations, currentLine);
        printCurrentOperation(operations, currentLine, opNum);

        do
        {
            c = getch();

            if (c == 'q')
                exit(0);

        } while (c == 'q');
    }

    clearScreen();
}

/*
 * Print vars.
 *
 * Print all vars from vars list. (if DEBUG)
 */
void printVars(Var *head, int *varsTab)
{
    Var *x = head;
    int i, lineNum = 5, col = SHOW_VARS_START;

    printHeader("All vars with values", 2, col);
    printf(SAVE_POSITION);

    while (x != NULL)
    {
        if (x->elements == 1)
            printf(GO_TO("%d", "%d")  BOLD RED_STRING("%s") " | Address: %d | Elements: %d | Value: ", lineNum++, col, shortenToSize(x->label, 20), x->index * 4, x->elements);
        else
            printf(GO_TO("%d", "%d")  BOLD RED_STRING("%s") " | Address: %d | Elements: %d | Value(s): [ ", lineNum++, col, shortenToSize(x->label, 20), x->index * 4, x->elements);

        for (i=0; i<x->elements; i++)
        {
            if ((i <= 11 && (i+1)%12 == 0) || (i > 11 && (i-11)%24 == 0))
                printf(GO_TO("%d", "%d"), lineNum++, col);

            printf(GREEN_STRING("%d "), varsTab[x->index + i]);
        }

        if (x->elements != 1)  printf("]");

        x = x->next;

        printf("\n");
    }

    printf(RESTORE_POSITION);
}

/*
 * Print operations structure.
 *
 * Print all operations from operations[] tab. (if DEBUG)
 */
void printOperationsStructure(Operation *head)
{
    Operation *x = head;
    int i = 1;

    if (DEBUG)
    {
        printHeader("All operations in structure", 0, 0);

        while (x != NULL)
        {
            printf("Nr: %d ", i++);

            if (x->isLabel)
                printf("| Label: %s ", x->label);
            else
                printf("| Label: --- ");

            if (x->isCommand)
                printf("| Command: %s ", x->command);
            else
                printf("| Command: --- ");

            if (x->areParams)
                printf("| ParamsString: %s | Param1: %s | Param2: %s", x->paramsString, x->param1, x->param2);
            else
                printf("| ParamsString: --- ");

            printf("| LineNum: %d\n", x->lineNum);
            x = x->next;
        }

        printf("\n\nPress any button to continue...\n");

        getch();
        clearScreen();
    }
}

/*
 * Print operations.
 *
 * Print all operations and highlight current operation.
 */
void printOperations(Operation *head, int currentLine)
{
    Operation *x = head;
    int lineNum = 5;

    printHeader("All operations", 2, SHOW_OPERATIONS_START);

    while (x != NULL)
    {
        if (x->lineNum == currentLine) printf(REVERSE);

        printf(GO_TO("%d", "%d") "%d. ", lineNum, SHOW_OPERATIONS_START, x->lineNum);
        if (x->isLabel) printf(GO_TO("%d", "%d") BOLD RED_STRING("%s"), lineNum, SHOW_OPERATIONS_START + 8, shortenToSize(x->label, 20));

        if (x->lineNum == currentLine) printf(REVERSE);
        printf(GO_TO("%d", "%d") BOLD YELLOW_STRING("%s"), lineNum, SHOW_OPERATIONS_START + 28, x->commandWsk->name);

        if (x->lineNum == currentLine) printf(REVERSE);
        printf(GO_TO("%d", "%d") BOLD_STRING("%s"), lineNum, SHOW_OPERATIONS_START + 35, shortenToSize(x->paramsString, 23));

        if (x->lineNum == currentLine)
        {
            printf(RESET);
            printf(GO_TO("%d", "%d") BOLD GREEN_STRING("←"), lineNum, SHOW_REGISTERS_START-3);
        }


        printf("\n");

        lineNum++;

        x = x->next;
    }

    if (lineNum > REGISTERS_ROWS)
        printf(GO_TO("%d", "0"), lineNum + 1);
    else
        printf(GO_TO("%d", "0"), REGISTERS_ROWS + 1);
}

/*
 * Shorten to size.
 *
 * Shorten char to specific size.
 */
char *shortenToSize(char *word, int maxSize)
{
    char *newChar = malloc(MAX_WORD_SIZE);

    if (strlen(word) > maxSize)
    {
        strncpy(newChar, word, maxSize - 6);
        strcat(newChar, "(...)");

        return newChar;
    }
    else
        return word;
}

/*
 * Print regs.
 *
 * Print all registers
 */
void printRegs(Reg reg[], int statusReg)
{
    int i, lineNum = 5, col = SHOW_REGISTERS_START;

    printHeader("All Registers", 2, col);

    printf(SAVE_POSITION);

    for (i=0; i<14; i++)
    {
        if (i == regChanged)
        {
            regChanged = -1;

            if (i <= 9)
                printf(UNDERLINE BOLD GO_TO("%d", "%d") "reg[" BOLD_STRING("%d") UNDERLINE BOLD "]  = ",  lineNum + i, col, i);
            else
                printf(UNDERLINE BOLD GO_TO("%d", "%d") "reg[" BOLD_STRING("%d") UNDERLINE BOLD "] = ",  lineNum + i, col, i);

            printf(UNDERLINE BOLD GREEN_STRING("%d"), reg[i].value);

            printf(RESET);
        }
        else
        {
            if (i <= 9)
                printf(GO_TO("%d", "%d") "reg[" BOLD_STRING("%d") "]  = ",  lineNum + i, col, i);
            else
                printf(GO_TO("%d", "%d") "reg[" BOLD_STRING("%d") "] = ",  lineNum + i, col, i);

            if (reg[i].isEmpty)
                printf("??");
            else
                printf(GREEN_STRING("%d"), reg[i].value);
        }

        printf("\n");
    }

    printf(GO_TO("%d", "%d") "reg[" BOLD_STRING("14") "] = Address of data section", lineNum + (i++), col);
    printf(GO_TO("%d", "%d") "reg[" BOLD_STRING("15") "] = Address of program section", lineNum + (i++), col);
    printf(GO_TO("%d", "%d") BOLD_STRING("Status register") " = " GREEN_STRING("%d"), lineNum + (i++), col, statusReg);

    printf(RESTORE_POSITION);
}

/*
 * Print current operation.
 *
 * Print some info about current executed operation and some additional info.
 */
void printCurrentOperation(Operation *operations, int currentLine, int opNum)
{
    Operation *x = operations;

    while (x->lineNum != currentLine && x != NULL)
        x = x->next;

    printf("[" RED_STRING("»") "] Current executed operation: " BOLD_STRING("%s") "\n", x->fullLine);
    printf("[" RED_STRING("»") "] Operation number: " BOLD_STRING("%d") "\n", opNum);
    printf("[" RED_STRING("»") "] Press: " BOLD_STRING("q") " to exit or " BOLD_STRING("any other key") " to continue\n");
    printf(RED_STRING(" » "));
}
