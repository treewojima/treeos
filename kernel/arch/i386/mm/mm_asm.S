.section .text

# Flushes/updates a page in the TLB
# Parameters:
#     uint32_t - pointer to virtual address
#
# Returns:
#     void
#
.global mm_flush_tlb
.type mm_flush_tlb, @function
mm_flush_tlb:
    pushl %ebp
    movl  %esp, %ebp

    movl   4(%ebp), %eax
    invlpg (%eax)

    movl %ebp, %esp
    popl %ebp
    ret

# Flushes/updates the entire TLB
# Parameters:
#     none
#
# Returns:
#     void
#
.global mm_flush_tlb_full
.type mm_flush_tlb_full, @function
mm_flush_tlb_full:
    #pushl %ebp
    #movl  %esp, %ebp

    movl %cr3, %eax
    movl %eax, %cr3

    #movl %ebp, %esp
    #popl %ebp
    ret
