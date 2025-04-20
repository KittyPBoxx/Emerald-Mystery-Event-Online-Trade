	.section	".init"
	.global     _start
	.align
	.arm
@---------------------------------------------------------------------------------
_start:
@---------------------------------------------------------------------------------
	b	rom_header_end

	.fill   156,1,0			@ Nintendo Logo Character Data (8000004h)
	.fill	16,1,0			@ Game Title
	.byte   0x30,0x31		@ Maker Code (80000B0h)
	.byte   0x96			@ Fixed Value (80000B2h)
	.byte   0x00			@ Main Unit Code (80000B3h)
	.byte   0x00			@ Device Type (80000B4h)
	.fill	7,1,0			@ unused
	.byte	0x00			@ Software Version No (80000BCh)
	.byte	0xf0			@ Complement Check (80000BDh)
	.byte	0x00,0x00    		@ Checksum (80000BEh)

@---------------------------------------------------------------------------------
rom_header_end:
@---------------------------------------------------------------------------------
	b	start_vector			@ This branch must be here for proper
						@ positioning of the following header.

	.GLOBAL	__boot_method, __slave_number
@---------------------------------------------------------------------------------
__boot_method:
@---------------------------------------------------------------------------------
	.byte   0				@ boot method (0=ROM boot, 3=Multiplay boot)
@---------------------------------------------------------------------------------
__slave_number:
@---------------------------------------------------------------------------------
	.byte   0				@ slave # (1=slave#1, 2=slave#2, 3=slave#3)

	.byte   0 				@ reserved
	.byte   0 				@ reserved
	.word   0    				@ reserved
	.word   0				@ reserved
	.word   0    				@ reserved
	.word   0    				@ reserved
	.word   0    				@ reserved
	.word   0    				@ reserved

	.fill 8192,1,0 @ 8kb of filler so no useful code gets overwritten when flash bytes get copied over the top.
    .global     start_vector
    .align
@---------------------------------------------------------------------------------
start_vector:
@---------------------------------------------------------------------------------

@---------------------------------------------------------------------------------
@ Enter Thumb mode
@---------------------------------------------------------------------------------
	add	r0, pc, #1
	bx	r0

	.thumb
@ Turn off sound
	ldr r1, =0x4000084
	eor r0, r0, r0
	strh r0, [r1]
	
@---------------------------------------------------------------------------------
@ set heap end
@---------------------------------------------------------------------------------
	ldr	r1, =fake_heap_end
	ldr	r0, =__eheap_end
	str	r0, [r1]
@---------------------------------------------------------------------------------
@ global constructors
@---------------------------------------------------------------------------------
	ldr	r3, =__libc_init_array
	push {lr}
	bl	_blx_r3_stub
@---------------------------------------------------------------------------------
@ Jump to user code
@---------------------------------------------------------------------------------
	mov	r0, #0				@ int argc
	mov	r1, #0				@ char	*argv[]
	ldr	r3, =main
	bl	_blx_r3_stub
@; If we're here, turn the sound back on before we return
	ldr r1, =0x4000084
	mov r0, #0x8F
	strh r0, [r1]
@; Also turn interrupts back on
	ldr r1, =0x4000208
	mov r0, #1
	str r0, [r1]
	pop {pc}
	
@---------------------------------------------------------------------------------
_blx_r3_stub:
@---------------------------------------------------------------------------------
	bx	r3

	.align
	.pool
	.end

