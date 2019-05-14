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

#include <stdint.h>
#include <stdlib.h>

#include "kernel.h"
#include "utils.h"
#include "altitude.h" // for the alt_getIntCount and ALT_SAMPLE_RATE_HZ

/**
 * The maximum amount of tasks that can be scheduled.
 */
static const uint8_t MAX_TASKS = 16;

/**
 * The total number of tasks stored in the g_tasks array.
 */
static uint8_t g_task_total;

/**
 * The array of tasks (to be allocated in kernel_init)
 */
static KernelTask* g_tasks;

/**
 * Stores the number of times the SysTickIntHandler has been called.
 */
static uint32_t g_last_count;

/**
 * Is true when memory allocation was successful.
 */
static bool g_init_ok = false;

void kernel_init(void)
{
    g_task_total = 0;
    g_last_count = 0;

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
        uint32_t this_count = alt_getIntCount();
        if (g_last_count != this_count)
        {
            // iterate through each task and see if it needs to be
            // run.
            uint8_t i;
            for (i = 0; i < g_task_total; i++)
            {
                KernelTask task = g_tasks[i];
                uint32_t count_delta = this_count - task.int_count;

                if (task.frequency == 0 || (((float)count_delta / ALT_SAMPLE_RATE_HZ) > (1.0f / task.frequency)))
                {
                    ((void(*)(void))(task.function))();
                    g_tasks[i].int_count = this_count;
                }
            }

            g_last_count = this_count;
        }
    }
}

bool kernel_ready(void)
{
    return g_init_ok;
}
