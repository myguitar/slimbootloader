/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/ArmGenericTimerCounterLib.h>

/**
  Read current timestamp.

  @return  64 bit current timestampe value.

**/
UINT64
EFIAPI
ReadTimeStamp (
  VOID
  )
{
  return GetPerformanceCounter ();
}

/**
  Get timestamp frequency in KHZ.

  @return   Timestamp frequency in KHZ.

**/
UINT32
EFIAPI
GetTimeStampFrequency (
  VOID
  )
{
  return (UINT32)ArmGenericTimerGetTimerFreq ();
}
