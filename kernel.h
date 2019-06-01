/*******************************************************************************
 *
 * kernel.h
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

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdbool.h>
#include <stdint.h>


struct kernel_task_s
{
    /**
     * The friendly name for the task.
     */
    char* name;

    /**
     * A function pointer. The function must return void and accept one parameter
     * of type uint32_t that describes how many microseconds have passed since the
     * task was last run.
     */
    void* function;

    /**
     * The frequency of the task in HZ. This must be less than or equal to the
     * kernel frequency.
     */
    uint16_t frequency;

    /**
     * The priority of the task. By default all tasks will have a priority of 0,
     * which is the highest.
     */
    uint8_t priority;

    /**
     * Used internally to store the amount of ticks that had elapsed last time
     * the task was run.
     * DO NOT MODIFY OUTSIDE THE KERNEL MODULE!!!!
     */
    uint32_t int_count;

    /**
     * The actual period for this task in microseconds.
     */
    uint32_t period_micros;

    /**
     * Used internally to store the latest running time of this task (in microseconds).
     * DO NOT MODIFY OUTSIDE THE KERNEL MODULE!!!
     */
    uint32_t duration_micros;
};

/**
 * Stores information about a schedulable task.
 */
typedef struct kernel_task_s KernelTask;

/**
 * Initialises the kernel.
 */
void kernel_init(uint32_t t_frequency);

/**
 * Adds a task with a priority level (higher is lower priority) to the kernel.
 */
void kernel_add_task(char* t_name, void* t_func_ptr, uint16_t t_frequency, uint8_t t_priority);

/**
 * Runs the next kernel task.
 */
void kernel_run(void);

/**
 * Returns true if memory was allocated ok. Nothing will run if this returns false.
 */
bool kernel_ready(void);

/**
 * Returns the number of times the SysTick interrupt was called.
 */
uint32_t kernel_get_systick_count(void);

/**
 * Returns the frequency the kernel was initialised with.
 */
uint32_t kernel_get_frequency(void);

/**
 * Returns all of the kernel tasks as an array.
 */
KernelTask* kernel_get_tasks(uint8_t* t_size);

#endif /* KERNEL_H_ */
