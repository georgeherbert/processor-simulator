	.file	"arithmetic.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-192
	sw	s0,188(sp)
	addi	s0,sp,192
	li	a5,100
	sw	a5,-184(s0)
	li	a5,1
	sw	a5,-20(s0)
	li	a5,2
	sw	a5,-24(s0)
	li	a5,3
	sw	a5,-28(s0)
	li	a5,4
	sw	a5,-32(s0)
	li	a5,5
	sw	a5,-36(s0)
	li	a5,6
	sw	a5,-40(s0)
	li	a5,7
	sw	a5,-44(s0)
	li	a5,8
	sw	a5,-48(s0)
	li	a5,9
	sw	a5,-52(s0)
	li	a5,10
	sw	a5,-56(s0)
	li	a5,11
	sw	a5,-60(s0)
	li	a5,12
	sw	a5,-64(s0)
	li	a5,13
	sw	a5,-68(s0)
	li	a5,14
	sw	a5,-72(s0)
	li	a5,15
	sw	a5,-76(s0)
	li	a5,16
	sw	a5,-80(s0)
	li	a5,17
	sw	a5,-84(s0)
	li	a5,18
	sw	a5,-88(s0)
	li	a5,19
	sw	a5,-92(s0)
	li	a5,20
	sw	a5,-96(s0)
	li	a5,21
	sw	a5,-100(s0)
	li	a5,22
	sw	a5,-104(s0)
	li	a5,23
	sw	a5,-108(s0)
	li	a5,24
	sw	a5,-112(s0)
	li	a5,25
	sw	a5,-116(s0)
	li	a5,26
	sw	a5,-120(s0)
	sw	zero,-124(s0)
	sw	zero,-128(s0)
	sw	zero,-132(s0)
	sw	zero,-136(s0)
	sw	zero,-140(s0)
	sw	zero,-144(s0)
	sw	zero,-148(s0)
	sw	zero,-152(s0)
	sw	zero,-156(s0)
	sw	zero,-160(s0)
	sw	zero,-164(s0)
	sw	zero,-168(s0)
	sw	zero,-172(s0)
	sw	zero,-176(s0)
	sw	zero,-180(s0)
	j	.L2
