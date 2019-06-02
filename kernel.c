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
#include "mutex.h"
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

static Mutex g_systick_count_mutex;

/**
 * The frequency that the kernel runs at.
 */
static uint32_t g_kernel_frequency;

/**
 * Is true when memory allocation was successful.
 */
static bool g_init_ok = false;

/**
 * The SysTick event handler. Simply increments a global static variable.
 */
void kernel_systick_int_handler(void)
{
    mutex_lock(g_systick_count_mutex);
    g_systick_count++;
    mutex_unlock(g_systick_count_mutex);
}

/**
 * Converts kernel ticks into microseconds.
 */
uint32_t kernel_convert_ticks_to_microseconds(uint32_t t_ticks)
{
    return t_ticks * 1000000.0f / g_kernel_frequency;
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
    const KernelTask* task_a = (const KernelTask *)t_task_a;
    const KernelTask* task_b = (const KernelTask *)t_task_b;
    return (int)task_a->priority - (int)task_b->priority;
}

void kernel_add_task(char* t_name, void* t_func_ptr, uint16_t t_frequency, uint8_t t_priority)
{
    // only add tasks if enough memory has been allocated and everything is ok
    if (g_task_total < MAX_TASKS && g_init_ok)
    {
        KernelTask task = (KernelTask){
            t_name,
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
    }
}

void kernel_prioritise(void)
{
    qsort(g_tasks, g_task_total, sizeof(KernelTask), kernel_task_compare);
}

void kernel_run(void)
{
    if (g_task_total > 0)
    {
        mutex_wait(g_systick_count_mutex);
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

                // fix the possible overflow
                count_delta = this_count - task.int_count;
                if (this_count < task.int_count)
                {
                    count_delta = UINT_MAX - task.int_count + this_count;
                }

                // check if the task must be run
                if (task.frequency == 0 || (((float)count_delta / g_kernel_frequency) > (1.0f / task.frequency)))
                {
                    mutex_wait(g_systick_count_mutex);
                    uint32_t start_count = g_systick_count;

                    g_tasks[i].period_micros = kernel_convert_ticks_to_microseconds(count_delta - 1);

                    // execute the task
                    ((void(*)(KernelTask*))(task.function))(&task);

                    mutex_wait(g_systick_count_mutex);
                    uint32_t end_count = g_systick_count;

                    uint32_t duration_count = end_count - start_count;
                    if (end_count < start_count)
                    {
                        duration_count = UINT_MAX - start_count + end_count;
                    }

                    // we keep track of the time taken to perform a task
                    g_tasks[i].duration_micros = kernel_convert_ticks_to_microseconds(duration_count);

                    // update the last time it was run
                    g_tasks[i].int_count = this_count;

                }

            }

            g_last_count = this_count;
        }
    }
}

uint32_t kernel_get_systick_count(void)
{
    mutex_wait(g_systick_count_mutex);
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

KernelTask* kernel_get_tasks(uint8_t* t_size)
{
    *t_size = g_task_total;
    return g_tasks;
}

void kernel_saturation_task(KernelTask* t_task)
{
    utils_wait_for_seconds(1);
}

