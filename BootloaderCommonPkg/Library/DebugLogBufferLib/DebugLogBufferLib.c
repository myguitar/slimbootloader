/** @file
  Provide Log Buffer Library functions.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/DebugLogBufferLib.h>
#include <Guid/LoaderPlatformDataGuid.h>

/**
  Write data from buffer to console buffer.

  Writes NumberOfBytes data bytes from Buffer to the serial device.
  The number of bytes actually written to the serial device is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the serial device.
                           If this value is less than NumberOfBytes, then the write operation failed.

**/
UINTN
EFIAPI
DebugLogBufferWrite (
  IN UINT8     *Buffer,
  IN UINTN      NumberOfBytes
  )
{
  DEBUG_LOG_BUFFER_HEADER  *LogBufHdr;
  UINTN                     RemainingBytes;
  UINT32                    BufferIndex;

  // This function will be called by DEBUG or ASSERT macro.
  // So please DON'T use DEBUG/ASSERT macro inside this function,
  // to avoid recursion.
  LogBufHdr = (DEBUG_LOG_BUFFER_HEADER *) GetDebugLogBufferPtr ();
  if (LogBufHdr == NULL) {
    return 0;
  }

  if (LogBufHdr->Signature != DEBUG_LOG_BUFFER_SIGNATURE) {
    return 0;
  }

  RemainingBytes = 0;

  while (!AcquireSpinLockOrFail (&LogBufHdr->SpinLock)) {
    CpuPause ();
  }

  //
  // Something wrong in Debug Log Buffer.
  // Reset buffer index and continue to record logs.
  //
  if (LogBufHdr->UsedLength > LogBufHdr->TotalLength) {
    LogBufHdr->UsedLength = LogBufHdr->HeaderLength;
  }

  //
  // Update LogBufHdr info first to minimize spinlock wait
  //
  BufferIndex = LogBufHdr->UsedLength - LogBufHdr->HeaderLength;
  if (LogBufHdr->UsedLength + NumberOfBytes >= LogBufHdr->TotalLength) {
    RemainingBytes = LogBufHdr->UsedLength + NumberOfBytes - LogBufHdr->TotalLength;
    NumberOfBytes  = LogBufHdr->TotalLength - LogBufHdr->UsedLength;
    LogBufHdr->UsedLength = LogBufHdr->HeaderLength + (UINT32)RemainingBytes;
    LogBufHdr->Attribute |= DEBUG_LOG_BUFFER_ATTRIBUTE_FULL;
  } else {
    LogBufHdr->UsedLength += (UINT32)NumberOfBytes;
  }

  ReleaseSpinLock (&LogBufHdr->SpinLock);

  if (NumberOfBytes > 0) {
    CopyMem (&LogBufHdr->Buffer[BufferIndex], Buffer, NumberOfBytes);
  }

  //
  // Handle Ring Buffer
  //
  if (RemainingBytes > 0) {
    CopyMem (&LogBufHdr->Buffer[0], Buffer + NumberOfBytes, RemainingBytes);
  }

  return (NumberOfBytes + RemainingBytes);
}
