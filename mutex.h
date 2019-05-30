/*******************************************************************************
 *
 * mutex.h
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
 * Defines macros for locking, unlocking and waiting for mutexes (mutices?).
 * For more information about general mutual exclusion principles see the wiki:
 * https://en.wikipedia.org/wiki/Mutual_exclusion
 *
 ******************************************************************************/

#ifndef MUTEX_H_
#define MUTEX_H_

/**
 * Locks a mutex.
 */
#define mutex_lock(mutex) (mutex = true)

/**
 * Unlocks a mutex.
 */
#define mutex_unlock(mutex) (mutex = false)

/**
 * Waits for a mutex to become unlocked.
 */
#define mutex_wait(mutex) (while (mutex) continue;)

/**
 * Define a mutex type.
 */
typedef volatile static bool Mutex;

#endif /* MUTEX_H_ */
