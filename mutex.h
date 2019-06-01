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
 * The rules that we are following for mutexes so that we don't run into deadlocks
 * are as follows:
 *
 * - All variables that are written to from ISRs require a mutex companion.
 * - All mutexes must be static within their own modules.
 * - Only ISRs should call mutex_lock and mutex_unlock.
 * - Only non-ISRs should call mutex_wait.
 *
 * If these rules are followed, we will greatly reduce the risk of accessing
 * a variable while it is being changed.
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
#define mutex_wait(mutex) while (mutex) continue

/**
 * Define a mutex type.
 */
typedef volatile bool Mutex;

#endif /* MUTEX_H_ */
