/* treeos
 * Copyright (C) 2014-2016 Scott Bishop <treewojima@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

.section .text

.global test_usermode_stage2
.type test_usermode_stage2, @function
test_usermode_stage2:

    // Do not use this function
    ret

    movw $0x23, %ax
    movw %ax  , %ds
    movw %ax  , %es
    movw %ax  , %fs
    movw %ax  , %gs

    //movl  %esp, %eax
    pushl $0x23
    //pushl %eax
    //movl $g_usermode_esp, %eax
    pushl (%eax)
    pushfl
    pushl $0x1B
    //pushl $test_usermode_stage3
    iret

.global sys_puts
.type sys_puts, @function
sys_puts:
    pushl %ebp
    movl  %esp, %ebp

    pushl $0xCAFEBABE

    movl $0x1   , %ecx
    movl 8(%ebp), %edx
    int  $0x30

    addl $4, %esp

    movl %ebp, %esp
    popl %ebp
    ret
