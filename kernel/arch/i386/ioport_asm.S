# Read a byte in from an I/O port
# Parameters:
#     uint16_t - port
#
# Returns:
#     uint8_t
#
.global asm_ioport_inb
.type asm_ioport_inb, @function
asm_ioport_inb:
    # Get the port parameter to read from
    movw 4(%esp), %dx

    # Read
    xorl %eax, %eax
    inb  %dx , %al

    ret

# Write a byte to an I/O port
# Parameters:
#     uint16_t - port
#     uint8_t - data
#
# Returns:
#     void
#
.global asm_ioport_outb
.type asm_ioport_outb, @function
asm_ioport_outb:
    # Get port and data parameters
    movw  4(%esp), %dx
    movb  8(%esp), %al

    # Write
    outb %al, %dx

    ret

# Read a word in from an I/O port
# Parameters:
#     uint16_t - port
#
# Returns:
#     uint16_t
#
.global asm_ioport_inw
.type asm_ioport_inw, @function
asm_ioport_inw:
    # Get the port parameter to read from
    movw 4(%esp), %dx

    # Read
    xorl %eax, %eax
    inw  %dx , %ax

    ret

# Write a word to an I/O port
# Parameters:
#     uint16_t - port
#     uint16_t - data
#
# Returns:
#     void
#
.global asm_ioport_outw
.type asm_ioport_outw, @function
asm_ioport_outw:
    # Get port and data parameters
    movw  4(%esp), %dx
    movw  8(%esp), %ax

    # Write
    outw %ax, %dx

    ret

# Read a long in from an I/O port
# Parameters:
#     uint16_t - port
#
# Returns:
#     uint32_t
#
.global asm_ioport_inl
.type asm_ioport_inl, @function
asm_ioport_inl:
    # Get the port parameter to read from
    movw 4(%esp), %dx

    # Read
    #xorl %eax, %eax
    inl  %dx , %eax

    ret

# Write a long to an I/O port
# Parameters:
#     uint16_t - port
#     uint32_t - data
#
# Returns:
#     void
#
.global asm_ioport_outl
.type asm_ioport_outl, @function
asm_ioport_outl:
    # Get port and data parameters
    movw  4(%esp), %dx
    movl  8(%esp), %eax

    # Write
    outl %eax, %dx

    ret
