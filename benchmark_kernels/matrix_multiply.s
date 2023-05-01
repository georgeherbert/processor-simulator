	.file	"matrix_multiply.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	_start
	.type	_start, @function
_start:
	addi	sp,sp,-528
	sw	ra,524(sp)
	sw	s0,520(sp)
	addi	s0,sp,528
	sw	zero,-20(s0)
	j	.L2
.L3:
	lw	a5,-20(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-20(s0)
	sw	a4,-208(a5)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L2:
	lw	a4,-20(s0)
	li	a5,49
	ble	a4,a5,.L3
	sw	zero,-24(s0)
	j	.L4
.L5:
	lw	a5,-24(s0)
	slli	a5,a5,2
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-24(s0)
	sw	a4,-408(a5)
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L4:
	lw	a4,-24(s0)
	li	a5,49
	ble	a4,a5,.L5
	addi	a5,s0,-524
	addi	a4,s0,-424
	addi	a3,s0,-224
	li	a2,5
	li	a1,10
	li	a0,5
	call	matrix_multiply
	lw	a5,-524(s0)
	mv	a0,a5
	lw	ra,524(sp)
	lw	s0,520(sp)
	addi	sp,sp,528
	jr	ra
	.size	_start, .-_start
	.align	2
	.globl	multiply
	.type	multiply, @function
multiply:
	addi	sp,sp,-48
	sw	s0,44(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	zero,-20(s0)
	sw	zero,-24(s0)
	j	.L8
.L9:
	lw	a4,-20(s0)
	lw	a5,-36(s0)
	add	a5,a4,a5
	sw	a5,-20(s0)
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L8:
	lw	a5,-40(s0)
	lw	a4,-24(s0)
	bltu	a4,a5,.L9
	lw	a5,-20(s0)
	mv	a0,a5
	lw	s0,44(sp)
	addi	sp,sp,48
	jr	ra
	.size	multiply, .-multiply
	.align	2
	.globl	matrix_multiply
	.type	matrix_multiply, @function
matrix_multiply:
	addi	sp,sp,-64
	sw	ra,60(sp)
	sw	s0,56(sp)
	sw	s1,52(sp)
	addi	s0,sp,64
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	sw	a2,-44(s0)
	sw	a3,-48(s0)
	sw	a4,-52(s0)
	sw	a5,-56(s0)
	sw	zero,-20(s0)
	j	.L12
.L17:
	sw	zero,-24(s0)
	j	.L13
.L16:
	lw	a1,-44(s0)
	lw	a0,-20(s0)
	call	multiply
	mv	a5,a0
	mv	a4,a5
	lw	a5,-24(s0)
	add	a5,a4,a5
	slli	a5,a5,2
	lw	a4,-56(s0)
	add	a5,a4,a5
	sw	zero,0(a5)
	sw	zero,-28(s0)
	j	.L14
.L15:
	lw	a1,-40(s0)
	lw	a0,-20(s0)
	call	multiply
	mv	a5,a0
	mv	a4,a5
	lw	a5,-28(s0)
	add	a5,a4,a5
	slli	a5,a5,2
	lw	a4,-48(s0)
	add	a5,a4,a5
	lw	s1,0(a5)
	lw	a1,-44(s0)
	lw	a0,-28(s0)
	call	multiply
	mv	a5,a0
	mv	a4,a5
	lw	a5,-24(s0)
	add	a5,a4,a5
	slli	a5,a5,2
	lw	a4,-52(s0)
	add	a5,a4,a5
	lw	a5,0(a5)
	mv	a1,a5
	mv	a0,s1
	call	multiply
	mv	s1,a0
	lw	a1,-44(s0)
	lw	a0,-20(s0)
	call	multiply
	mv	a3,a0
	mv	a4,a3
	lw	a5,-24(s0)
	add	a5,a4,a5
	slli	a5,a5,2
	lw	a4,-56(s0)
	add	a5,a4,a5
	lw	a4,0(a5)
	lw	a5,-24(s0)
	add	a5,a3,a5
	slli	a5,a5,2
	lw	a3,-56(s0)
	add	a5,a3,a5
	add	a4,s1,a4
	sw	a4,0(a5)
	lw	a5,-28(s0)
	addi	a5,a5,1
	sw	a5,-28(s0)
.L14:
	lw	a4,-28(s0)
	lw	a5,-40(s0)
	blt	a4,a5,.L15
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L13:
	lw	a4,-24(s0)
	lw	a5,-44(s0)
	blt	a4,a5,.L16
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
.L12:
	lw	a4,-20(s0)
	lw	a5,-36(s0)
	blt	a4,a5,.L17
	nop
	nop
	lw	ra,60(sp)
	lw	s0,56(sp)
	lw	s1,52(sp)
	addi	sp,sp,64
	jr	ra
	.size	matrix_multiply, .-matrix_multiply
	.ident	"GCC: (g2ee5e430018-dirty) 12.2.0"
