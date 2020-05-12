/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Stage1AArch.h>

#define TICKS_PER_MICRO_SEC     (PcdGet32 (PcdArmArchTimerFreqInHz)/1000000U)

STATIC
VOID
ArmArchTimerInit (
  VOID
  )
{
  if (ArmIsArchTimerImplemented ()) {
    //
    // Check if Architectural Timer frequency is pre-determined by the platform
    // (ie. nonzero).
    //
    if (PcdGet32 (PcdArmArchTimerFreqInHz) != 0) {
      //
      // Check if ticks/uS is not 0. The Architectural timer runs at constant
      // frequency, irrespective of CPU frequency. According to Generic Timer
      // Ref manual, lower bound of the frequency is in the range of 1-10MHz.
      //
      ASSERT (TICKS_PER_MICRO_SEC);

#ifdef MDE_CPU_ARM
      //
      // Only set the frequency for ARMv7. We expect the secure firmware to
      // have already done it.
      // If the security extension is not implemented, set Timer Frequency
      // here.
      //
      if ((ArmReadIdPfr1 () & ARM_PFR1_SEC) == 0x0) {
        ArmWriteCntFrq (PcdGet32 (PcdArmArchTimerFreqInHz));
      }
#endif
    }
  }
}

VOID
EFIAPI
PeiCommonExceptionEntry (
  IN UINT32 Entry,
  IN UINTN  LR
  )
{
  CHAR8           Buffer[100];
  UINTN           CharCount;

  switch (Entry) {
  case 0:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"Reset Exception at 0x%X\n\r",LR);
    break;
  case 1:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"Undefined Exception at 0x%X\n\r",LR);
    break;
  case 2:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"SWI Exception at 0x%X\n\r",LR);
    break;
  case 3:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"PrefetchAbort Exception at 0x%X\n\r",LR);
    break;
  case 4:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"DataAbort Exception at 0x%X\n\r",LR);
    break;
  case 5:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"Reserved Exception at 0x%X\n\r",LR);
    break;
  case 6:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"IRQ Exception at 0x%X\n\r",LR);
    break;
  case 7:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"FIQ Exception at 0x%X\n\r",LR);
    break;
  default:
    CharCount = AsciiSPrint (Buffer,sizeof (Buffer),"Unknown Exception at 0x%X\n\r",LR);
    break;
  }
  SerialPortWrite ((UINT8 *) Buffer, CharCount);
  while(1);
}

VOID
EFIAPI
ArchSecStartup (
  IN VOID *Param
  )
{
  // Init timer asap
  ArmArchTimerInit ();

  // Data Cache enabled on Primary core when MMU is enabled.
  ArmDisableDataCache ();
  // Invalidate Data cache
  ArmInvalidateDataCache ();
  // Invalidate instruction cache
  ArmInvalidateInstructionCache ();
  // Enable Instruction Caches on all cores.
  ArmEnableInstructionCache ();

  ASSERT (((UINTN)PeiVectorTable & ARM_VECTOR_TABLE_ALIGNMENT) == 0);
  ArmWriteVBar ((UINTN)PeiVectorTable);

  SecStartup (Param);
}

/**
  Get Arch Descriptor

  @retval      Arch Descriptor Pointer
**/
VOID *
EFIAPI
GetArchDescriptor (
  VOID
  )
{
  return NULL;
}
