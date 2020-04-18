.text

GCC_ASM_EXPORT(AsmFlushCacheRange)
GCC_ASM_EXPORT(AsmEnableAvx)

ASM_PFX(AsmFlushCacheRange):
    bx  lr

ASM_PFX(AsmEnableAvx):
    bx  lr
