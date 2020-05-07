## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

##
# Import Modules
#
import os
import sys
import subprocess

sys.dont_write_bytecode = True
sys.path.append (os.path.join('..', '..'))
from BuildLoader import BaseBoard, STITCH_OPS, HASH_USAGE

class Board(BaseBoard):

    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        EXECUTE_IN_PLACE              = 0
        FREE_TEMP_RAM_TOP             = 0x50000000

        self.STAGE1_STACK_SIZE        = 0x00002000
        self.STAGE1_DATA_SIZE         = 0x00006000
        self.STAGE1_STACK_BASE        = 0x40080000 - (self.STAGE1_STACK_SIZE + self.STAGE1_DATA_SIZE)

        self.SYSTEM_MEMORY_BASE           = 0x40000000
        self.SYSTEM_MEMORY_SIZE           = 0x40000000

        self.VERINFO_IMAGE_ID         = 'ARM_QEMU '
        self.VERINFO_PROJ_MAJOR_VER   = 0
        self.VERINFO_PROJ_MINOR_VER   = 0
        self.VERINFO_SVN              = 1
        self.VERINFO_BUILD_DATE       = '02/11/2020'

        self.BOARD_NAME               = 'armqemu'
        self.BOARD_PKG_NAME           = 'ArmQemuBoardPkg'
        self.SILICON_PKG_NAME         = 'ArmQemuSocPkg'

        self.FLASH_LAYOUT_TOPDOWN     = 0
        self.FLASH_LAYOUT_START       = 0x00000000
        self.PCI_IO_BASE              = 0x3eff0000
        self.PCI_MEM32_BASE           = 0x10000000
        self.PCI_EXPRESS_BASE         = 0x3f000000
        self.USB_KB_POLLING_TIMEOUT   = 10

        self.HAVE_MEASURED_BOOT       = 0
        self.HAVE_VERIFIED_BOOT       = 0
        self.HAVE_VBT_BIN             = 0
        self.HAVE_FSP_BIN             = 0
        self.HAVE_ACPI_TABLE          = 0
        self.ENABLE_SPLASH            = 0
        self.ENABLE_FRAMEBUFFER_INIT  = 0
        self.ENABLE_FWU               = 0
        self.ENABLE_GRUB_CONFIG       = 0
        self.ENABLE_LINUX_PAYLOAD     = 0
        self.ENABLE_CRYPTO_SHA_OPT    = 0
        self.ENABLE_SMP_INIT          = 0
        self.ENABLE_PCI_ENUM          = 1

        self.LOADER_RSVD_MEM_SIZE = 0x00100000

        self.CPU_MAX_LOGICAL_PROCESSOR_NUMBER = 255

        # To enable source debug, set 1 to self.ENABLE_SOURCE_DEBUG
        # self.ENABLE_SOURCE_DEBUG  = 1



        # For test purpose
        # self.SKIP_STAGE1A_SOURCE_DEBUG = 1

        self.DEBUG_OUTPUT_DEVICE_MASK = 0x00000002

        # BIT0:Serial  BIT1:USB KB
        # Support serial port input console by default
        self.CONSOLE_IN_DEVICE_MASK  = 0x00000001

        # BIT0:Serial  BIT1:GFX
        self.CONSOLE_OUT_DEVICE_MASK = 0x00000001

        # FWU_PLD | PLD | Stage2 | Stage1B
        # Let Stage1A verifies Stage1B
        # self.VERIFIED_BOOT_HASH_MASK  = 0x000000F
        self.VERIFIED_BOOT_HASH_MASK  = 0x0000000

        self.RESET_SIZE           = 0x00001000
        self.STAGE1A_SIZE         = 0x00008000
        self.STAGE1B_SIZE         = 0x0000A000
        self.STAGE1_SIZE          = self.RESET_SIZE + self.STAGE1A_SIZE + self.STAGE1B_SIZE
        self.STAGE2_SIZE          = 0x0000A000

        #self.SIIPFW_SIZE          = 0x00010000
        #self.EPAYLOAD_SIZE        = 0x0020D000
        self.EPAYLOAD_SIZE        = 0x00000000
        self.PAYLOAD_SIZE         = 0x00020000
        self.CFGDATA_SIZE         = 0x00001000
        self.KEYHASH_SIZE         = 0x00001000
        self.VARIABLE_SIZE        = 0x00002000
        self.SBLRSVD_SIZE         = 0x00001000
        self.FWUPDATE_SIZE        = 0x00018000 if self.ENABLE_FWU else 0

        self._REDUNDANT_LAYOUT    = 0
        if not self._REDUNDANT_LAYOUT:
            self.TOP_SWAP_SIZE      = 0x000000
            self.REDUNDANT_SIZE     = 0x000000
            self.NON_VOLATILE_SIZE  = 0x000000
            self.NON_REDUNDANT_SIZE = 0x100000
        else:
            self.TOP_SWAP_SIZE      = 0x080000
            self.REDUNDANT_SIZE     = 0x080000
            self.NON_VOLATILE_SIZE  = 0x001000
            self.NON_REDUNDANT_SIZE = 0x2DF000


        self.SLIMBOOTLOADER_SIZE = (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 + \
            self.NON_REDUNDANT_SIZE + self.NON_VOLATILE_SIZE

        self.STAGE1A_XIP          = EXECUTE_IN_PLACE
        if not self.STAGE1A_XIP:
            # For Stage1A, STAGE1_FD_BASE is always located at 4GB - STAGE1_FD_SIZE
            # And execution will start from there.  If STAGE1A_XIP is 0, Stage1A
            # will relocate itself to STAGE1A_LOAD_BASE to continue execution
            FREE_TEMP_RAM_TOP      -= self.STAGE1A_SIZE
            self.STAGE1A_LOAD_BASE  = FREE_TEMP_RAM_TOP

        self.STAGE1B_XIP          = EXECUTE_IN_PLACE
        if not self.STAGE1B_XIP:
            # For Stage1B, it can be compressed if STAGE1B_XIP is 0
            # If so, STAGE1B_FD_BASE/STAGE1B_FD_SIZE need to be defined
            self.STAGE1B_FD_SIZE    = 0x30000
            if self.NO_OPT_MODE:
                self.STAGE1B_FD_SIZE += 0x2000
            self.STAGE1B_FD_BASE    = FREE_TEMP_RAM_TOP - self.STAGE1B_FD_SIZE

        # For Stage2, it is always compressed.
        # if STAGE2_LOAD_HIGH is 1, STAGE2_FD_BASE will be ignored
        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x00060000

        if self.NO_OPT_MODE:
            self.OS_LOADER_FD_SIZE   += 0x00010000
            self.OS_LOADER_FD_NUMBLK  = self.OS_LOADER_FD_SIZE // self.FLASH_BLOCK_SIZE

        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE

        # Add following to force to use a specific platform ID
        # By default it uses auto-detection.
        #self._PLATFORM_ID         = 1

        self._CFGDATA_INT_FILE    = ['CfgDataExt_Brd1.dlt']
        self._CFGDATA_EXT_FILE    = []

        # If mulitple VBT table support is required, list them as:
        #   {VbtImageId1 : VbtFileName1, VbtImageId2 : VbtFileName2, ...}
        # VbtImageId is ID to identify a VBT image. It is a UINT32 number to match
        #   the ImageId field in the VBT container.
        # VbtFileName is the VBT file name. It needs to be located under platform
        #   VbtBin folder.
        self._MULTI_VBT_FILE      = {1:'Vbt800x600.dat', 2:'Vbt1024x768.dat'}

    def GetDscLibrarys (self):
        dsc_libs = {}
        # These libraries will be added into the DSC files
        dsc_libs['ARM'] = [
            'IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsicArmVirt.inf',
            'LoaderLib|Platform/CommonBoardPkg/Library/LoaderLib/LoaderLib.inf',
            'PlatformHookLib|Silicon/$(SILICON_PKG_NAME)/Library/PlatformHookLib/PlatformHookLib.inf',
            'ArmGenericTimerCounterLib|ArmPkg/Library/ArmGenericTimerVirtCounterLib/ArmGenericTimerVirtCounterLib.inf',
            'ArmLib|ArmPkg/Library/ArmLib/ArmBaseLib.inf',
            'TimerLib|ArmPkg/Library/ArmArchTimerLib/ArmArchTimerLib.inf',
            'PL011UartLib|Silicon/ArmCommonSocPkg/Library/PL011UartLib/PL011UartLib.inf',
            'SerialPortLib|Silicon/$(SILICON_PKG_NAME)/Library/SerialPortLib/SerialPortLib.inf'
        ]
        return dsc_libs

    def GetKeyHashList (self):
        # Define a set of new key used for different purposes
        # The key is either public key PEM format or private key PEM format
        pub_key_list = [
          (
            # Use a single test key
            HASH_USAGE['PUBKEY_CFG_DATA'] | HASH_USAGE['PUBKEY_FWU'] | HASH_USAGE['PUBKEY_OS'] | HASH_USAGE['PUBKEY_CONT_DEF'],
            'TestSigningPrivateKey.pem'
          ),
        ]
        return pub_key_list

    def GetContainerList (self):
        container_list = []
        return container_list

    def PlatformBuildHook (self, build, phase):
        if phase == 'pre-build:after':
            print('Build Reset Vector!')
            vtf_dir = os.path.join('BootloaderCorePkg', 'Stage1A', 'Arm', 'Vtf0')
            vtf_args = '0x%x' % self.RESET_SIZE
            x = subprocess.call([sys.executable, 'Build.py', 'arm', vtf_args],  cwd=vtf_dir)
            if x: raise Exception ('Failed to build reset vector !')

            src_path = os.path.join(vtf_dir, 'Bin', 'ResetVector.arm.raw')
            fi = open(src_path, 'rb')
            bins = bytearray(fi.read())
            fi.close()

            out_path = os.path.join(build._fv_dir, 'RESET.bin')
            fo = open(out_path,'wb')
            fo.write(bins)
            fo.close()

    def GetImageLayout (self):

        compress = '' if self.STAGE1B_XIP else 'Lz4'
        fwu_mode = STITCH_OPS.MODE_FILE_PAD if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR

        img_list = []

        # output files need to have unique base name (excluding file extension)
        # output files ends with 'rom' extension will be copied over for final stitching
        if self.REDUNDANT_SIZE == 0:
            img_list.extend ([
                ('STAGE1.bin', [
                    ('RESET.bin'    ,  ''        , self.RESET_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A.fd'   ,  ''        , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1B.fd'   ,  compress  , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('SlimBootloader.bin', [
                    ('STAGE1.bin'   ,  ''        , self.STAGE1_SIZE,   STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  ,  ''        , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('PAYLOAD.bin'  ,  'Lzma'    , self.PAYLOAD_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('VARIABLE.bin' ,  ''        , self.VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('SBLRSVD.bin',    ''        , self.SBLRSVD_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ])
        else:
            img_list.extend ([
                ('NON_VOLATILE.bin', [
                    ('SBLRSVD.bin',    ''        , self.SBLRSVD_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_HEAD),
                    ]
                ),
                ('NON_REDUNDANT.bin', [
                    ('VARIABLE.bin' ,  ''        , self.VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_HEAD),
                    ('PAYLOAD.bin'  ,  'Lzma'    , self.PAYLOAD_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
#                    ('EPAYLOAD.bin' ,  ''        , self.EPAYLOAD_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
#                    ('SIIPFW.bin'   ,  ''        , self.SIIPFW_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ]
                ),
                ('REDUNDANT_A.bin', [
                    ('STAGE1B_A.fd' ,  compress  , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
#                    ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, fwu_mode,                 STITCH_OPS.MODE_POS_HEAD),
                    ('CFGDATA.bin'  ,  ''        , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('KEYHASH.bin'  ,  ''        , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ]
                ),
                ('REDUNDANT_B.bin', [
                    ('STAGE1B_B.fd' ,  compress  , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
 #                   ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, fwu_mode,                 STITCH_OPS.MODE_POS_HEAD),
                    ('CFGDATA.bin'  , ''         , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('KEYHASH.bin'  ,  ''        , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ]
                ),
                ('TOP_SWAP_A.bin', [
                    ('STAGE1A_A.fd'      , ''      , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_HEAD),
                    ]
                ),
                ('TOP_SWAP_B.bin', [
                    ('STAGE1A_B.fd'      , ''      , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_HEAD),
                    ]
                ),
                ('SlimBootloader.bin', [
                    ('TOP_SWAP_A.bin'    , '' , self.TOP_SWAP_SIZE,      STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('TOP_SWAP_B.bin'    , '' , self.TOP_SWAP_SIZE,      STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('REDUNDANT_A.bin'   , '' , self.REDUNDANT_SIZE,     STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('REDUNDANT_B.bin'   , '' , self.REDUNDANT_SIZE,     STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('NON_REDUNDANT.bin' , '' , self.NON_REDUNDANT_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('NON_VOLATILE.bin'  , '' , self.NON_VOLATILE_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ]
                ),
                ])

        return img_list
