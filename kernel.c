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
#include <stdbool.h>
#include <limits.h>

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

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
 * The array of tasks (to be allocated in kernel_init)
 */
static KernelTask* g_tasks;

/**
 * Stores the number of times the SysTickIntHandler has been called when
 * kernel_run was last called.
 */
static uint32_t g_last_count;

/**
 * Stores the number of times the SysTickIntHandler has been called.
 */
static volatile uint32_t g_systick_count;

/**
 * The frequency that the kernel runs at.
 */
static uint32_t g_kernel_frequency;

/**
 * Is true when memory allocation was successful.
 */
static bool g_init_ok = false;

/**
 * True when all the tasks are sorted in order of priority.
 */
static bool g_tasks_are_sorted = true;

/**
 * The SysTick event handler. Simply increments a global static variable.
 */
void kernel_systick_int_handler(void)
{
    g_systick_count++;
}

/**
 * (Original code by P.J. Bones)
 * Intialises the system tick interrupt handler.
 */
void kernel_init_systick(void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / g_kernel_frequency);
    //
    // Register the interrupt handler
    SysTickIntRegister(kernel_systick_int_handler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void kernel_init(uint32_t t_frequency)
{
    g_task_total = 0;
    g_systick_count = 0;
    g_last_count = 0;
    g_kernel_frequency = t_frequency;

    // allocate the memory for the arrays
    g_tasks = malloc(sizeof(KernelTask) * MAX_TASKS);

    // set g_init_ok if memory was allocated
    g_init_ok = g_tasks != NULL;

    kernel_init_systick();
}

/**
 * The compare function to be plugged into stdlib's qsort.
 */
int kernel_task_compare(const void * t_task_a, const void * t_task_b)
{
    return ((KernelTask*)t_task_a)->priority - ((KernelTask*)t_task_b)->priority;
}

void kernel_add_task(void* t_func_ptr, uint16_t t_frequency, uint8_t t_priority)
{
    // only add tasks if enough memory has been allocated and everything is ok
    if (g_task_total < MAX_TASKS && g_init_ok)
    {
        KernelTask task = (KernelTask){
            t_func_ptr,
            t_frequency,
            t_priority,
            0,
            0
        };

        // if the frequency is too high for the kernel
        // to support, make the task run every time.
        if (task.frequency > g_kernel_frequency)
        {
            task.frequency = 0;
        }

        g_tasks[g_task_total] = task;
        g_task_total++;

        g_tasks_are_sorted = false;
    }
}

void kernel_run(void)
{
    if (g_task_total > 0)
    {
        // sort the tasks in place if we need to, this ensures that the higher priority tasks
        // are run first
        if (!g_tasks_are_sorted) {
            qsort(g_tasks, g_task_total, sizeof(KernelTask), kernel_task_compare);
            g_tasks_are_sorted = true;
        }

        uint32_t this_count = g_systick_count;
        if (g_last_count != this_count)
        {
            // iterate through each task and see if it needs to be
            // run.
            uint8_t i;
            for (i = 0; i < g_task_total; i++)
            {
                KernelTask task = g_tasks[i];
                uint32_t count_delta;

                // NOTE: There is a case when this_count overflows and becomes less than task.int_count
                // which causes the count_delta to be off by around 1%. TODO: Fix in future, not a
                // huge problem right now.
                count_delta = this_count - task.int_count;

                if (task.frequency == 0 || (((float)count_delta / g_kernel_frequency) > (1.0f / task.frequency)))
                {
                    uint32_t start_count = g_systick_count;
                    uint32_t elapsed_micros = ((count_delta - 1) * 1000000) / g_kernel_frequency;
                    ((void(*)(uint32_t, KernelTask*))(task.function))(elapsed_micros, &task);
                    g_tasks[i].int_count = this_count;

                    // we can also keep track of the time taken to perform a task
                    uint32_t end_count = g_systick_count;
                    uint32_t task_ticks;
                    if (end_count < start_count)
                    {
                        task_ticks = UINT_MAX - start_count + end_count;
                    }
                    else
                    {
                        task_ticks = end_count - start_count;
                    }
                    task.duration_micros = ((task_ticks - 1) * 1000000) / g_kernel_frequency;
                }

            }

            g_last_count = this_count;
        }
    }
}

uint32_t kernel_get_systick_count(void)
{
    return g_systick_count;
}

uint32_t kernel_get_frequency(void)
{
    return g_kernel_frequency;
}

bool kernel_ready(void)
{
    return g_init_ok;
}
