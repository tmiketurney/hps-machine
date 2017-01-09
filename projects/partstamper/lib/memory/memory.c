/****************************
 *
 * FILE : memory.c
 *
 * DESC : shared memory library
 *
 * DATE : 22.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: memory.c,v $
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
 *  Revision 1.4  2003/10/07 22:48:34  tmike
 *  Removed message table
 *
 *  Revision 1.3  2003/09/30 05:01:18  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
 *
 *  Revision 1.2  2003/09/23 23:59:01  tmike
 *  Version with working SHM and MSG for IPC
 *      OS Service Layers : message, memory, dump
 *      APP Tasks : main, io, pgrabber
 *  
 *  Revision 1.1  2003/09/23 18:32:06  tmike
 *  SHM working, attempting to verify MSG
 *  
 *
 ****************************/

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** INCLUDE FILES *****/
/*************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>

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

static char *cvsid = "$Id: memory.c,v 1.7 2003/10/10 20:57:09 tmike Exp $";

MEMORY_TABLE mem_table[LAST_SHM] = {
 { SHM_process, -1, SHM_process+SHM_OFFSET, PROC_SIZE, SHM_FLAGS, SHM_AT_FLAGS, NULL },
 { SHM_log,     -1, SHM_log+SHM_OFFSET,     LOG_SIZE,  SHM_FLAGS, SHM_AT_FLAGS, NULL },
 { SHM_io,      -1, SHM_io+SHM_OFFSET,      IO_SIZE,   SHM_FLAGS, SHM_AT_FLAGS, NULL }
};

PROCESS_TABLE proc_table[LAST_PROC];
int *ioptr;
LogFlags *lfptr;

/***** EXTERNAL REFERENCES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

//////////////////
void ps_shm_init()
{
    int i, shm_key, shm_size, shm_flags, shm_id;
    void *shm_ptr;
    MEMORY_TABLE *mt;

    for (i=0; i<LAST_SHM; i++) {
        shm_size  = mem_table[i].shm_size;
        shm_key   = mem_table[i].shm_key;
        shm_flags = mem_table[i].shm_flags;

        shm_id = shmget(shm_key, shm_size, shm_flags);
        if (shm_id == -1)
        {
            perror("ps_shm_init");
            exit( 2 );
        }
        else {
            mem_table[i].shm_id = shm_id;

            shm_flags = mem_table[i].shm_at_flags;
            shm_ptr = shmat(shm_id, NULL, shm_flags);
            if (shm_ptr == (void *)-1)
            {
                perror("ps_shm_init");
                exit( 3 );
            }
            else
                mem_table[i].shm_ptr = shm_ptr;
        }

        mt = (MEMORY_TABLE *) &(mem_table[i]);

#ifdef LOG_ENABLE
        Log(LOG_INIT,"[%s]..shm_type[%d].shm_id[%x].shm_key[%x].shm_size[%d].shm_flags[%x].shm_ptr[%x].",
            Log_proc_list[this_proc], mt->shm_type, mt->shm_id, mt->shm_key, mt->shm_size, mt->shm_flags, mt->shm_ptr);
#endif

    }

    // initialize this process SHM pointers
    ///////////////////////////////////////
    lfptr = mem_table[SHM_log].shm_ptr;
    ioptr = mem_table[SHM_io].shm_ptr;
}

/***** END OF FILE HERE *****/
/****************************/

