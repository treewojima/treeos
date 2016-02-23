#include <kernel/proc/scheduler.h>

#include <kernel/const.h>
#include <kernel/interrupt.h>
#include <kernel/panic.h>
#include <kernel/proc/process.h>
#include <kernel/reboot.h>
#include <kernel/vmm/heap.h> // for kheap boundaries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct process *g_current_process;

// Do these need to be volatile?
bool g_preemption_enabled;
bool g_switch_processes = false;

static struct process *process_queue;

// Scheduler mechanism:
//     - interrupt handler is fired on a clock tick and pushes the current
//       thread context onto the stack
//     - tick handler updates process quantum and checks if a reschedule is
//       needed
//     - if process is flagged/due for preemption, call stage1
//     - stage1 copies thread context into its control structure, selects the
//       next process and returns to the interrupt handler
//     - interrupt handler calls stage2 (only if preemption is enabled and
//       we're actively switching to a different process)
//     - stage2 moves the stack pointer to the stack of the "new" process and
//       returns to the interrupt handler
//     - interrupt handler finishes as it normally would, except as if it were
//       the new process that was originally executing

static void scheduler_tick(const struct thread_context *const context);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the scheduling system
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void scheduler_init(void)
{
    process_queue = NULL;
    g_preemption_enabled = true;

    int_register_irq_handler(IRQ_PIT, scheduler_tick);
}

/* Add a process to the scheduling queue
 *
 * Parameters:
 *     process - pointer to process structure
 *
 * Returns:
 *     bool - true if process was added, false if not (already on queue?)
 */
bool scheduler_add_process(struct process *p)
{
    KASSERT(p);
    WORRY_IF(p->next, "process has residual queue information");

    bool lock = int_lock_region();
    bool ret = true;

    if (!process_queue)
    {
        // Queue is empty, so add p as the first (and only) item
        process_queue = p;
    }
    else if (process_queue == p)
    {
        // There's only one process, and it's p
        ret = false;
    }
    else
    {
        // Walk the queue, checking for duplicates along the way
        struct process *node;
        for (node = process_queue;
             node->next != NULL;
             node = node->next)
        {
            if (node->next == p)
            {
                ret = false;

                // A goto is necessary here because of the critical region lock
                goto unlock_and_return;
            }
        }

        // Append the process to the end of the queue
        node->next = p;
    }

unlock_and_return:
    int_unlock_region(lock);
    return ret;
}

/* Find the next best eligible process without updating process
 * control structures
 *
 * Parameters:
 *     none
 *
 * Returns:
 *     struct process * - pointer to found process
 */
struct process *scheduler_find_next_best(void)
{
    bool lock = int_lock_region();

    KASSERT(g_current_process);

    struct process *next = g_current_process->next;
    if (!next)
    {
        // End of the queue, so start over at the beginning
        next = process_queue;
    }

    int_unlock_region(lock);
    return next;
}

/* Update process control structures during the first phase of a context
 * switch
 *
 * Parameters:
 *     context - pointer to thread context on the stack
 *
 * Returns:
 *     void
 */
void scheduler_advance_stage1(const struct thread_context *const context)
{
    // NOTE: Stack grows DOWNWARD, so double-check the kernel stack to make
    //       sure that we're not overwriting anything!

    bool lock = int_lock_region();

    struct process *p = g_current_process;
    p->flags &= ~PROC_DEBUG_YIELD;

    // Check if is running in kernel mode, in which case no ss/esp values
    // were pushed onto the interrupt stack frame
    size_t size = sizeof(struct thread_context);
    if (context->cs == KERN_CODE_SELECTOR)
    {
        size -= sizeof(context->user_esp) + sizeof(context->user_ss);
    }

    // Preserve stack address of thread context
    if (p->flags & PROC_FREE_THREAD_CONTEXT)
    {
        p->flags &= ~PROC_FREE_THREAD_CONTEXT;
        kfree(p->thread->context);
    }
    p->thread->context = (struct thread_context *)context;
    //memcpy(&p->thread->context, context, size);

    // Switch to a new process
    g_current_process = scheduler_find_next_best();

    int_unlock_region(lock);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Called on every clock tick
void scheduler_tick(const struct thread_context *const context UNUSED)
{
    bool lock = int_lock_region();

    struct process *p = g_current_process;
    p->quantum--;

    if (p->flags & PROC_DEBUG_YIELD || p->quantum == 0)
    {
        // Only signal a context switch if there's multiple processes
        if (process_queue->next)
        {
            g_switch_processes = true;
        }
    }

    int_unlock_region(lock);
}
