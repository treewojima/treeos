.section .text

# Flushes/updates a page in the TLB
# Parameters:
#     uint32_t - pointer to virtual address
#
# Returns:
#     void
#
.global flush_tlb
.type flush_tlb, @function
flush_tlb:
    movl   4(%esp), %eax
    invlpg (%eax)
    ret

# Flushes/updates the entire TLB
# Parameters:
#     none
#
# Returns:
#     void
#
.global flush_tlb_full
.type flush_tlb_full, @function
flush_tlb_full:
    movl %cr3, %eax
    movl %eax, %cr3
    ret
