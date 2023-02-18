	.file	"euclidean.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	_start
	.type	_start, @function
_start:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
	li	a5,487010304
	addi	a5,a5,647
	sw	a5,-20(s0)
	li	a5,686
	sw	a5,-24(s0)
	lw	a1,-24(s0)
	lw	a0,-20(s0)
	call	euclidean_algorithm
	sw	a0,-28(s0)
	lw	a5,-28(s0)
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	_start, .-_start
	.align	2
	.globl	euclidean_algorithm
	.type	euclidean_algorithm, @function
euclidean_algorithm:
	addi	sp,sp,-32
	sw	s0,28(sp)
	addi	s0,sp,32
	sw	a0,-20(s0)
	sw	a1,-24(s0)
	j	.L4
.L6:
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	bleu	a4,a5,.L5
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	sub	a5,a4,a5
	sw	a5,-20(s0)
	j	.L4
.L5:
	lw	a4,-24(s0)
	lw	a5,-20(s0)
	sub	a5,a4,a5
	sw	a5,-24(s0)
.L4:
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	bne	a4,a5,.L6
	lw	a5,-20(s0)
	mv	a0,a5
	lw	s0,28(sp)
	addi	sp,sp,32
	jr	ra
	.size	euclidean_algorithm, .-euclidean_algorithm
	.ident	"GCC: (g2ee5e430018-dirty) 12.2.0"
