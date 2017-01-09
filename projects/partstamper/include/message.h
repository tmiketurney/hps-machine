/****************************
 *
 * FILE : message.h
 *
 * DESC : message headers
 *
 * DATE : 20.September.2003
 *
 * CVS : $Id: message.h,v 1.7 2003/10/10 20:57:09 tmike Exp $
 *
 *  $Log: message.h,v $
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
 *  Revision 1.6  2003/10/09 21:50:04  tmike
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
 *  Revision 1.5  2003/10/07 22:46:50  tmike
 *  General cleanup, consolidate files, etc.
 *
 *  Revision 1.4  2003/09/30 05:01:10  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
 *
 *  Revision 1.3  2003/09/29 14:17:02  tmike
 *  added sendcmd to pstcl tester
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

#ifndef _MESSAGE_H
#define _MESSAGE_H

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** MACROS *****/
/******************/

#define MSGBUF_SIZE 256
#define MSGBUF_PAYLOAD_SIZE (MSGBUF_SIZE-sizeof(long))

#define QUE_OFFSET 0x2000    // que key offset
#define QUE_PERMS 0x1FF
#define QUE_CREATE (IPC_NOWAIT | QUE_PERMS)

/***** TYPEDEFS *****/
/********************/

// Message Packet Messages (mp->message)
////////////////////////////////////////
typedef enum {
    PS_COMMAND_MSG,     // 00
    SM_COMMAND_MSG,     // 01
    UI_COMMAND_MSG,     // 02
    PS_GENERIC_MSG,     // 03
    LAST_MSG            // 04
} MSG_LIST;

// Message Packet Errors (mp->error)
////////////////////////////////////
typedef enum {
    MP_NOERROR,         // 00 no error
    MP_NOMESSAGE,       // 01 no message  specified in MESSAGE_PACKET
    MP_NOSENDER,        // 02 no sender   specified in MESSAGE_PACKET
    MP_NORECEIVER,      // 03 no receiver specified in MESSAGE_PACKET
    MP_BADCOMMAND,      // 04 receiver unable to handle command
    MP_NULLRETURN,      // 05 receiver handler experienced NULL return
    MP_UNHANDLEDMSG,    // 06 receiver unable to handle message
    MP_BADSTATE,        // 07 receiver in bad state to handle message
    MP_LAST
} MP_ERROR;

// Generic Command Message Packet Header
////////////////////////////////////////
typedef struct {
    MSG_LIST  message;   // type of this message
    int  sender;    // sender process ID
    int  receiver;  // receiver process ID
    MP_ERROR  error;     // packet error flag
} MESSAGE_PACKET;

// Generic Command Message Structure
////////////////////////////////////
typedef struct {
    MESSAGE_PACKET mp;
    char           data[MSGBUF_PAYLOAD_SIZE-sizeof(MESSAGE_PACKET)];
} GENERIC_MSG;

// SYSV IPC MSG Structure
/////////////////////////
typedef struct {
    long        mtype;
    GENERIC_MSG gm;
} msgbuf;

/***** EXTERN REFERENCES *****/
/*****************************/

extern void ps_msg_init();

extern void packet_init( MESSAGE_PACKET * );

extern void generic_msg_free( GENERIC_MSG *, char *, int );
extern GENERIC_MSG * generic_msg_init( MSG_LIST, char *, int );

extern GENERIC_MSG * msg_receive( MSG_LIST, char *, int );
extern int msg_send( GENERIC_MSG *, char *, int );
extern int msg_reply( GENERIC_MSG *, char *, int );

#endif

/***** END OF FILE HERE *****/
/****************************/