.L3:
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	add	a5,a4,a5
	sw	a5,-124(s0)
	lw	a4,-28(s0)
	lw	a5,-32(s0)
	sub	a5,a4,a5
	sw	a5,-128(s0)
	lw	a4,-36(s0)
	lw	a5,-40(s0)
	xor	a5,a4,a5
	sw	a5,-132(s0)
	lw	a4,-44(s0)
	lw	a5,-48(s0)
	or	a5,a4,a5
	sw	a5,-136(s0)
	lw	a4,-52(s0)
	lw	a5,-56(s0)
	and	a5,a4,a5
	sw	a5,-140(s0)
	lw	a5,-60(s0)
	slli	a5,a5,2
	sw	a5,-144(s0)
	lw	a5,-64(s0)
	srai	a5,a5,3
	sw	a5,-148(s0)
	lw	a4,-68(s0)
	lw	a5,-72(s0)
	or	a5,a4,a5
	sw	a5,-152(s0)
	lw	a4,-76(s0)
	lw	a5,-80(s0)
	xor	a5,a4,a5
	sw	a5,-156(s0)
	lw	a4,-84(s0)
	lw	a5,-88(s0)
	and	a5,a4,a5
	sw	a5,-160(s0)
	lw	a4,-92(s0)
	lw	a5,-96(s0)
	or	a5,a4,a5
	sw	a5,-164(s0)
	lw	a4,-100(s0)
	lw	a5,-104(s0)
	xor	a5,a4,a5
	sw	a5,-168(s0)
	lw	a4,-108(s0)
	lw	a5,-112(s0)
	and	a5,a4,a5
	sw	a5,-172(s0)
	lw	a4,-116(s0)
	lw	a5,-120(s0)
	or	a5,a4,a5
	sw	a5,-176(s0)
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
	lw	a5,-24(s0)
	addi	a5,a5,2
	sw	a5,-24(s0)
	lw	a5,-28(s0)
	addi	a5,a5,3
	sw	a5,-28(s0)
	lw	a5,-32(s0)
	addi	a5,a5,4
	sw	a5,-32(s0)
	lw	a5,-36(s0)
	addi	a5,a5,5
	sw	a5,-36(s0)
	lw	a5,-40(s0)
	addi	a5,a5,6
	sw	a5,-40(s0)
	lw	a5,-44(s0)
	addi	a5,a5,7
	sw	a5,-44(s0)
	lw	a5,-48(s0)
	addi	a5,a5,8
	sw	a5,-48(s0)
	lw	a5,-52(s0)
	addi	a5,a5,9
	sw	a5,-52(s0)
	lw	a5,-56(s0)
	addi	a5,a5,10
	sw	a5,-56(s0)
	lw	a5,-60(s0)
	addi	a5,a5,11
	sw	a5,-60(s0)
	lw	a5,-64(s0)
	addi	a5,a5,12
	sw	a5,-64(s0)
	lw	a5,-68(s0)
	addi	a5,a5,13
	sw	a5,-68(s0)
	lw	a5,-72(s0)
	addi	a5,a5,14
	sw	a5,-72(s0)
	lw	a5,-76(s0)
	addi	a5,a5,15
	sw	a5,-76(s0)
	lw	a5,-80(s0)
	addi	a5,a5,16
	sw	a5,-80(s0)
	lw	a5,-84(s0)
	addi	a5,a5,17
	sw	a5,-84(s0)
	lw	a5,-88(s0)
	addi	a5,a5,18
	sw	a5,-88(s0)
	lw	a5,-92(s0)
	addi	a5,a5,19
	sw	a5,-92(s0)
	lw	a5,-96(s0)
	addi	a5,a5,20
	sw	a5,-96(s0)
	lw	a5,-100(s0)
	addi	a5,a5,21
	sw	a5,-100(s0)
	lw	a5,-104(s0)
	addi	a5,a5,22
	sw	a5,-104(s0)
	lw	a5,-108(s0)
	addi	a5,a5,23
	sw	a5,-108(s0)
	lw	a5,-112(s0)
	addi	a5,a5,24
	sw	a5,-112(s0)
	lw	a5,-116(s0)
	addi	a5,a5,25
	sw	a5,-116(s0)
	lw	a5,-120(s0)
	addi	a5,a5,26
	sw	a5,-120(s0)
	lw	a5,-180(s0)
	addi	a5,a5,1
	sw	a5,-180(s0)
.L2:
	lw	a4,-180(s0)
	lw	a5,-184(s0)
	blt	a4,a5,.L3
	lw	a4,-124(s0)
	lw	a5,-128(s0)
	add	a4,a4,a5
	lw	a5,-132(s0)
	add	a4,a4,a5
	lw	a5,-136(s0)
	add	a4,a4,a5
	lw	a5,-140(s0)
	add	a4,a4,a5
	lw	a5,-144(s0)
	add	a4,a4,a5
	lw	a5,-148(s0)
	add	a4,a4,a5
	lw	a5,-152(s0)
	add	a4,a4,a5
	lw	a5,-156(s0)
	add	a4,a4,a5
	lw	a5,-160(s0)
	add	a4,a4,a5
	lw	a5,-164(s0)
	add	a4,a4,a5
	lw	a5,-168(s0)
	add	a4,a4,a5
	lw	a5,-172(s0)
	add	a5,a4,a5
	lw	a4,-176(s0)
	add	a5,a4,a5
	sw	a5,-188(s0)
	lw	a5,-188(s0)
	mv	a0,a5
	lw	s0,188(sp)
	addi	sp,sp,192
	jr	ra
	.size	main, .-main
	.ident	"GCC: (g2ee5e430018-dirty) 12.2.0"
