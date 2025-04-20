	.text
	.code 16

	.global	call_into_middle_of_titlescreen_func
	.thumb_func
call_into_middle_of_titlescreen_func:
	push {lr}
	push {r4-r5}
	@ use r4 (which already got saved to the stack) as scratch space to reserve a variable amount of stack space
	mov r4,sp
	sub r4,r4,r1
	mov sp,r4
	bx r0