/****************************
 *
 * FILE : statemachine.c
 *
 * DESC : State Machine process entry point
 *
 * DATE : 26.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 * 	$Log: smachine.c,v $
 * 	Revision 1.8  2003/10/10 20:57:09  tmike
 * 	First Project RELEASE!!
 * 	Working:
 * 	    SHM and MSG
 * 	    Processes : smachine, pgrabber, ppusher, pstamper, pstcl, main
 * 	    Monitor   : io
 * 	    Simulator : io
 * 	
 * 	ToDo:
 * 	    pstcl commands into I/O Monitor
 * 	    add timebase
 * 	        protect I/O Monitor with SEM
 * 	        timeout on wait_until
 * 	        timeout on msg_receive
 * 	    handle return from msg_send and msg_reply
 * 	
 * 	Revision 1.7  2003/10/10 05:44:41  tmike
 * 	State Machine appears functional (need to detail review Log file)
 * 	    I/O task removed
 * 	    I/O table in SHM
 * 	    LogFlags in SHM
 * 	
 * 	Revision 1.6  2003/10/09 21:50:05  tmike
 * 	Everything compiles, nothing has been tested:
 * 	    Log file
 * 	        ascii strings instead of enum values
 * 	        __FILE__ and __LINE__ from caller
 * 	        organized, with rules
 * 	        CONDITIONAL COMPILE and runtime control
 * 	        runtime control in SHM
 * 	    PROC_io removed
 * 	    I/O monitor calls stubbed out
 * 	    msg_receive change to IPC_WAIT
 * 	
 * 	Revision 1.5  2003/10/07 22:52:41  tmike
 * 	Implemented state machine, just beginning to test
 * 	
 * 	Revision 1.4  2003/09/30 21:14:59  tmike
 * 	minor cleanup
 * 	
 * 	Revision 1.3  2003/09/30 05:01:18  tmike
 * 	Major Project Milestone
 * 	    All processes except UI coded and running
 * 	    All CMD messages except UI handled in pstcl
 * 	    Local library dependency fixed
 * 	    Add Copyright notice to all files
 * 	    expand -4 to all files
 * 	    State Machine needs coding
 * 	
 * 	Revision 1.2  2003/09/29 14:17:11  tmike
 * 	added sendcmd to pstcl tester
 * 	
 * 	Revision 1.1  2003/09/27 22:46:48  tmike
 * 	TCL process added with rt and showsched commands
 * 	add State Machine process
 * 	add I/O monitor
 * 	pgrabber process coded, not tested
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
#include "smachine.h"
#include "pstamper.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static char *cvsid = "$Id: smachine.c,v 1.8 2003/10/10 20:57:09 tmike Exp $";

int sm_state;

PROC_LIST this_proc;
LogFlags  *lfptr;
int *ioptr;

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

///////////////////////////
static void smachine_init()
{
    GENERIC_MSG *gm;

	// identify myself
	//////////////////
	this_proc = PROC_smachine;
	sm_state = state_nostate;

	// grab shm pointer to process_table
	////////////////////////////////////
	ps_shm_init();
	memcpy(proc_table,mem_table[SHM_process].shm_ptr,PROC_SIZE);
	ps_msg_init();

    // wait for continue command
    ////////////////////////////
    gm = msg_receive( SM_COMMAND_MSG, __FILE__, __LINE__ );
    if (gm == NULL)
    {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive return null.",
            Log_proc_list[this_proc], __FILE__, __LINE__);
    Log(LOG_ERROR,"[%s].State Machine process unable to complete initialization.",
            Log_proc_list[this_proc]);
#endif

        exit(1);

    } else
        generic_msg_free( gm, __FILE__, __LINE__ );

    // all processes are created, proc IDs are in table
    ///////////////////////////////////////////////////
    memcpy(proc_table,mem_table[SHM_process].shm_ptr,PROC_SIZE);

    // attend to the I/O system
    ///////////////////////////
    if ( value_of(estop) )

        sm_state = state_ehardware;

    else if ( ! value_of(airpressure) )

        sm_state = state_ehardware;

    else {
        reset_subsystems();
        sm_state = state_quiet;
    }

#ifdef LOG_ENABLE
    Log(LOG_INIT,"[%s].EXITING smachine_init().",Log_proc_list[this_proc]);
#endif

}

/////////////////////////////////
int main( int argc, char *argv[])
{
	int		ret_value;
	GENERIC_MSG	*gm;
    SMCM *smcm;

#ifdef LOG_ENABLE
	Log(LOG_TRACE,"**PROCESS CREATION**[%s][%d].smachine(this_proc[%d]).",
            __FILE__, __LINE__, PROC_smachine);
#endif

	// initialize State Machine process
	///////////////////////////////////
	smachine_init();

	for (;;) {
		// wait on command queue
		////////////////////////
		gm = msg_receive( SM_COMMAND_MSG, __FILE__, __LINE__ );

		if (gm == NULL)
		{

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive return NULL",
            Log_proc_list[this_proc], __FILE__, __LINE__);
#endif

			continue;

		} else if (gm->mp.message == SM_COMMAND_MSG) {

            smcm = (SMCM *) &(gm->data[0]);
            if (smcm->command == sm_io_value_change)
            {
                switch (smcm->bit)
                {
                    case estop :
                    case airpressure :
                        next_state_check( gm );
                        break;

                    default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on smcm->bit[%d].",
            Log_proc_list[this_proc], __FILE__, __LINE__, smcm->bit);
#endif

                        gm->mp.error = MP_UNHANDLEDMSG;
                        break;
                }

                ret_value = msg_reply( gm, __FILE__, __LINE__ );
                generic_msg_free( gm, __FILE__, __LINE__ );
                continue;
            }
        }

		switch (sm_state)
		{
            case state_ehardware :
                h_state_ehardware( gm );
				break;

            case state_quiet :
                h_state_quiet( gm );
				break;

            case state_grab :
                h_state_grab( gm );
				break;

            case state_push :
                h_state_push( gm );
				break;

            case state_stamp :
                h_state_stamp( gm );
				break;

            case state_stamp_grab :
                h_state_stamp_grab( gm );
				break;

			default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on sm_state(%d).",
            Log_proc_list[this_proc], __FILE__, __LINE__, sm_state);
#endif

                gm->mp.error = MP_BADCOMMAND;
				break;
		}

        if (gm->mp.message == SM_COMMAND_MSG)
            ret_value = msg_reply( gm, __FILE__, __LINE__ );

		generic_msg_free( gm, __FILE__, __LINE__ );
	}
}

/***** END OF FILE HERE *****/
/****************************/

