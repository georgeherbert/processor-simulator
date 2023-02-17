	.file	"add.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	_start
	.type	_start, @function
_start:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	addi	s0,sp,16
	li	a0,3
	call	factorial
	mv	a5,a0
	mv	a0,a5
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	_start, .-_start
	.align	2
	.globl	factorial
	.type	factorial, @function
factorial:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
	mv	a5,a0
	sb	a5,-17(s0)
	lbu	a5,-17(s0)
	bne	a5,zero,.L4
	li	a5,1
	j	.L5
.L4:
	lbu	a5,-17(s0)
	addi	a5,a5,-1
	andi	a5,a5,0xff
	mv	a0,a5
	call	factorial
	mv	a5,a0
	mv	a4,a5
	lbu	a5,-17(s0)
	mv	a1,a4
	mv	a0,a5
	call	multiply
	mv	a5,a0
.L5:
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	factorial, .-factorial
	.align	2
	.globl	multiply
	.type	multiply, @function
multiply:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	mv	a5,a0
	mv	a4,a1
	sb	a5,-33(s0)
	mv	a5,a4
	sb	a5,-34(s0)
	sw	zero,-20(s0)
	sw	zero,-24(s0)
	j	.L7
.L8:
	lbu	a5,-33(s0)
	lw	a4,-20(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L7:
	lbu	a5,-34(s0)
	lw	a4,-24(s0)
	blt	a4,a5,.L8
	lw	a5,-20(s0)
	andi	a5,a5,0xff
	mv	a0,a5
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	multiply, .-multiply
	.ident	"GCC: (g2ee5e430018-dirty) 12.2.0"
