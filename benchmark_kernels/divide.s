	.file	"divide.c"
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
	li	a5,99999744
	addi	a1,a5,256
	li	a0,-1
	call	divide
	mv	a5,a0
	mv	a0,a5
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	_start, .-_start
	.align	2
	.globl	divide
	.type	divide, @function
divide:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	lw	a5,-40(s0)
	bne	a5,zero,.L4
	li	a5,0
	j	.L5
.L4:
	sw	zero,-20(s0)
	sw	zero,-24(s0)
	li	a5,31
	sw	a5,-28(s0)
	j	.L6
.L8:
	lw	a5,-24(s0)
	slli	a5,a5,1
	sw	a5,-24(s0)
	lw	a5,-28(s0)
	lw	a4,-36(s0)
	srl	a5,a4,a5
	andi	a5,a5,1
	lw	a4,-24(s0)
	or	a5,a4,a5
	sw	a5,-24(s0)
	lw	a4,-24(s0)
	lw	a5,-40(s0)
	bltu	a4,a5,.L7
	lw	a4,-24(s0)
	lw	a5,-40(s0)
	sub	a5,a4,a5
	sw	a5,-24(s0)
	lw	a5,-28(s0)
	li	a4,1
	sll	a5,a4,a5
	mv	a4,a5
	lw	a5,-20(s0)
	or	a5,a5,a4
	sw	a5,-20(s0)
.L7:
	lw	a5,-28(s0)
	addi	a5,a5,-1
	sw	a5,-28(s0)
.L6:
	lw	a5,-28(s0)
	bge	a5,zero,.L8
	lw	a5,-20(s0)
.L5:
	mv	a0,a5
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	divide, .-divide
	.ident	"GCC: (g2ee5e430018-dirty) 12.2.0"
