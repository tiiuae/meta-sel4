/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Persistent Storage - pstore.h
 *
 * Copyright (C) 2010 Intel Corporation <tony.luck@intel.com>
 *
 * This code is the generic layer to export data records from platform
 * level persistent storage via a file system.
 */
#ifndef _SEL4_SUPPORT_H
#define _SEL4_SUPPORT_H

#ifdef CONFIG_KERNEL_MCS
#define MCS_PARAM_DECL(r)    register seL4_Word reply_reg asm(r) = reply
#define MCS_PARAM    , "r"(reply_reg)
#else
#define MCS_PARAM_DECL(r)
#define MCS_PARAM
#endif

#define SEL4_PACKED             __attribute__((packed))
typedef uint64_t seL4_Word;

char *entry_names[] = {
    "Entry_Interrupt",
    "Entry_UnknownSyscall",
    "Entry_UserLevelFault",
    "Entry_DebugFault",
    "Entry_VMFault",
    "Entry_Syscall",
    "Entry_UnimplementedDevice",
    "Entry_VCPUFault",
    "Entry_Switch",
};

typedef enum {
    seL4_NoError = 0,
    seL4_InvalidArgument,
    seL4_InvalidCapability,
    seL4_IllegalOperation,
    seL4_RangeError,
    seL4_AlignmentError,
    seL4_FailedLookup,
    seL4_TruncatedMessage,
    seL4_DeleteFirst,
    seL4_RevokeFirst,
    seL4_NotEnoughMemory,

    /* This should always be the last item in the list
     * so it gives a count of the number of errors in the
     * enum.
     */
    seL4_NumErrors
} seL4_Error;

typedef enum {
       seL4_SysCall = -1,
       seL4_SysReplyRecv = -2,
       seL4_SysSend = -3,
       seL4_SysNBSend = -4,
       seL4_SysRecv = -5,
       seL4_SysReply = -6,
       seL4_SysYield = -7,
       seL4_SysNBRecv = -8,
       seL4_SysDebugPutChar = -9,
       seL4_SysDebugDumpScheduler = -10,
       seL4_SysDebugHalt = -11,
       seL4_SysDebugCapIdentify = -12,
       seL4_SysDebugSnapshot = -13,
       seL4_SysDebugNameThread = -14,
       seL4_SysDebugSendIPI = -15,
       seL4_SysDebugRun = -16,
       seL4_SysBenchmarkFlushCaches = -17,
       seL4_SysBenchmarkResetLog = -18,
       seL4_SysBenchmarkFinalizeLog = -19,
       seL4_SysBenchmarkSetLogBuffer = -20,
       seL4_SysBenchmarkNullSyscall = -21,
       seL4_SysBenchmarkGetThreadUtilisation = -22,
       seL4_SysBenchmarkResetThreadUtilisation = -23,
       seL4_SysBenchmarkDumpAllThreadsUtilisation = -24,
       seL4_SysBenchmarkResetAllThreadsUtilisation = -25,
       seL4_SysX86DangerousWRMSR = -26,
       seL4_SysX86DangerousRDMSR = -27,
       seL4_SysVMEnter = -28,
       seL4_SysSetTLSBase = -29,
} seL4_Syscall_ID;

/**
 * @brief Kernel entry logging
 *
 * Encapsulates useful info about the cause of the kernel entry
 */
typedef struct SEL4_PACKED kernel_entry {
    seL4_Word path: 4;
    union {
        struct {
            seL4_Word core: 3;
            seL4_Word word: 26;
        };
        /* Tracked kernel entry info filled from outside this file */
        struct {
            seL4_Word syscall_no: 4;
            seL4_Word cap_type: 5;
            seL4_Word is_fastpath: 1;
            seL4_Word invocation_tag: 19;
        };
        void *next;
    };
} kernel_entry_t;

typedef struct benchmark_syscall_log_entry {
    uint64_t  start_time;
    uint32_t  duration;
    kernel_entry_t entry;
} benchmark_track_kernel_entry_t;

static inline void arm_sys_send_recv(seL4_Word sys, seL4_Word dest, seL4_Word *out_badge, seL4_Word info_arg,
                                     seL4_Word *out_info, seL4_Word *in_out_mr0, seL4_Word *in_out_mr1, seL4_Word *in_out_mr2, seL4_Word *in_out_mr3, seL4_Word reply)
{
    register seL4_Word destptr asm("x0") = dest;
    register seL4_Word info asm("x1") = info_arg;

    /* Load beginning of the message into registers. */
    register seL4_Word msg0 asm("x2") = *in_out_mr0;
    register seL4_Word msg1 asm("x3") = *in_out_mr1;
    register seL4_Word msg2 asm("x4") = *in_out_mr2;
    register seL4_Word msg3 asm("x5") = *in_out_mr3;
    MCS_PARAM_DECL("x6");

    /* Perform the system call. */
    register seL4_Word scno asm("x7") = sys;
    asm volatile(
        "hvc #0"
        : "+r"(msg0), "+r"(msg1), "+r"(msg2), "+r"(msg3),
        "+r"(info), "+r"(destptr)
        : "r"(scno) MCS_PARAM
        : "memory"
    );
    *out_info = info;
    *out_badge = destptr;
    *in_out_mr0 = msg0;
    *in_out_mr1 = msg1;
    *in_out_mr2 = msg2;
    *in_out_mr3 = msg3;
}

static inline seL4_Error seL4_BenchmarkResetLog(void)
{
    seL4_Word unused0 = 0;
    seL4_Word unused1 = 0;
    seL4_Word unused2 = 0;
    seL4_Word unused3 = 0;
    seL4_Word unused4 = 0;

    seL4_Word ret;
    arm_sys_send_recv(seL4_SysBenchmarkResetLog, 0, &ret, 0, &unused0, &unused1, &unused2, &unused3, &unused4, 0);

    return (seL4_Error) ret;
}

static inline seL4_Word seL4_BenchmarkFinalizeLog(void)
{
    seL4_Word unused0 = 0;
    seL4_Word unused1 = 0;
    seL4_Word unused2 = 0;
    seL4_Word unused3 = 0;
    seL4_Word unused4 = 0;

    seL4_Word index_ret;
    arm_sys_send_recv(seL4_SysBenchmarkFinalizeLog, 0, &index_ret, 0, &unused0, &unused1, &unused2, &unused3, &unused4, 0);

    return (seL4_Word) index_ret;
}

#endif //_SEL4_SUPPORT_H
