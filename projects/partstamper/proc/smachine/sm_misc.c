/****************************
 *
 * FILE : sm_misc.c
 *
 * DESC : State Machine miscellaneous entry point
 *
 * DATE : 6.October.2003    Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 * 	$Log: sm_misc.c,v $
 * 	Revision 1.4  2003/10/10 20:57:09  tmike
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
 * 	Revision 1.3  2003/10/10 05:44:41  tmike
 * 	State Machine appears functional (need to detail review Log file)
 * 	    I/O task removed
 * 	    I/O table in SHM
 * 	    LogFlags in SHM
 * 	
 * 	Revision 1.2  2003/10/09 21:50:05  tmike
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
 * 	Revision 1.1  2003/10/07 22:52:41  tmike
 * 	Implemented state machine, just beginning to test
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

static char *cvsid = "$Id: sm_misc.c,v 1.4 2003/10/10 20:57:09 tmike Exp $";

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

///////////////////////
void reset_subsystems()
{
	GENERIC_MSG *gm;
	PSCM *pscm;
    int  ret_value;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING reset_subsystems().", Log_proc_list[this_proc]);
    }
#endif

	// drive Part Pusher subsystem to halt and reset
	////////////////////////////////////////////////
	gm = generic_msg_init( PS_COMMAND_MSG, __FILE__, __LINE__ );
	if (gm == NULL)
	{

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].generic_msg_init[%d] return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__, PS_COMMAND_MSG);
#endif

		return;

	} else {
		pscm = (PSCM *) &(gm->data[0]);
	}

	// initialize payload
	/////////////////////
	gm->mp.receiver = PROC_ppusher;
	gm->mp.message = PS_COMMAND_MSG;
	pscm->command = ps_reset;

    // send command message
    ///////////////////////
	ret_value = msg_send( gm, __FILE__, __LINE__ );
    generic_msg_free( gm, __FILE__, __LINE__ );

    // wait for reply
    /////////////////
	gm = msg_receive( PS_COMMAND_MSG, __FILE__, __LINE__ );
	if (gm == NULL)
	{

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive( [%s] ) return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_msg_list[PS_COMMAND_MSG]);
#endif

		return;

	} else if (gm->mp.error != MP_NOERROR) {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive( gm->mp.error[%s] ).",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_mp_error[gm->mp.error]);
#endif

        generic_msg_free( gm, __FILE__, __LINE__ );
        return;
    }

	// drive Part Stamper subsystem to halt and reset
    // re-using reply message buffer from PROC_ppusher
	//////////////////////////////////////////////////
    pscm = (PSCM *) &(gm->data[0]);

	// initialize payload
	/////////////////////
	gm->mp.receiver = PROC_pstamper;
	gm->mp.message = PS_COMMAND_MSG;
	pscm->command = ps_reset;

    // send command message
    ///////////////////////
	ret_value = msg_send( gm, __FILE__, __LINE__ );
    generic_msg_free( gm, __FILE__, __LINE__ );

    // wait for reply
    /////////////////
	gm = msg_receive( PS_COMMAND_MSG, __FILE__, __LINE__ );
	if (gm == NULL)
	{

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive( [%s] ) return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_msg_list[PS_COMMAND_MSG]);
#endif

		return;

	} else if (gm->mp.error != MP_NOERROR) {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive( gm->mp.error[%s] ).",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_mp_error[gm->mp.error]);
#endif

        generic_msg_free( gm, __FILE__, __LINE__ );
        return;
    }

	// drive Part Grabber subsystem to halt and reset
    // re-using reply message buffer from PROC_pstamper
	///////////////////////////////////////////////////
    pscm = (PSCM *) &(gm->data[0]);

	// initialize payload
	/////////////////////
	gm->mp.receiver = PROC_pgrabber;
	gm->mp.message = PS_COMMAND_MSG;
	pscm->command = ps_reset;

    // send command message
    ///////////////////////
	ret_value = msg_send( gm, __FILE__, __LINE__ );
    generic_msg_free( gm, __FILE__, __LINE__ );

    // wait for reply
    /////////////////
	gm = msg_receive( PS_COMMAND_MSG, __FILE__, __LINE__ );
	if (gm == NULL)
	{

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive( [%s] ) return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_msg_list[PS_COMMAND_MSG]);
#endif

		return;

	} else if (gm->mp.error != MP_NOERROR) {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive( gm->mp.error[%s] ).",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_mp_error[gm->mp.error]);
#endif

        generic_msg_free( gm, __FILE__, __LINE__ );
        return;
    }

	generic_msg_free( gm, __FILE__, __LINE__ );

	// drive I/O subsystem to halt and reset
	///////////////////////////////////////////////////
    c_io_reset();

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING reset_subsystems().", Log_proc_list[this_proc]);
    }
#endif

}

/***** END OF FILE HERE *****/
/****************************/

