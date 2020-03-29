/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/PL011UartLib.h>
#include <Protocol/SerialIo.h>

/**
  Initialize the serial device hardware.

  If no initialization is required, then return RETURN_SUCCESS.
  If the serial device was successfully initialized, then return RETURN_SUCCESS.
  If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.

  @retval EFI_SUCCESS        The serial device was initialized.
  @retval EFI_DEVICE_ERROR   The serial device could not be initialized.
**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  UINT64              BaudRate;
  UINT32              ReceiveFifoDepth;
  EFI_PARITY_TYPE     Parity;
  UINT8               DataBits;
  EFI_STOP_BITS_TYPE  StopBits;

  BaudRate          = 38400;
  ReceiveFifoDepth  = 0;
  Parity            = (EFI_PARITY_TYPE)1;
  DataBits          = 8;
  StopBits          = (EFI_STOP_BITS_TYPE) 1;

  return PL011UartInitializePort (
           (UINTN)GetSerialPortBase (),
           24000000,
           &BaudRate,
           &ReceiveFifoDepth,
           &Parity,
           &DataBits,
           &StopBits
           );
}

/**
  Write data from buffer to serial device.

  Writes NumberOfBytes data bytes from Buffer to the serial device.
  The number of bytes actually written to the serial device is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the serial device.
                           If this value is less than NumberOfBytes, then the read operation failed.
**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  )
{
  return PL011UartWrite ((UINTN)GetSerialPortBase (), Buffer, NumberOfBytes);
}

/**
  Reads data from a serial device into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the serial device.
  @param  NumberOfBytes    Number of bytes to read from the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the serial device.
                           If this value is less than NumberOfBytes, then the read operation failed.
**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
)
{
  return PL011UartRead ((UINTN)GetSerialPortBase (), Buffer, NumberOfBytes);
}

/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls aserial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.
**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{
  return PL011UartPoll ((UINTN)GetSerialPortBase ());
}
