/****************************
 *
 * FILE : io.c
 *
 * DESC : process entry point for I/O process,
 *        including simulation support
 *
 * DATE : 20.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: io.c,v $
 *  Revision 1.9  2003/10/09 21:50:04  tmike
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
 *  Revision 1.8  2003/10/07 22:49:05  tmike
 *  added SM_COMMAND_MSG to airpressure and estop
 *
 *  Revision 1.7  2003/09/30 21:14:59  tmike
 *  minor cleanup
 *
 *  Revision 1.6  2003/09/30 05:01:18  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
 *
 *  Revision 1.5  2003/09/29 18:53:51  tmike
 *  TCL sendcmd working for IO_CMD_MESSAGE
 *  
 *  Revision 1.4  2003/09/27 22:46:48  tmike
 *  TCL process added with rt and showsched commands
 *  add State Machine process
 *  add I/O monitor
 *  pgrabber process coded, not tested
 *  
 *  Revision 1.3  2003/09/23 23:59:02  tmike
 *  Version with working SHM and MSG for IPC
 *      OS Service Layers : message, memory, dump
 *      APP Tasks : main, io, pgrabber
 *  
 *  Revision 1.2  2003/09/23 18:32:55  tmike
 *  SHM working, attempting to verify MSG
 *  
 *  Revision 1.1  2003/09/20 19:14:29  tmike
 *  Added partstamper project, first check-in,
 *  initial project structure modeled by process nodes
 *  
 *  
 *
 ****************************/

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** INCLUDE FILES *****/
/*************************/

#include <stdio.h>

#include "message.h"
#include "process.h"
#include "memory.h"
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

static char *cvsid = "$Id: io.c,v 1.9 2003/10/09 21:50:04 tmike Exp $";

PROC_LIST this_proc;

/***** EXTERNAL REFERNECES *****/
/*******************************/

extern void h_io_turn_off( GENERIC_MSG *);
extern void h_io_turn_on( GENERIC_MSG *);
extern void h_io_value_of( GENERIC_MSG *);
extern void h_io_report( GENERIC_MSG *);
extern void h_io_reset( GENERIC_MSG *);

/***** CODE STARTS HERE *****/
/****************************/

/////////////////////
static void io_init()
{
    int i;
    GENERIC_MSG *gm;

    // identify myself
    //////////////////
    this_proc = PROC_io;

    // reset I/O system
    ///////////////////
    h_io_reset( NULL );

    // init SHM module for this process
    ///////////////////////////////////
    ps_shm_init();
    memcpy(proc_table,mem_table[SHM_process].shm_ptr,PROC_SIZE);
    ps_msg_init();

    // wait for continue command
    ////////////////////////////
    gm = msg_receive( IO_COMMAND_MSG, __FILE__, __LINE__ );
    if (gm == NULL)
    {

#ifdef LOG_ENABLE
        Log(LOG_ERROR,"[%d][%s][%d].msg_receive return null.",
                this_proc, __FILE__, __LINE__);
        Log(LOG_ERROR,"[%d].I/O process unable to complete initialization.",
                this_proc);
#endif

        exit(1);

    } else
        generic_msg_free( gm, __FILE__, __LINE__ );

    // all processes are created, proc IDs are in table
    ///////////////////////////////////////////////////
    memcpy(proc_table,mem_table[SHM_process].shm_ptr,PROC_SIZE);

}

/////////////////////////////////
int main( int argc, char *argv[])
{
    int     i, err;
    IOCM  *iocm;
    MESSAGE_PACKET  *mp;
    GENERIC_MSG *gm;

#ifdef LOG_ENABLE
    Log(LOG_TRACE,"**PROCESS CREATION**[%s][%d].io(this_proc[%d]).",
            __FILE__, __LINE__, PROC_io);
#endif

    // initialize I/O subsystem
    ///////////////////////////
    io_init();

    for (;;) {
        // receive command packet
        /////////////////////////
        gm = msg_receive(IO_COMMAND_MSG);
        if (gm == NULL)
        {
            Log(LOG_ERROR,"[%d][%s][%d].msg_receive return null.",
                this_proc, __FILE__, __LINE__);
            continue;
        } else {
            mp = (MESSAGE_PACKET *) &(gm->mp);
            iocm = (IOCM *) &(gm->data[0]);
            iocm->value = -1;
            for (i=0;i<IO_LAST;i++)
                iocm->report[i] = -1;
        }

        switch (iocm->command) {

            case io_turn_off:
                Log(LOG_MESSAGE,"[%d][%s][%d]..io_turn_off(bit[%d]) cmd msg.",
                        this_proc, __FILE__,__LINE__,iocm->bit);
                h_io_turn_off( gm );
                break;

            case io_turn_on:
                Log(LOG_MESSAGE,"[%d][%s][%d]..io_turn_on(bit[%d]) cmd msg.",
                        this_proc, __FILE__,__LINE__,iocm->bit);
                h_io_turn_on( gm );
                break;

            case io_value_of:
                Log(LOG_MESSAGE,"[%d][%s][%d]..io_value_of(bit[%d]) cmd msg.",
                        this_proc, __FILE__,__LINE__,iocm->bit);
                h_io_value_of( gm );
                break;

            case io_report:
                Log(LOG_MESSAGE,"[%d][%s][%d]..io_report() cmd msg.",
                        this_proc, __FILE__,__LINE__);
                h_io_report( gm );
                break;

            case io_reset:
                Log(LOG_MESSAGE,"[%d][%s][%d]..io_reset() cmd msg.",
                        this_proc, __FILE__,__LINE__);
                h_io_reset( gm );
                break;

            default:
                Log(LOG_DEBUG,"[%d][%s][%d]..default on iocm->command[%d].",
                        this_proc, __FILE__, __LINE__, iocm->command);
                mp->error = MP_BADCOMMAND;
                break;
        }

        // always provide I/O snapshot
        //////////////////////////////
        h_io_report( gm );

        msg_reply( gm );
        generic_msg_free( gm );
    }

}

/***** END OF FILE HERE *****/
/****************************/

