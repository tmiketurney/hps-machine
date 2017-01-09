/****************************
 *
 * FILE : process.h
 *
 * DESC : system-wide process info
 *
 * DATE : 22.September.2003
 *
 * CVS : $Id: process.h,v 1.6 2003/10/10 05:44:41 tmike Exp $
 *
 *  $Log: process.h,v $
 *  Revision 1.6  2003/10/10 05:44:41  tmike
 *  State Machine appears functional (need to detail review Log file)
 *      I/O task removed
 *      I/O table in SHM
 *      LogFlags in SHM
 *
 *  Revision 1.5  2003/10/09 21:50:04  tmike
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
 *  Revision 1.4  2003/10/07 22:46:50  tmike
 *  General cleanup, consolidate files, etc.
 *
 *  Revision 1.3  2003/09/30 05:01:10  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
 *
 *  Revision 1.2  2003/09/27 22:46:48  tmike
 *  TCL process added with rt and showsched commands
 *  add State Machine process
 *  add I/O monitor
 *  pgrabber process coded, not tested
 *  
 *  Revision 1.1  2003/09/23 23:59:01  tmike
 *  Version with working SHM and MSG for IPC
 *      OS Service Layers : message, memory, dump
 *      APP Tasks : main, io, pgrabber
 *  
 *
 ****************************/

#ifndef _PROCESS_H
#define _PROCESS_H

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** MACROS *****/
/******************/

// size of char buffer for command line
///////////////////////////////////////
#define CMD_SIZE 0x10

/***** TYPEDEFS *****/
/********************/

typedef enum {
    PROC_NONE = -1,
    PROC_pgrabber = 0,
    PROC_ppusher  = 1,
    PROC_pstamper = 2,
    PROC_smachine = 3,
    PROC_ui       = 4,
    PROC_tcl      = 5,
    PROC_main     = 6,
    LAST_PROC
} PROC_LIST;

typedef struct {
    int         proc_id;            // system process id
    int         que_id;             // input message queue ID
    int         que_key;            // input message queue key
    int         que_flags;          // input message queue flags
    int         msg_size;           // size of input message
    MSG_LIST    message;            // type of input message
    char        command[CMD_SIZE];  // command line to exec
} PROCESS_TABLE;

#define PROC_SIZE (sizeof(PROCESS_TABLE)*LAST_PROC)

/***** EXTERN REFERENCES *****/
/*****************************/

extern PROCESS_TABLE proc_table[LAST_PROC];

extern PROC_LIST this_proc;

#endif

/***** END OF FILE HERE *****/
/****************************/

