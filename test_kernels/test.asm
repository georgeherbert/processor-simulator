addi x1 x2 -2048
addi x3 x4 2047

slti x0 x0 -2048
slti x0 x0 2047

sltiu x0 x0 0
sltiu x0 x0 2047
sltiu x0 x0 4294965248

andi x0 x0 -2048
andi x0 x0 2047

ori x0 x0 -2048
ori x0 x0 2047

xori x0 x0 -2048
xori x0 x0 2047

slli x0 x0 0
slli x0 x0 31

srli x0 x0 0
srli x0 x0 31

srai x0 x0 0
srai x0 x0 31

lui x0 -524288
lui x0 524287

auipc x0 -524288
auipc x0 524287


add x0 x0 x0

slt x0 x0 x0

sltu x0 x0 x0

and x0 x0 x0

or x0 x0 x0

xor x0 x0 x0

sll x0 x0 x0

srl x0 x0 x0

sub x0 x0 x0

sra x0 x0 x0


jal x0 -524288
jal x0 524287

jalr x0 x0 -2048
jalr x0 x0 2047


beq x0 x0 -2048
beq x0 x0 2047

bne x0 x0 -2048
bne x0 x0 2047

blt x0 x0 -2048
blt x0 x0 2047

bltu x0 x0 -2048
bltu x0 x0 2047

bge x0 x0 -2048
bge x0 x0 2047

bgeu x0 x0 -2048
bgeu x0 x0 2047


lw x0 x0 -2048
lw x0 x0 2047

lh x0 x0 -2048
lh x0 x0 2047

lhu x0 x0 -2048
lhu x0 x0 2047

lb x0 x0 -2048
lb x0 x0 2047

lbu x0 x0 -2048
lbu x0 x0 2047

sw x0 x0 -2048
sw x0 x0 2047

sh x0 x0 -2048
sh x0 x0 2047

sb x0 x0 -2048
sb x0 x0 2047