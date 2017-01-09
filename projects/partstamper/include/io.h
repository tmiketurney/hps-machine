/****************************
 *
 * FILE : io.h
 *
 * DESC : I/O process public interface
 *
 * DATE : 20.September.2003
 *
 * CVS : $Id: io.h,v 1.7 2003/10/10 20:57:09 tmike Exp $
 *
 *  $Log: io.h,v $
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
 *  Revision 1.6  2003/10/10 05:44:40  tmike
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

#ifndef _IO_H
#define _IO_H

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

// I/O bit list
///////////////
typedef enum {
    cylAretracted,      // 00
    cylAextended,       // 01
    cylAsolenoid,       // 02
    cylBretracted,      // 03
    cylBextended,       // 04
    cylBsolenoid,       // 05
    cylCretracted,      // 06
    cylCextended,       // 07
    cylCsolenoid,       // 08
    airpressure,        // 09
    estop,              // 10
    IO_LAST
} IO_BIT_LIST;

// I/O Command list
///////////////////
typedef enum {
    io_turn_off,        // 00
    io_turn_on,         // 01
    io_value_of,        // 02
    io_report,          // 03
    io_reset,           // 04
    io_cmd_last         // 05
} IO_CMD_LIST;

// I/O activator function pointer
/////////////////////////////////
typedef int (*PFI)( IO_CMD_LIST, IO_BIT_LIST );

/***** EXTERN REFERENCES *****/
/*****************************/

extern int turn_off( IO_BIT_LIST );
extern int turn_on( IO_BIT_LIST );
extern int value_of( IO_BIT_LIST );
extern int wait_until( IO_BIT_LIST, int );
extern void c_io_report( IO_BIT_LIST * );
extern void c_io_reset();

extern int io_table[];
extern int io_table_reset[];

extern int io_simulate( IO_CMD_LIST, IO_BIT_LIST );

extern int *ioptr;

#define IO_SIZE (sizeof(int)*IO_LAST)

#endif

/***** END OF FILE HERE *****/
/****************************/

