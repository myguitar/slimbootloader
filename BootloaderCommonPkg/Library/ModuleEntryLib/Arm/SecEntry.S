.text

GCC_ASM_EXPORT(_ModuleEntryPoint)
GCC_ASM_IMPORT(SecStartup)

ASM_PFX(_ModuleEntryPoint):
    ldr   r2, =ASM_PFX(SecStartup)
    blx   r2
