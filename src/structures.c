/*****************************************************
 * structures.c                                      *
 *                                                   *
 * Structures file.                                  *
 * Implement functions to manage list and            *
 * print elements of structures.                     *
 *                                                   *
 * Author: Tymoteusz Bartnik                         *
 * Copyright: 2019                                   *
 *****************************************************/

#include "structures.h"

/*
 * Init regs.
 *
 * Set that all regs are not pointers.
 */
void initRegs(Reg reg[])
{
    int i;

    regChanged = -1;

    for (i=0; i<15; i++)
        reg[i].isEmpty = true;
}

/*
 * Search var.
 *
 * Search for specific var in vars list.
 */
Var *searchVar(Var *head, char *label)
{
    Var *x = head;

    while (x != NULL && strcmp(label, x->label) != 0)
        x = x->next;

    return x;
}

/*
 * Add var.
 *
 * Add new var to vars list.
 */
Var *addVar(Var *head, char *label, int index, int elements)
{
    Var *x = head;
    Var *newVar = (Var*)malloc(sizeof(Var));

    strcpy(newVar->label, label);
    newVar->index = index;
    newVar->elements = elements;

    if (x != NULL)
    {
        while (x->next != NULL)
            x = x->next;

        x->next = newVar;
        newVar->prev = x;
        newVar->next = NULL;
    } /* List is NOT empty */
    else
    {
        head = newVar;
        head->prev = NULL;
        head->next = NULL;
    } /* List is empty */

    return head;
}

/*
 * Set var.
 *
 * If is new execute addVar function, otherwise change its value.
 */
Var *setVar(Var *head, int **varsTab, char *label, int elements, int value, int shift)
{
    Var *x = head;
    Var *lastVar;
    int i = 0;
    int *newVarsTab;

    if (searchVar(head, label) == NULL)
    {
        lastVar = getLastVar(head);

        if (*varsTab == NULL)
            *varsTab = malloc(elements * sizeof(int));
        else
            *varsTab = realloc(*varsTab, (lastVar->index + lastVar->elements + elements) * sizeof(int));

        if (lastVar != NULL)
        {
            for (i; i<elements; i++)
                (*varsTab)[lastVar->index + lastVar->elements + i] = value;

            x = addVar(head, label, lastVar->index + lastVar->elements, elements);
        }
        else
        {
            for (i; i<elements; i++)
                (*varsTab)[0 + i] = value;

            x = addVar(head, label, 0, elements);
        }

        head = x;
    } /* Add new var */
    else
    {
        if (shift == -1)
        {
            while (x->next != NULL)
            {
                if (strcmp(label, x->label) == 0)
                {
                    (*varsTab)[x->index] = value;
                    break;
                }

                x = x->next;
            }
        }
        else
        {
            while (x->next != NULL)
            {
                if (strcmp(label, x->label) == 0)
                {
                    (*varsTab)[x->index + shift] = value;
                    break;
                }

                x = x->next;
            }
        }

    } /* Change existing var value */

    return head;
}

/*
 * Get last var.
 *
 * Return pointer to last var in the list.
 */
Var *getLastVar(Var *head)
{
    Var *x = head;

    while (x != NULL && x->next != NULL)
        x = x->next;

    return x;
}

/*
 * Found var.
 *
 * Found var by its label.
 */
Var *foundVar(Var *head, char *label)
{
    Var *x = head;

    while (x != NULL && x->next != NULL)
    {
        if (strcmp(x->label, label) == 0)
            return x;

        x = x->next;
    }

    return NULL;
}

/*
 * Add operation.
 *
 * Add new operation to operations list.
 */
Operation  *addOperation(Operation *head, Operation op)
{
    Operation *x = head;
    Operation *newOperation = (Operation*)malloc(sizeof(Operation));

    *newOperation = op;

    if (x != NULL)
    {
        while (x->next != NULL)
            x = x->next;

        x->next = newOperation;
        newOperation->prev = x;
        newOperation->next = NULL;
    } /* List is NOT empty */
    else
    {
        head = newOperation;
        head->prev = NULL;
        head->next = NULL;
    } /* List is empty */

    return head;
}

/*
 * Get last operation.
 *
 * Return pointer to last operation in the list.
 */
Operation *getLastOperation(Operation *head)
{
    Operation *x = head;

    while (x->next != NULL)
        x = x->next;

    return x;
}
