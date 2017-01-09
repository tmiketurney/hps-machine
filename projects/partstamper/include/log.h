/****************************
 *
 * FILE : log.h
 *
 * DESC : application log library public interface
 *
 * DATE : 26.September.2003
 *
 * CVS : $Id: log.h,v 1.7 2003/10/10 20:57:09 tmike Exp $
 *
 *  $Log: log.h,v $
 *  Revision 1.7  2003/10/10 20:57:09  tmike
 *  First Project RELEASE!!
 *  Working:
 *      SHM and MSG
 *      Processes : smachine, pgrabber, ppusher, pstamper, pstcl, main
 *      Monitor   : io
 *      Simulator : io
 *
 *  ToDo:
 *      pstcl commands into I/O Monitor
 *      add timebase
 *          protect I/O Monitor with SEM
 *          timeout on wait_until
 *          timeout on msg_receive
 *      handle return from msg_send and msg_reply
 *
 *  Revision 1.6  2003/10/09 22:57:39  tmike
 *  Log file generated, with single Tcl command
 *  to StateMachine process : return state
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
 *  Revision 1.2  2003/09/29 14:17:02  tmike
 *  added sendcmd to pstcl tester
 *  
 *  Revision 1.1  2003/09/27 22:46:48  tmike
 *  TCL process added with rt and showsched commands
 *  add State Machine process
 *  add I/O monitor
 *  pgrabber process coded, not tested
 *  
 *
 ****************************/

#ifndef _LOG_H
#define _LOG_H

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

#define LOG_ENABLE      // comment this line to disable Log feaure

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

typedef struct {
    int trace_flag;     // flag to control LOG_TRACE
    int msg_flag;       // flag to control LOG_MSG
    int simulate_flag;  // flag to control LOG_SIMULATE
    int library_flag;   // flag to control LOG_LIBRARY
    int smachine_flag;  // flag to control LOG_SMACHINE
    int pgrabber_flag;  // flag to control LOG_PGRABBER
    int ppusher_flag;   // flag to control LOG_PPUSHER
    int pstamper_flag;  // flag to control LOG_PSTAMPER
} LogFlags;

#define LOG_SIZE (sizeof(LogFlags))

// when adding to this list, make sure you add
// to switch statement at start of Log function
///////////////////////////////////////////////
typedef enum {
    LOG_ERROR,          // 00 system or application error
    LOG_DEBUG,          // 01 default case on switch
    LOG_TRACE,          // 02 function entry/exit
    LOG_MSG,            // 03 MSG handling
    LOG_SIMULATE,       // 04 I/O simulator
    LOG_LIBRARY,        // 05 library log
    LOG_SMACHINE,       // 06 State Machine process log
    LOG_PGRABBER,       // 07 Part Grabber  process log
    LOG_PPUSHER,        // 08 Part Pusher   process log
    LOG_PSTAMPER,       // 09 Part Stamper  process log
    LOG_INIT,           // 10 Initialization log
    LOG_LAST
} LOG_LIST;

/***** EXTERN REFERENCES *****/
/*****************************/

extern void Log( LOG_LIST, char *, ... );

extern LogFlags logflags;
extern LogFlags logflags_reset;
extern LogFlags *lfptr;

// enum to character string mapping tables
//////////////////////////////////////////

extern char *Log_msg_list[];
extern char *Log_shm_list[];
extern char *Log_log_list[];
extern char *Log_proc_list[];

extern char *Log_mp_error[];

extern char *Log_io_cmd_list[];
extern char *Log_io_bit_list[];

extern char *Log_sm_state[];
extern char *Log_sm_cmd_list[];
extern char *Log_ps_cmd_list[];
extern char *Log_ui_cmd_list[];

#endif

/***** END OF FILE HERE *****/
/****************************/

