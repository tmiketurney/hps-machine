/****************************
 *
 * FILE : smachine.h
 *
 * DESC : State Machine process public interface
 *
 * DATE : 26.September.2003
 *
 * CVS : $Id: smachine.h,v 1.7 2003/10/10 20:57:09 tmike Exp $
 *
 *  $Log: smachine.h,v $
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
 *  Revision 1.3  2003/09/30 21:14:52  tmike
 *  minor cleanup
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
 *  Revision 1.1  2003/09/27 22:46:48  tmike
 *  TCL process added with rt and showsched commands
 *  add State Machine process
 *  add I/O monitor
 *  pgrabber process coded, not tested
 *  
 *
 ****************************/

#ifndef _SMACHINE_H
#define _SMACHINE_H

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

typedef enum {
    state_quiet,        // 00
    state_ehardware,    // 01
    state_grab,         // 02
    state_push,         // 03
    state_stamp,        // 04
    state_stamp_grab,   // 05
    state_last,         // 06
    state_nostate = -1
} SM_STATE;

typedef enum {
    sm_start,           // 00
    sm_stop,            // 01
    sm_return_state,    // 02
    sm_reset,           // 03
    sm_io_value_change, // 04
    sm_cmd_last         // 05
} SM_CMD_LIST;

// Incoming Command Message
///////////////////////////
typedef struct {
    SM_CMD_LIST command;
    int         continuous;
    int         state;
    IO_BIT_LIST bit;
    int         value;
} SMCM;

/***** EXTERN REFERENCES *****/
/*****************************/

extern int sm_state;

extern void h_state_quiet( GENERIC_MSG * );
extern void h_state_ehardware( GENERIC_MSG * );
extern void h_state_grab( GENERIC_MSG * );
extern void h_state_push( GENERIC_MSG * );
extern void h_state_stamp( GENERIC_MSG * );
extern void h_state_stamp_grab( GENERIC_MSG * );

#endif

/***** END OF FILE HERE *****/
/****************************/

