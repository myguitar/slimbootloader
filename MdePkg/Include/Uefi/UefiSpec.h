/** @file
  Include file that supports UEFI.

  This include file must contain things defined in the UEFI 2.7 specification.
  If a code construct is defined in the UEFI 2.7 specification it must be included
  by this include file.

  Copyright (c) 2006 - 2020, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __UEFI_SPEC_H__
#define __UEFI_SPEC_H__

#include <Uefi/UefiMultiPhase.h>

///
/// Enumeration of EFI memory allocation types.
///
typedef enum {
  ///
  /// Allocate any available range of pages that satisfies the request.
  ///
  AllocateAnyPages,
  ///
  /// Allocate any available range of pages whose uppermost address is less than
  /// or equal to a specified maximum address.
  ///
  AllocateMaxAddress,
  ///
  /// Allocate pages at a specified address.
  ///
  AllocateAddress,
  ///
  /// Maximum enumeration value that may be used for bounds checking.
  ///
  MaxAllocateType
} EFI_ALLOCATE_TYPE;

//
// Bit definitions for EFI_TIME.Daylight
//
#define EFI_TIME_ADJUST_DAYLIGHT  0x01
#define EFI_TIME_IN_DAYLIGHT      0x02

///
/// Value definition for EFI_TIME.TimeZone.
///
#define EFI_UNSPECIFIED_TIMEZONE  0x07FF

//
// Memory cacheability attributes
//
#define EFI_MEMORY_UC               0x0000000000000001ULL
#define EFI_MEMORY_WC               0x0000000000000002ULL
#define EFI_MEMORY_WT               0x0000000000000004ULL
#define EFI_MEMORY_WB               0x0000000000000008ULL
#define EFI_MEMORY_UCE              0x0000000000000010ULL
//
// Physical memory protection attributes
//
// Note: UEFI spec 2.5 and following: use EFI_MEMORY_RO as write-protected physical memory
// protection attribute. Also, EFI_MEMORY_WP means cacheability attribute.
//
#define EFI_MEMORY_WP               0x0000000000001000ULL
#define EFI_MEMORY_RP               0x0000000000002000ULL
#define EFI_MEMORY_XP               0x0000000000004000ULL
#define EFI_MEMORY_RO               0x0000000000020000ULL
//
// Physical memory persistence attribute.
// The memory region supports byte-addressable non-volatility.
//
#define EFI_MEMORY_NV               0x0000000000008000ULL
//
// The memory region provides higher reliability relative to other memory in the system.
// If all memory has the same reliability, then this bit is not used.
//
#define EFI_MEMORY_MORE_RELIABLE    0x0000000000010000ULL

//
// Note: UEFI spec 2.8 and following:
//
// Specific-purpose memory (SPM). The memory is earmarked for
// specific purposes such as for specific device drivers or applications.
// The SPM attribute serves as a hint to the OS to avoid allocating this
// memory for core OS data or code that can not be relocated.
//
#define EFI_MEMORY_SP               0x0000000000040000ULL
//
// If this flag is set, the memory region is capable of being
// protected with the CPU?s memory cryptographic
// capabilities. If this flag is clear, the memory region is not
// capable of being protected with the CPU?s memory
// cryptographic capabilities or the CPU does not support CPU
// memory cryptographic capabilities.
//
#define EFI_MEMORY_CPU_CRYPTO       0x0000000000080000ULL

//
// Runtime memory attribute
//
#define EFI_MEMORY_RUNTIME          0x8000000000000000ULL

///
/// Memory descriptor version number.
///
#define EFI_MEMORY_DESCRIPTOR_VERSION 1

///
/// Definition of an EFI memory descriptor.
///
typedef struct {
  ///
  /// Type of the memory region.
  /// Type EFI_MEMORY_TYPE is defined in the
  /// AllocatePages() function description.
  ///
  UINT32                Type;
  ///
  /// Physical address of the first byte in the memory region. PhysicalStart must be
  /// aligned on a 4 KiB boundary, and must not be above 0xfffffffffffff000. Type
  /// EFI_PHYSICAL_ADDRESS is defined in the AllocatePages() function description
  ///
  EFI_PHYSICAL_ADDRESS  PhysicalStart;
  ///
  /// Virtual address of the first byte in the memory region.
  /// VirtualStart must be aligned on a 4 KiB boundary,
  /// and must not be above 0xfffffffffffff000.
  ///
  EFI_VIRTUAL_ADDRESS   VirtualStart;
  ///
  /// NumberOfPagesNumber of 4 KiB pages in the memory region.
  /// NumberOfPages must not be 0, and must not be any value
  /// that would represent a memory page with a start address,
  /// either physical or virtual, above 0xfffffffffffff000.
  ///
  UINT64                NumberOfPages;
  ///
  /// Attributes of the memory region that describe the bit mask of capabilities
  /// for that memory region, and not necessarily the current settings for that
  /// memory region.
  ///
  UINT64                Attribute;
} EFI_MEMORY_DESCRIPTOR;

#endif
