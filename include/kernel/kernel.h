/**
 * @file kernel.h
 * @brief Kernel initialization and core functions
 * @ingroup kernel_core
 *
 * Main kernel initialization and startup for the UniQOS operating system.
 * Handles FreeRTOS setup and task creation.
 */

#ifndef KERNEL_H
#define KERNEL_H

/**
 * @ingroup kernel_core
 * @brief Initialize the kernel and start the scheduler
 *
 * Initializes all system tasks, drivers, and starts the FreeRTOS scheduler.
 * This function does not return if successful.
 */
void kernel_init(void);

#endif /* KERNEL_H */