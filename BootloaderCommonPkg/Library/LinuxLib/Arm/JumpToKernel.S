.text

GCC_ASM_EXPORT(JumpToKernel)
ASM_PFX(JumpToKernel):
    bx      lr

GCC_ASM_EXPORT(JumpToKernel64)
ASM_PFX(JumpToKernel64):
    bx      lr
