/****************************
 *
 * FILE : message.c
 *
 * DESC : IPC message library
 *
 * DATE : 20.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: message.c,v $
 *  Revision 1.9  2003/10/10 20:57:09  tmike
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
 *  Revision 1.8  2003/10/09 21:50:04  tmike
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
 *  Revision 1.7  2003/10/07 22:48:44  tmike
 *  Removed message table, general cleanup
 *
 *  Revision 1.6  2003/09/30 21:14:59  tmike
 *  minor cleanup
 *
 *  Revision 1.5  2003/09/30 05:01:18  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
 *
 *  Revision 1.4  2003/09/29 18:53:51  tmike
 *  TCL sendcmd working for IO_CMD_MESSAGE
 *  
 *  Revision 1.3  2003/09/29 14:17:09  tmike
 *  added sendcmd to pstcl tester
 *  
 *  Revision 1.2  2003/09/26 21:55:27  tmike
 *  merged dump library with new log library
 *  
 *  Revision 1.1  2003/09/23 23:59:02  tmike
 *  Version with working SHM and MSG for IPC
 *      OS Service Layers : message, memory, dump
 *      APP Tasks : main, io, pgrabber
 *  
 *
 ****************************/

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** INCLUDE FILES *****/
/*************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>

#include "message.h"
#include "process.h"
#include "log.h"

#include "io.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static msgbuf MsgBuf;

static char *cvsid = "$Id: message.c,v 1.9 2003/10/10 20:57:09 tmike Exp $";

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

//////////////////
void ps_msg_init()
{
    int i, que_key, que_flags, que_proc_flags, que_id;

    if (this_proc == PROC_main)
        que_proc_flags = IPC_CREAT;
    else
        que_proc_flags = 0;

    for (i=0; i<LAST_PROC; i++) {

        if (proc_table[i].msg_size != -1) {
            que_key = proc_table[i].que_key;
            que_flags = que_proc_flags | proc_table[i].que_flags;
            que_id = msgget(que_key, que_flags);
            if (que_id == -1)
            {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msgget return -1, errno[%s].goodbye.",
            Log_proc_list[this_proc], __FILE__, __LINE__, strerror(errno));
#endif

                exit( 1 );

            }
            else
                proc_table[i].que_id = que_id;

#ifdef LOG_ENABLE
    Log(LOG_INIT,"[%s]..i[%d].que_id[%x].que_key[%x].que_flags[%x].",
            Log_proc_list[this_proc], i, que_id, que_key, que_flags);
#endif

        }

    }

}

//////////////////////////////////////
void packet_init( MESSAGE_PACKET *mp )
{
    mp->message = LAST_MSG;
    mp->sender = LAST_PROC;
    mp->receiver = LAST_PROC;
    mp->error = MP_NOERROR;
}

////////////////////////////////////////////////////////////////////////
GENERIC_MSG * generic_msg_init( MSG_LIST message, char *file, int line )
{
    GENERIC_MSG *gm;

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].ENTERING generic_msg_init(%d).",
                Log_proc_list[this_proc], file, line, Log_msg_list[message]);
    }
#endif

    gm = malloc(sizeof(GENERIC_MSG));
    if (gm == NULL)
    {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d]malloc return NULL, errno[%s]",
        Log_proc_list[this_proc], __FILE__, __LINE__, strerror(errno));
#endif

        return NULL;

    }

    packet_init( &(gm->mp) );
    gm->mp.message = message;

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].EXITING generic_msg_init(%d).",
                Log_proc_list[this_proc], file, line, Log_msg_list[message]);
    }
#endif

    return gm;

}

///////////////////////////////////////////////////////////////
void generic_msg_free( GENERIC_MSG * gm, char *file, int line )
{
#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].generic_msg_free(%x).",
                Log_proc_list[this_proc], file, line, gm);
    }
#endif

    free(gm);

}


////////////////////////////////////////
GENERIC_MSG * msg_receive( MSG_LIST message, char *file, int line )
{
    int msqid, msgflg, ret_value;
    msgbuf  *msgp = &MsgBuf;
    size_t  msgsz;
    long    msgtyp;
    PROCESS_TABLE *pt = &proc_table[this_proc];
    GENERIC_MSG *gm;

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].ENTERING msg_receive(%s).",
                Log_proc_list[this_proc], file, line, Log_msg_list[message]);
    }
#endif

    gm = generic_msg_init( PS_GENERIC_MSG, __FILE__, __LINE__ );
    if (gm == NULL)
    {

#ifdef LOG_ENABLE
    Log( LOG_ERROR, "[%d][%s][%d].generic_msg_init return NULL.",
        this_proc, __FILE__, __LINE__ );
#endif

        return NULL;
    }

    // initialize system call parameters
    ////////////////////////////////////
    msqid = pt->que_id;
    msgsz = pt->msg_size;
    msgtyp = 0L;
    msgflg = 0;

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_MSG,"[%s][%s][%d].msgrcv start.",
                Log_proc_list[this_proc], file, line);
    }
#endif

    ret_value = msgrcv( msqid, msgp, msgsz, msgtyp, msgflg );
    if (ret_value == -1)
    {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%d][%s][%d]msgrcv errno[%s].",
        this_proc, __FILE__, __LINE__, strerror(errno));
#endif

        generic_msg_free( gm, __FILE__, __LINE__ );
        return NULL;

    } else if (ret_value != msgsz) {

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%d][%s][%d].received[%d] != expected[%d].",
            this_proc, __FILE__, __LINE__, ret_value, msgsz);
#endif

    }

    // copy payload to GENERIC_MSG
    //////////////////////////////
    memcpy(gm,&(msgp->gm),sizeof(GENERIC_MSG));

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_MSG,"[%s][%s][%d].return from msgrcv.",
                Log_proc_list[this_proc], file, line);
    }
#endif

    // debug
    // dump_msg(gm);

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].EXITING msg_receive( %s ).[%x].",
                Log_proc_list[this_proc], file, line, Log_msg_list[message],gm);
    }
#endif

    return gm;

}

//////////////////////////////////////////////////////
int msg_send( GENERIC_MSG * gm, char *file, int line )
{
    int msqid, msgflg, ret_value;
    msgbuf  *msgp = &MsgBuf;
    size_t  msgsz;
    PROCESS_TABLE *pt = &proc_table[gm->mp.receiver];
    char          *s;

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].ENTERING msg_send( %x ).",
                Log_proc_list[this_proc], file, line, gm);
    }
#endif

    // initialize system call parameters
    ////////////////////////////////////
    msqid = pt->que_id;
    msgsz = pt->msg_size;
    msgflg = 0;

    // initialize message packet
    ////////////////////////////
    gm->mp.sender = this_proc;

    // copy message packet to payload
    /////////////////////////////////
    memset( msgp, 0, sizeof(msgbuf) );
    msgp->mtype = gm->mp.message + QUE_OFFSET;
    s = (char *)&(msgp->gm);
    memcpy(s, gm, sizeof(GENERIC_MSG));

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        dump_msg(gm);
    }
#endif

    ret_value = msgsnd( msqid, msgp, msgsz, msgflg );

    if (ret_value == -1)
    {
#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%d][%s][%d].msgsnd errno[%s].",
            this_proc, __FILE__, __LINE__, strerror(errno));
#endif
    }
    
#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].EXITING msg_send( %x ).[%d].",
                Log_proc_list[this_proc], file, line, gm, ret_value);
    }
#endif

    return ret_value;

}

///////////////////////////////////////////////////////
int msg_reply( GENERIC_MSG * gm, char *file, int line )
{
    int msqid, msgflg, ret_value;
    msgbuf  *msgp = &MsgBuf;
    size_t  msgsz;
    PROCESS_TABLE *pt = &proc_table[gm->mp.sender];
    char          *s;

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].ENTERING msg_reply( %x ).",
                Log_proc_list[this_proc], file, line, gm);
    }
#endif

    // initialize system call parameters
    ////////////////////////////////////
    msqid = pt->que_id;
    msgsz = pt->msg_size;
    msgflg = 0;

    // initialize message packet
    ////////////////////////////
    gm->mp.receiver = gm->mp.sender;
    gm->mp.sender = this_proc;

    // copy message packet to payload
    /////////////////////////////////
    memset( msgp, 0, sizeof(msgbuf) );
    msgp->mtype = gm->mp.message + QUE_OFFSET;
    s = (char *)&(msgp->gm);
    memcpy(s, gm, sizeof(GENERIC_MSG));

#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        dump_msg(gm);
    }
#endif

    ret_value = msgsnd( msqid, msgp, msgsz, msgflg );

    if (ret_value == -1)
    {
#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%d][%s][%d].msgsnd errno[%s].",
            this_proc, __FILE__, __LINE__, strerror(errno));
#endif
    }
    
#ifdef LOG_ENABLE
    if (logflags.library_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d].EXITING msg_reply( %x ).[%d].",
                Log_proc_list[this_proc], file, line, gm, ret_value);
    }
#endif

    return ret_value;

}

/***** END OF FILE HERE *****/
/****************************/

