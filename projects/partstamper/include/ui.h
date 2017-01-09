/****************************
 *
 * FILE : ui.h
 *
 * DESC : User Interface process public interface
 *
 * DATE : 28.September.2003
 *
 * CVS : $Id: ui.h,v 1.5 2003/10/10 20:57:09 tmike Exp $
 *
 *  $Log: ui.h,v $
 *  Revision 1.5  2003/10/10 20:57:09  tmike
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
 *  Revision 1.1  2003/09/29 14:17:02  tmike
 *  added sendcmd to pstcl tester
 *  
 *
 ****************************/

#ifndef _UI_H
#define _UI_H

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

typedef enum {
    ui_reset,        // 00
    ui_return_state, // 01
    ui_cmd_last      // 02
} UI_CMD_LIST;

// Incoming Command Message
///////////////////////////
typedef struct {
    UI_CMD_LIST command;
    int         state;
} UICM;

/***** EXTERN REFERENCES *****/
/*****************************/

#endif

/***** END OF FILE HERE *****/
/****************************/

