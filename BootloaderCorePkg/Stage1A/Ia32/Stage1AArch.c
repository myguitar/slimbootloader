/** @file

  Copyright (c) 2016 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// Global Descriptor Table (GDT)
//
STATIC
CONST IA32_SEGMENT_DESCRIPTOR
mGdtEntries[STAGE_GDT_ENTRY_COUNT] = {
  /* selector { Global Segment Descriptor                              } */
  /* 0x00 */  {{0,      0,  0,  0,    0,  0,  0,  0,    0,  0, 0,  0,  0}}, //null descriptor
  /* 0x08 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear data segment descriptor
  /* 0x10 */  {{0xffff, 0,  0,  0xb,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //linear code segment descriptor
  /* 0x18 */  {{0xffff, 0,  0,  0x3,  1,  0,  1,  0xf,  0,  0, 1,  1,  0}}, //system data segment descriptor
  /* 0x20 */  {{0xffff, 0,  0,  0xb,  1,  0,  1,  0xf,  0,  1, 0,  1,  0}}, //linear code (64-bit) segment descriptor
  /* 0x28 */  {{0xffff, 0,  0,  0xb,  1,  0,  1,  0x0,  0,  0, 0,  0,  0}}, //16-bit code segment descriptor
  /* 0x30 */  {{0xffff, 0,  0,  0x2,  1,  0,  1,  0x0,  0,  0, 0,  0,  0}}, //16-bit data segment descriptor
};

//
// IA32 Gdt register
//
STATIC
CONST IA32_DESCRIPTOR mGdt = {
  sizeof (mGdtEntries) - 1,
  (UINTN) mGdtEntries
  };

/**
  Get Arch Descriptor

  @retval      Arch Descriptor Pointer
**/
ARCH_DESCRIPTOR *
GetArchDescriptor (
  VOID
  )
{
  return (ARCH_DESCRIPTOR *)&mGdt;
}
