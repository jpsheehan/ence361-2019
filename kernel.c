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
static KernelTask* g_tasks;

/**
 * Stores the number of times kernel_run was called.
 */
static uint32_t g_run_count;

/**
 * Is true when memory allocation was successful.
 */
static bool g_init_ok = false;

void kernel_init(void)
{
    g_task_total = 0;
    g_current_task = 0;
    g_run_count = 0;

    // allocate the memory for the arrays
    g_tasks = malloc(sizeof(KernelTask) * MAX_TASKS);

    // set g_init_ok if memory was allocated
    g_init_ok = g_tasks != NULL;
}

void kernel_add_task(KernelTask t_task)
{
    if (g_task_total < MAX_TASKS && g_init_ok)
    {
        g_tasks[g_task_total] = t_task;
        g_task_total++;
    }
}

void kernel_run(void)
{
    if (g_task_total > 0)
    {
        KernelTask task = g_tasks[g_current_task];

        ((void(*)(void))(task.function))();
        if (++g_current_task >= g_task_total)
        {
            g_current_task = 0;
        }

        // increment the run count
        g_run_count++;
    }
}

bool kernel_ready(void)
{
    return g_init_ok;
}
