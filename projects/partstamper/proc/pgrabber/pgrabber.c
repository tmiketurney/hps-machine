/****************************
 *
 * FILE : pgrabber.c
 *
 * DESC : Part Grabber process entry point
 *
 * DATE : 23.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: pgrabber.c,v $
 *  Revision 1.10  2003/10/10 20:57:09  tmike
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
 *  Revision 1.9  2003/10/10 05:44:41  tmike
 *  State Machine appears functional (need to detail review Log file)
 *      I/O task removed
 *      I/O table in SHM
 *      LogFlags in SHM
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
 *  Revision 1.7  2003/10/07 22:50:42  tmike
 *  Consolidated all machine control messages
 *  into single typedef
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
#include "pstamper.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static char *cvsid = "$Id: pgrabber.c,v 1.10 2003/10/10 20:57:09 tmike Exp $";

PROC_LIST this_proc;
LogFlags *lfptr;
int *ioptr;

int pg_state;

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

////////////////////
void pgrabber_init()
{
    GENERIC_MSG *gm;

    // identify myself
    //////////////////
    this_proc = PROC_pgrabber;
    pg_state = -1;

    // init SHM module for this process
    ///////////////////////////////////
    ps_shm_init();
    memcpy(proc_table,mem_table[SHM_process].shm_ptr,PROC_SIZE);
    ps_msg_init();

    // wait for continue command
    ////////////////////////////
    gm = msg_receive( PS_COMMAND_MSG, __FILE__, __LINE__ );
    if (gm == NULL)
    {

#ifdef LOG_ENABLE
        Log(LOG_ERROR,"[%s][%s][%d].msg_receive return null.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
        Log(LOG_ERROR,"[%s].Part Grabber process unable to complete initialization.",
            Log_proc_list[this_proc]);
#endif

        exit(1);

    } else
        generic_msg_free( gm, __FILE__, __LINE__ );

    // all processes are created, proc IDs are in table
    ///////////////////////////////////////////////////
    memcpy(proc_table,mem_table[SHM_process].shm_ptr,PROC_SIZE);

#ifdef LOG_ENABLE
    Log(LOG_INIT,"[%s].EXITING pgrabber_init().",Log_proc_list[this_proc]);
#endif

}

//////////////////////////////////////////////
static void h_ps_grab_part( GENERIC_MSG * gm )
{
    int io_value;

#ifdef LOG_ENABLE
    if (lfptr->pgrabber_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING h_ps_grab_part( gm[%x] ).",
                Log_proc_list[this_proc], gm);
    }
#endif

    pg_state = 0;

    io_value = wait_until( cylBretracted, 1 );

    pg_state++;

    io_value = turn_on( cylAsolenoid );

    pg_state++;

    io_value = wait_until( cylAextended, 1 );

    pg_state++;

    io_value = turn_off( cylAsolenoid );

    pg_state++;

    io_value = wait_until( cylAretracted, 1 );

#ifdef LOG_ENABLE
    if (lfptr->pgrabber_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING h_ps_grab_part( gm[%x] ).",
                Log_proc_list[this_proc], gm);
    }
#endif

}

////////////////////////////////////////////////
static void h_ps_return_state( GENERIC_MSG *gm )
{
#ifdef LOG_ENABLE
    if (lfptr->pgrabber_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING h_ps_return_state( gm[%x] ).",
                Log_proc_list[this_proc], gm);
    }
#endif
}

/////////////////////////////////////////
static void h_ps_reset( GENERIC_MSG *gm )
{
#ifdef LOG_ENABLE
    if (lfptr->pgrabber_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING h_ps_reset( gm[%x] ).",
                Log_proc_list[this_proc], gm);
    }
#endif
}


/////////////////////////////////
int main( int argc, char *argv[])
{
    int     ret_value;
    PSCM  *pscm;
    GENERIC_MSG *gm;

#ifdef LOG_ENABLE
        Log(LOG_TRACE,"**PROCESS CREATION**[%s][%d].pgrabber(this_proc[%d]).",
            __FILE__, __LINE__, PROC_pgrabber);
#endif

    // initialize Part Grabber process
    //////////////////////////////////
    pgrabber_init();

    for (;;) {
        // wait on command queue
        ////////////////////////
        gm = msg_receive( PS_COMMAND_MSG, __FILE__, __LINE__ );

        if (gm == NULL)
        {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive return NULL",
        Log_proc_list[this_proc], __FILE__, __LINE__);
#endif

            continue;

        } else {
            pscm = (PSCM *) &(gm->data[0]);
        }

        switch (pscm->command)
        {
            case ps_grab_part :
                h_ps_grab_part( gm );
                break;

            case ps_return_state :
                h_ps_return_state( gm );
                break;

            case ps_reset :
                h_ps_reset( gm );
                break;

            default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on command(%d).",
            Log_proc_list[this_proc], __FILE__, __LINE__, pscm->command);
#endif

                gm->mp.error = MP_BADCOMMAND;
                break;
        }

        ret_value = msg_reply( gm, __FILE__, __LINE__ );
        generic_msg_free( gm, __FILE__, __LINE__ );
    }
}

/***** END OF FILE HERE *****/
/****************************/

