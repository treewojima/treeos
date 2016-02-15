#include <kernel/proc/scheduler.h>

#include <kernel/interrupt.h>
#include <kernel/panic.h>
#include <kernel/proc/process.h>
#include <kernel/reboot.h>
#include <kernel/vmm/heap.h> // for kheap boundaries
#include <stdio.h>
#include <string.h>

struct process *g_current_process;

static struct process *process_queue;

// NOTE: rename/refactor this function
static void irq0_handler(const struct thread_context *const context);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PUBLIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Initialize the scheduling system
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
void scheduler_init(void)
{
    process_queue = NULL;

    int_register_irq_handler(IRQ_PIT, irq0_handler);
}

/* Add a process to the scheduling queue
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

/* Trigger the scheduler to "advance" to the next process
 * Parameters:
 *     none
 *
 * Returns:
 *     void
 */
struct process *scheduler_advance(void)
{
    bool lock = int_lock_region();

    KASSERT(g_current_process);
    struct process *next_process = g_current_process->next;
    if (!next_process)
    {
        // End of the queue, so start over at the beginning
        g_current_process = process_queue;
    }
    else
    {
        g_current_process = next_process;
    }

    int_unlock_region(lock);
    return g_current_process;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  STATIC FUNCTIONS                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// NOTE: rename/refactor this function
void irq0_handler(const struct thread_context *const context)
{
    bool lock = int_lock_region();

    struct process *p = g_current_process;
    p->quantum--;

    if (p->flags & PROC_DEBUG_YIELD || p->quantum == 0)
    {
        p->flags &= ~PROC_DEBUG_YIELD;

        // NOTE: This code currently does NOT take into consideration that
        //       the processor will NOT push the old esp/ss if there isn't a
        //       privilege change! Since kernel tasks run in kernel mode,
        //       the thread_context structure is filled incorrectly. Normally
        //       this wouldn't be an issue (as long as ss/esp aren't used),
        //       but when switching between contexts, copying one structure
        //       to another WILL corrupt the stack!

        // TODO: Perhaps implement a conditional that checks the pushed cs
        //       register to see if we're coming from user or kernel mode

        // TODO: Rename functions - irq0_handler should be something like
        //       "scheduler_advance_stage1", and scheduler_advance would
        //       become "scheduler_advance_stage2"

        // First, preserve the current process's thread context
        // NOTE: Try replacing this with a more efficient "register copy"
        //       function that takes a pointer to a registers structure,
        //       moves its own stack pointer to where that structure points,
        //       and pushes all the necessary values in the correct order
        memcpy(&p->thread->context, context, sizeof(struct thread_context));

        // Next, advance to the next thread
        p = scheduler_advance();

        // Finally, copy the new thread's context into the location of the
        // old one, so that the interrupt handler will jump to that thread
        // NOTE: cast to void is necessary to get around const-ness
        memcpy((void *)context, &p->thread->context, sizeof(struct thread_context));
    }

    int_unlock_region(lock);
}
