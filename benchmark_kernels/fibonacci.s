	.file	"fibonacci.c"
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
	li	a0,46
	call	fibonacci
	mv	a5,a0
	mv	a0,a5
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	_start, .-_start
	.align	2
	.globl	fibonacci
	.type	fibonacci, @function
fibonacci:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	mv	a1,a0
	sb	a1,-33(s0)
	mv	a1,sp
	mv	t3,a1
	lbu	a0,-33(s0)
	li	a1,1
	bgtu	a0,a1,.L4
	lbu	a5,-33(s0)
	j	.L5
.L4:
	lbu	a1,-33(s0)
	addi	a1,a1,1
	addi	a0,a1,-1
	sw	a0,-24(s0)
	mv	a0,a1
	mv	t1,a0
	li	t2,0
	srli	a0,t1,27
	slli	a3,t2,5
	or	a3,a0,a3
	slli	a2,t1,5
	mv	a3,a1
	mv	a6,a3
	li	a7,0
	srli	a3,a6,27
	slli	a5,a7,5
	or	a5,a3,a5
	slli	a4,a6,5
	mv	a5,a1
	slli	a5,a5,2
	addi	a5,a5,15
	srli	a5,a5,4
	slli	a5,a5,4
	sub	sp,sp,a5
	mv	a5,sp
	addi	a5,a5,3
	srli	a5,a5,2
	slli	a5,a5,2
	sw	a5,-28(s0)
	lw	a5,-28(s0)
	sw	zero,0(a5)
	lw	a5,-28(s0)
	li	a4,1
	sw	a4,4(a5)
	li	a5,2
	sb	a5,-17(s0)
	j	.L6
.L7:
	lbu	a5,-17(s0)
	addi	a5,a5,-1
	lw	a4,-28(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a3,0(a5)
	lbu	a5,-17(s0)
	addi	a5,a5,-2
	lw	a4,-28(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a4,0(a5)
	lbu	a5,-17(s0)
	add	a4,a3,a4
	lw	a3,-28(s0)
	slli	a5,a5,2
	add	a5,a3,a5
	sw	a4,0(a5)
	lbu	a5,-17(s0)
	addi	a5,a5,1
	sb	a5,-17(s0)
.L6:
	lbu	a4,-17(s0)
	lbu	a5,-33(s0)
	bleu	a4,a5,.L7
	lbu	a5,-33(s0)
	lw	a4,-28(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a5,0(a5)
.L5:
	mv	sp,t3
	mv	a0,a5
	addi	sp,s0,-48
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	fibonacci, .-fibonacci
	.ident	"GCC: (g2ee5e430018-dirty) 12.2.0"
