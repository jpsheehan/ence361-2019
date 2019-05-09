/*
 * kernel.c
 *
 *  Created on: 10/05/2019
 *      Author: jps111
 */

#include <stdlib.h>

#include "kernel.h"
#include "utils.h"

static const uint8_t MAX_TASKS = 8;

static uint8_t g_task_total;

static uint8_t g_current_task;

static void** g_tasks;

void kernel_init(void)
{
    g_task_total = 0;
    g_current_task = 0;
    g_tasks = malloc(sizeof(void*) * MAX_TASKS);
}

void kernel_add_task(void (*t_task)(void))
{
    if (g_task_total < MAX_TASKS && g_tasks != NULL)
    {
        g_tasks[g_task_total] = (void*)t_task;
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
