/** @file
  MP init library implementation.

  Copyright (c) 2015 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/MpInitLib.h>

/** @file
  MP init library implementation.

  Copyright (c) 2015 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/**
  Common CPU initialization routine.

  @param[in]  Index       CPU index to initialize.

  @retval EFI_SUCCESS     CPU init has been done successfully.

**/
EFI_STATUS
EFIAPI
CpuInit (
  IN UINT32               Index
  )
{
  return EFI_SUCCESS;
}

/**
  BSP initialization routine.

**/
VOID
BspInit (
  VOID
  )
{
}


/**
  Multiprocessor Initialization.

  @param[in]  Phase       Initialization phase for MP.

  @retval EFI_SUCCESS     MP init has been done successfully for current phase.

**/
EFI_STATUS
EFIAPI
MpInit (
  IN MP_INIT_PHASE  Phase
  )
{
  return EFI_SUCCESS;

}


/**
  Get processor info for all CPUs.

  @retval    Pointer to SYS_CPU_INFO structure.

**/
SYS_CPU_INFO *
EFIAPI
MpGetInfo (
  VOID
  )
{
  return NULL;
}


/**
  Get processor task state for all CPUs.

  @retval    Pointer to SYS_CPU_TASK structure containing task info.

**/
SYS_CPU_TASK *
EFIAPI
MpGetTask (
  VOID
  )
{
  return NULL;
}


/**
  Run a task function for a specific processor.

  @param[in]  Index       CPU index
  @param[in]  TaskProc    Task function pointer
  @param[in]  Argument    Argument for the task function

  @retval EFI_INVALID_PARAMETER   Invalid Index parameter.
  @retval EFI_NOT_READY           CPU state is not ready for new task yet.
  @retval EFI_SUCCESS             CPU accepted the new task successfully.

**/
EFI_STATUS
EFIAPI
MpRunTask (
  IN  UINT32         Index,
  IN  CPU_TASK_PROC  TaskProc,
  IN  UINT32         Argument
  )
{
  return EFI_SUCCESS;
}


/**
  Dump MP task running state

  This is a debug function to dump current task running state for each
  processor.


**/
VOID
EFIAPI
MpDumpTask (
  VOID
  )
{
}
