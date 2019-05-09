/*******************************************************************************
 *
 * kernel.c
 *
 * ENEL361 Helicopter Project
 * Friday Morning, Group 7
 *
 * Written by:
 *  - Manu Hamblyn  <mfb31<@uclive.ac.nz>   95140875
 *  - Will Cowper   <wgc22@uclive.ac.nz>    81163265
 *  - Jesse Sheehan <jps111@uclive.ac.nz>   53366509
 *
 * Description:
 * This module contains functions for scheduling tasks.
 *
 ******************************************************************************/

#include <stdlib.h>

#include "kernel.h"
#include "utils.h"

/**
 * The maximum amount of tasks that can be scheduled.
 */
static const uint8_t MAX_TASKS = 16;

/**
 * The total number of tasks stored in the g_tasks array.
 */
static uint8_t g_task_total;

/**
 * The index of the task to be run next.
 */
static uint8_t g_current_task;

/**
 * The array of tasks (to be allocated in kernel_init)
 */
static void** g_tasks;

/**
 * The array of priorities (to be allocated in kernel_init)
 */
static uint8_t* g_priorities;

/**
 * Is true when memory allocation was successful.
 */
static bool g_init_ok = false;

void kernel_init(void)
{
    g_task_total = 0;
    g_current_task = 0;

    // allocate the memory for the arrays
    g_tasks = malloc(sizeof(void*) * MAX_TASKS);
    g_priorities = malloc(sizeof(uint8_t) * MAX_TASKS);

    // set g_init_ok if memory was allocated
    g_init_ok = g_tasks != NULL && g_priorities != NULL;
}

void kernel_add_task(void (*t_task)(void), uint8_t t_priority)
{
    if (g_task_total < MAX_TASKS && g_init_ok)
    {
        g_tasks[g_task_total] = (void*)t_task;
        g_priorities[g_task_total] = t_priority;
        g_task_total++;
    }
}

void kernel_run(void)
{
    if (g_task_total > 0)
    {
        ((void(*)(void))(g_tasks[g_current_task]))();
        if (++g_current_task >= g_task_total)
        {
            g_current_task = 0;
        }
    }
}

bool kernel_ready(void)
{
    return g_init_ok;
}
