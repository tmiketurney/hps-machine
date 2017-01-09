/****************************
 *
 * FILE : main.c
 *
 * DESC : main process entry point for partstamper
 *
 * DATE : 20.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: main.c,v $
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
 *  Revision 1.8  2003/10/09 22:57:39  tmike
 *  Log file generated, with single Tcl command
 *  to StateMachine process : return state
 *
 *  Revision 1.7  2003/10/09 21:50:04  tmike
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
 *  Revision 1.6  2003/10/07 22:50:02  tmike
 *  added process creation
 *  collapsed message table into process table
 *  general cleanup
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
 *  Revision 1.3  2003/09/27 22:46:48  tmike
 *  TCL process added with rt and showsched commands
 *  add State Machine process
 *  add I/O monitor
 *  pgrabber process coded, not tested
 *  
 *  Revision 1.2  2003/09/23 23:59:02  tmike
 *  Version with working SHM and MSG for IPC
 *      OS Service Layers : message, memory, dump
 *      APP Tasks : main, io, pgrabber
 *  
 *  Revision 1.1  2003/09/23 18:32:55  tmike
 *  SHM working, attempting to verify MSG
 *  
 *
 ****************************/

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** INCLUDE FILES *****/
/*************************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>

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

static char *cvsid = "$Id: main.c,v 1.10 2003/10/10 20:57:09 tmike Exp $";

PROCESS_TABLE proc_table_reset[LAST_PROC] = {
 { -1, -1, (PROC_pgrabber+QUE_OFFSET), QUE_CREATE, sizeof(GENERIC_MSG), PS_COMMAND_MSG, "./pgrabber" },
 { -1, -1, (PROC_ppusher+QUE_OFFSET), QUE_CREATE, sizeof(GENERIC_MSG), PS_COMMAND_MSG, "./ppusher" },
 { -1, -1, (PROC_pstamper+QUE_OFFSET), QUE_CREATE, sizeof(GENERIC_MSG), PS_COMMAND_MSG, "./pstamper" },
 { -1, -1, (PROC_smachine+QUE_OFFSET), QUE_CREATE, sizeof(GENERIC_MSG), SM_COMMAND_MSG, "./smachine" },
 { -1, -1, (PROC_ui+QUE_OFFSET), QUE_CREATE, sizeof(GENERIC_MSG), UI_COMMAND_MSG, "./ui" },
 { -1, -1, (PROC_tcl+QUE_OFFSET), QUE_CREATE, sizeof(GENERIC_MSG), PS_GENERIC_MSG, "./pstcl" },
 { -1, -1, (PROC_main+QUE_OFFSET), QUE_CREATE, sizeof(GENERIC_MSG), PS_GENERIC_MSG, "./main" }
};

PROC_LIST this_proc;

// local pointers for SHM tables
////////////////////////////////
LogFlags *lfptr;
int *ioptr;

/***** EXTERNAL REFERENCES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

/////////////////////
static void ps_kick()
{
    GENERIC_MSG *gm;
    int ret_value;

#ifdef LOG_ENABLE
    Log(LOG_INIT,"[%s].ENTERING ps_kick().", Log_proc_list[this_proc]);
#endif

    gm = generic_msg_init( PS_GENERIC_MSG, __FILE__, __LINE__ );

    if ( gm == NULL )
    {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].generic_msg_init returned NULL.goodbye.",
        Log_proc_list[this_proc], __FILE__, __LINE__);
#endif

        exit( 1 );

    }

    // kick pgrabber task
    /////////////////////
    gm->mp.receiver = PROC_pgrabber;
    ret_value = msg_send( gm, __FILE__, __LINE__ );

    // kick ppusher task
    ////////////////////
    gm->mp.receiver = PROC_ppusher;
    ret_value = msg_send( gm, __FILE__, __LINE__ );

    // kick pstamper task
    /////////////////////
    gm->mp.receiver = PROC_pstamper;
    ret_value = msg_send( gm, __FILE__, __LINE__ );

    // kick smachine task
    /////////////////////
    gm->mp.receiver = PROC_smachine;
    ret_value = msg_send( gm, __FILE__, __LINE__ );

#if 0
    // kick ui task
    ///////////////
    gm->mp.receiver = PROC_ui;
    ret_value = msg_send( gm, __FILE__, __LINE__ );
#endif

#ifdef LOG_ENABLE
    Log(LOG_INIT,"[%s].EXITING ps_kick().", Log_proc_list[this_proc]);
#endif

}

//////////////////////
static void ps_spawn()
{
    pid_t   pid;
    int     i;
    PROCESS_TABLE *pt;

    for (i=0; i<PROC_ui; i++)
    {
        // adjust PROCESS_TABLE pointer
        ///////////////////////////////
        pt = &(proc_table[i]);

        // spawn new task
        /////////////////
        if ( (pid = fork()) < 0 )
        {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s].fork on task[i] returned[%d].goodbye.",
            Log_proc_list[this_proc], i, pid);
#endif

            exit(1);

        } else if (pid == 0) {
            // child process, exec new task
            ///////////////////////////////
            pid = execlp( pt->command, pt->command, (char *) 0);

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s].execlp[%s] failed, errno[%s].goodbye.",
        Log_proc_list[this_proc], pt->command, strerror(errno));
#endif

            exit(1);

        } else {
            // back in the parent process, save the PID
            ///////////////////////////////////////////
            proc_table[i].proc_id = pid;
        }
    }

#ifdef LOG_ENABLE
    Log(LOG_INIT,"[%s].EXITING ps_spawn().", Log_proc_list[this_proc]);
#endif

}

/////////////////////////////////
int main( int argc, char *argv[])
{

#ifdef LOG_ENABLE
    Log(LOG_TRACE,"**PROCESS CREATION**[%s][%d].main(this_proc[%d]).",
            __FILE__, __LINE__, PROC_main);
#endif

    // identify myself
    //////////////////
    this_proc = PROC_main;

    // main process pre-initialization
    //////////////////////////////////
    memcpy(proc_table,proc_table_reset,PROC_SIZE);
    memcpy(io_table,io_table_reset,IO_SIZE);
    memcpy(&logflags,&logflags_reset,LOG_SIZE);

    // initialize shared memory
    ///////////////////////////
    ps_shm_init();

    // initialize message queues
    ////////////////////////////
    ps_msg_init();

    // spawn threads / processes
    ////////////////////////////
    ps_spawn();

    // copy process table info to shm
    /////////////////////////////////
    memcpy(mem_table[SHM_process].shm_ptr,proc_table,PROC_SIZE);
    memcpy(mem_table[SHM_io].shm_ptr,io_table,IO_SIZE);
    memcpy(mem_table[SHM_log].shm_ptr,&logflags,LOG_SIZE);

    // kick the processes to continue init
    //////////////////////////////////////
    ps_kick();

    // wait on kill signal
    //////////////////////
    for (;;) {
        sleep(5);
    }

}

/***** END OF FILE HERE *****/
/****************************/

