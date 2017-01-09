/****************************
 *
 * FILE : memory.h
 *
 * DESC : system-wide shm info
 *
 * DATE : 22.September.2003
 *
 * CVS : $Id: memory.h,v 1.5 2003/10/10 05:44:41 tmike Exp $
 *
 *  $Log: memory.h,v $
 *  Revision 1.5  2003/10/10 05:44:41  tmike
 *  State Machine appears functional (need to detail review Log file)
 *      I/O task removed
 *      I/O table in SHM
 *      LogFlags in SHM
 *
 *  Revision 1.4  2003/10/09 21:50:04  tmike
 *  Everything compiles, nothing has been tested:
 *      Log file
 *          ascii strings instead of enum values
 *          __FILE__ and __LINE__ from caller
 *          organized, with rules
 *          CONDITIONAL COMPILE and runtime control
 *          runtime control in SHM
 *      PROC_io removed
 *      I/O monitor calls stubbed out
 *      msg_receive change to IPC_WAIT
 *
 *  Revision 1.3  2003/10/07 22:46:50  tmike
 *  General cleanup, consolidate files, etc.
 *
 *  Revision 1.2  2003/09/30 05:01:10  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
 *
 *  Revision 1.1  2003/09/23 23:59:01  tmike
 *  Version with working SHM and MSG for IPC
 *      OS Service Layers : message, memory, dump
 *      APP Tasks : main, io, pgrabber
 *  
 *
 ****************************/

#ifndef _MEMORY_H
#define _MEMORY_H

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** MACROS *****/
/******************/

#define SHM_OFFSET 0x3000
#define SHM_FLAGS (IPC_CREAT)
#define SHM_AT_FLAGS 0

/***** TYPEDEFS *****/
/********************/

typedef enum {
    SHM_process,    // 00
    SHM_log,        // 01
    SHM_io,         // 02
    LAST_SHM
} SHM_LIST;

typedef struct {
    SHM_LIST    shm_type;
    int         shm_id;
    int         shm_key;
    int         shm_size;
    int         shm_flags;
    int         shm_at_flags;
    void        *shm_ptr;
} MEMORY_TABLE;

/***** EXTERN REFERENCES *****/
/*****************************/

extern void ps_shm_init();

extern MEMORY_TABLE mem_table[LAST_SHM];

#endif

/***** END OF FILE HERE *****/
/****************************/

