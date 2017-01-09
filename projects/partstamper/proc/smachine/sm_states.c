/****************************
 *
 * FILE : sm_states.c
 *
 * DESC : State Machine state handlers
 *
 * DATE : 02.October.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 * 	$Log: sm_states.c,v $
 * 	Revision 1.5  2003/10/10 20:57:09  tmike
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
 * 	Revision 1.4  2003/10/10 05:44:41  tmike
 * 	State Machine appears functional (need to detail review Log file)
 * 	    I/O task removed
 * 	    I/O table in SHM
 * 	    LogFlags in SHM
 * 	
 * 	Revision 1.3  2003/10/09 22:57:39  tmike
 * 	Log file generated, with single Tcl command
 * 	to StateMachine process : return state
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

static char *cvsid = "$Id: sm_states.c,v 1.5 2003/10/10 20:57:09 tmike Exp $";

// state information
////////////////////
static int continuous = 0;
static MP_ERROR pstamper = MP_LAST;
static MP_ERROR pgrabber = MP_LAST;

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

////////////////////////////////////////////////////
static void reset_state_vars( char *file, int line )
{
    continuous = 0;
    pstamper = MP_LAST;
    pgrabber = MP_LAST;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s][%s][%d]..EXITING reset_state_vars().",
                Log_proc_list[this_proc], file, line);
    }
#endif

}

////////////////////////////////////////
void next_state_check( GENERIC_MSG *gm )
{
    SMCM *smcm;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING next_state_check( gm[%x] ).",
                Log_proc_list[this_proc], gm);
    }
#endif

    switch (sm_state)
    {
        case state_ehardware :

            if ( (value_of(airpressure) == 1) &&
                 (value_of(estop == 0)) )
            {

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d]..new state = state_quiet.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                    reset_state_vars( __FILE__, __LINE__ );
                    sm_state = state_quiet;
            }
            break;

        case state_quiet :

            if ( (value_of(airpressure) == 0) ||
                 (value_of(estop == 1)) )
           {

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d]..new state = state_ehardware.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                reset_state_vars( __FILE__, __LINE__ );
                sm_state = state_ehardware;

           } else {

                smcm = (SMCM *) &(gm->data[0]);

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%d].h_state_quiet received command[%s].continuous[%d].bit[%s].value[%d].",
            Log_proc_list[this_proc], __LINE__,
            Log_sm_cmd_list[smcm->command], smcm->continuous,
            Log_io_bit_list[smcm->bit], smcm->value);
#endif

            }
            break;

        case state_grab :
        case state_push :
        case state_stamp :
        case state_stamp_grab :

            if ( (value_of(airpressure) == 0) ||
                 (value_of(estop == 1)) )
            {
                reset_subsystems();

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d]..new state = state_ehardware.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                sm_state = state_ehardware;

            } else {

                smcm = (SMCM *) &(gm->data[0]);

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%d].active state received command[%s].continuous[%d].bit[%s].value[%d].",
            Log_proc_list[this_proc], __LINE__,
            Log_sm_cmd_list[smcm->command], smcm->continuous,
            Log_io_bit_list[smcm->bit], smcm->value);
#endif

            }
            break;

        default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on sm_state[%d].",
        Log_proc_list[this_proc], __FILE__, __LINE__, sm_state);
#endif

            gm->mp.error = MP_BADSTATE;
            break;
    }

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING next_state_check( gm[%x] ).",
                Log_proc_list[this_proc], gm);
    }
#endif

}

/////////////////////////////////////////
void h_state_ehardware( GENERIC_MSG *gm )
{
    SMCM *smcm;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING h_state_ehardware( gm[%x] gm->mp.message[%s] ).",
                Log_proc_list[this_proc], gm,
                Log_msg_list[gm->mp.message]);
    }
#endif

    switch (gm->mp.message)
    {
        case SM_COMMAND_MSG :
            smcm = (SMCM *) &(gm->data[0]);

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%d]..h_state_ehardware( command[%s].continuous[%d].bit[%s].value[%d] ).",
                Log_proc_list[this_proc], __LINE__,
                Log_sm_cmd_list[smcm->command], smcm->continuous,
                Log_io_bit_list[smcm->bit], smcm->value);
    }
#endif

            switch (smcm->command)
            {
                case sm_start :
                    gm->mp.error = MP_UNHANDLEDMSG;
                    break;

                case sm_stop :
                    break;

                case sm_return_state :
                    smcm->state = sm_state;
                    break;

                default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on smcm->command[%d].",
        Log_proc_list[this_proc], __FILE__, __LINE__, smcm->command);
#endif

                    gm->mp.error = MP_UNHANDLEDMSG;
                    break;
            }
            break;

        case PS_COMMAND_MSG :

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%d]..h_ehardware_state( gm->mp.error[%s] on reply from[%s] ).",
                Log_proc_list[this_proc], __LINE__,
                Log_msg_list[gm->mp.error],
                Log_proc_list[gm->mp.sender]);
    }
#endif

            break;

        default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on gm->mp.message[%d].",
        Log_proc_list[this_proc], __FILE__, __LINE__, gm->mp.message);
#endif

            break;
    }

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING h_state_ehardware( gm[%x] gm->mp.message[%s] ).",
                Log_proc_list[this_proc], gm,
                Log_msg_list[gm->mp.message]);
    }
#endif

    return;
}

//////////////////////////////////////
void h_state_quiet( GENERIC_MSG *igm )
{
    GENERIC_MSG *ogm;
    SMCM *smcm;
    PSCM *pscm;
    int  ret_value;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING h_state_quiet( igm[%x] message[%s] ).",
                Log_proc_list[this_proc], igm,
                Log_msg_list[igm->mp.message]);
    }
#endif

    switch (igm->mp.message)
    {
        case SM_COMMAND_MSG :

            smcm = (SMCM *) &(igm->data[0]);

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%d]..h_state_quiet( command[%s].continuous[%d].bit[%s].value[%d] ).",
                Log_proc_list[this_proc], __LINE__,
                Log_sm_cmd_list[smcm->command], smcm->continuous,
                Log_io_cmd_list[smcm->bit], smcm->value);
    }
#endif 

            switch (smcm->command)
            {
                case sm_start :
                    ogm = generic_msg_init( PS_COMMAND_MSG, __FILE__, __LINE__ );
                    if (ogm == NULL)
                    {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].generic_msg_init return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__);
#endif

                        igm->mp.error = MP_NULLRETURN;

                    } else {
                        continuous = smcm->continuous;

                        pscm = (PSCM *) &(ogm->data[0]);
                        pscm->command = ps_grab_part;
                        ogm->mp.receiver = PROC_pgrabber;
                        ogm->mp.message = PS_COMMAND_MSG;
                        ret_value = msg_send( ogm, __FILE__, __LINE__ );
                        generic_msg_free( ogm, __FILE__, __LINE__ );

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d]..new state = state_grab.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                        sm_state = state_grab;
                    }
                    break;

                case sm_stop :
                    igm->mp.error = MP_BADCOMMAND;
                    break;

                case sm_return_state :
                    smcm->state = sm_state;
                    break;

                default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on smcm->command[%d].",
        Log_proc_list[this_proc], __FILE__, __LINE__, smcm->command);
#endif

                    igm->mp.error = MP_BADCOMMAND;
                    break;
            }
            break;

        case PS_COMMAND_MSG :

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%d].h_state_quiet received PS_COMMAND_MSG from[%s].",
        Log_proc_list[this_proc], __LINE__,
        Log_proc_list[igm->mp.sender]);
#endif

            break;

        default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on igm->mp.message[%d].",
        Log_proc_list[this_proc], __FILE__, __LINE__,
        igm->mp.message);
#endif

            break;
    }
    return;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING h_state_quiet( gm[%x] message[%s] ).",
                Log_proc_list[this_proc], igm,
                Log_msg_list[igm->mp.message]);
    }
#endif

}

//////////////////////////////////////
void h_state_grab( GENERIC_MSG *igm )
{
    GENERIC_MSG *ogm;
    SMCM *smcm;
    PSCM *pscm;
    int  ret_value;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING h_state_grab( gm[%x] message[%s] ).",
                Log_proc_list[this_proc], igm,
                Log_msg_list[igm->mp.message]);
    }
#endif

    switch (igm->mp.message)
    {
        case SM_COMMAND_MSG :
            smcm = (SMCM *) &(igm->data[0]);

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%d].h_state_grab( smcm->command[%s] ).",
                Log_proc_list[this_proc], __LINE__,
                Log_msg_list[smcm->command]);
    }
#endif

            switch (smcm->command)
            {
                case sm_start :
                    igm->mp.error = MP_BADCOMMAND;
                    break;

                case sm_stop :
                    reset_subsystems();
                    reset_state_vars( __FILE__, __LINE__ );

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_quiet.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                    sm_state = state_quiet;
                    break;

                case sm_return_state :
                    smcm->state = sm_state;
                    break;

                default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on smcm->command[%d].",
            Log_proc_list[this_proc], __FILE__, __LINE__, smcm->command);
#endif

                    igm->mp.error = MP_BADCOMMAND;
                    break;
            }
            break;

        case PS_COMMAND_MSG :
            pscm = (PSCM *) &(igm->data[0]);

            // only expected message is reply from PROC_pgrabber
            ////////////////////////////////////////////////////
            if ( (igm->mp.sender != PROC_pgrabber) ||
                 (igm->mp.error != MP_NOERROR) ) {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].h_state_grab received PS_COMMAND_MSG from[%s] with error[%s].",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_proc_list[igm->mp.sender],
            Log_mp_error[igm->mp.error]);
#endif

            } else {

                ogm = generic_msg_init( PS_COMMAND_MSG, __FILE__, __LINE__ );
                if (ogm == NULL)
                {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].generic_msg_init return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__);
#endif

                } else {

                    pscm = (PSCM *) &(ogm->data[0]);
                    pscm->command = ps_push_part;
                    ogm->mp.receiver = PROC_ppusher;
                    ogm->mp.message = PS_COMMAND_MSG;
                    ret_value = msg_send( ogm, __FILE__, __LINE__ );
                    generic_msg_free( ogm, __FILE__, __LINE__ );

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_push.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                    sm_state = state_push;
                }
            }
            break;

        default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%d][%s][%d].default on igm->mp.message[%d].",
            Log_proc_list[this_proc], __FILE__, __LINE__, igm->mp.message);
#endif

            break;
    }
    return;
}

/////////////////////////////////////
void h_state_push( GENERIC_MSG *igm )
{
    GENERIC_MSG *ogm;
    SMCM *smcm;
    PSCM *pscm;
    int ret_value;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].h_state_push( gm[%d] message[%s] ).",
                Log_proc_list[this_proc], igm,
                Log_msg_list[igm->mp.message]);
    }
#endif


    switch (igm->mp.message)
    {
        case SM_COMMAND_MSG :
            smcm = (SMCM *) &(igm->data[0]);

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%d].h_state_push( smcm->command[%s] ).",
                Log_proc_list[this_proc], __LINE__,
                Log_sm_cmd_list[smcm->command]);
    }
#endif

            switch (smcm->command)
            {
                case sm_start :
                    igm->mp.error = MP_BADCOMMAND;
                    break;

                case sm_stop :
                    reset_subsystems();
                    reset_state_vars( __FILE__, __LINE__);

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_quiet.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif
                    sm_state = state_quiet;
                    break;

                case sm_return_state :
                    smcm->state = sm_state;
                    break;

                default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on smcm->command[%d].",
            Log_proc_list[this_proc], __FILE__, __LINE__, smcm->command);
#endif

                    igm->mp.error = MP_BADCOMMAND;
                    break;
            }
            break;

        case PS_COMMAND_MSG :
            pscm = (PSCM *) &(igm->data[0]);

            // only expected message is reply from PROC_ppusher
            ///////////////////////////////////////////////////
            if ( (igm->mp.sender != PROC_ppusher) ||
                 (igm->mp.error != MP_NOERROR) ) {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].h_state_push received PS_COMMAND_MSG from[%s] with error[%s].",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_msg_list[igm->mp.sender],
            Log_mp_error[igm->mp.error]);
#endif

            } else {

                ogm = generic_msg_init( PS_COMMAND_MSG, __FILE__, __LINE__ );
                if (ogm == NULL)
                {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].generic_msg_init return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__);
#endif

                } else {
                    pscm = (PSCM *) &(ogm->data[0]);
                    pscm->command = ps_stamp_part;
                    ogm->mp.receiver = PROC_pstamper;
                    ogm->mp.message = PS_COMMAND_MSG;
                    ret_value = msg_send( ogm, __FILE__, __LINE__ );

                    if (continuous == 1)
                    {
                        pgrabber = MP_LAST;
                        pstamper = MP_LAST;
                        pscm->command = ps_grab_part;
                        ogm->mp.receiver = PROC_pgrabber;
                        ret_value = msg_send( ogm, __FILE__, __LINE__ );

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_stamp_grab.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                        sm_state = state_stamp_grab;

                    } else {

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_stamp.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                        sm_state = state_stamp;
                
                    }
                    generic_msg_free( ogm, __FILE__, __LINE__ );
                }
            }
            break;

        default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on igm->mp.message[%d].",
            Log_proc_list[this_proc], __FILE__, __LINE__, igm->mp.message);
#endif

            break;
    }
    return;
}

/////////////////////////////////////
void h_state_stamp( GENERIC_MSG *gm )
{
    SMCM *smcm;
    PSCM *pscm;
    int  ret_value;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].h_state_stamp( gm[%x] message[%s] ).",
                Log_proc_list[this_proc], gm,
                Log_msg_list[gm->mp.message]);
    }
#endif

    switch (gm->mp.message)
    {
        case SM_COMMAND_MSG :
            smcm = (SMCM *) &(gm->data[0]);

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%d].h_state_stamp( smcm->command[%s] ).",
                Log_proc_list[this_proc], __LINE__,
                Log_sm_cmd_list[smcm->command]);
    }
#endif

            switch (smcm->command)
            {
                case sm_start :
                    gm->mp.error = MP_BADCOMMAND;
                    break;

                case sm_stop :
                    reset_subsystems();
                    reset_state_vars( __FILE__, __LINE__ );

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_quiet.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                    sm_state = state_quiet;
                    break;

                case sm_return_state :
                    smcm->state = sm_state;
                    break;

                default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on smcm->command[%d].",
            Log_proc_list[this_proc], __FILE__, __LINE__, smcm->command);
#endif

                    gm->mp.error = MP_BADCOMMAND;
                    break;
            }
            break;

        case PS_COMMAND_MSG :
            pscm = (PSCM *) &(gm->data[0]);

            // only expected message is reply from PROC_pstamper
            ////////////////////////////////////////////////////
            if ( (gm->mp.sender != PROC_pstamper) ||
                 (gm->mp.error != MP_NOERROR) )
            {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].h_state_stamp received PS_COMMAND_MSG from[%s] with error[%s].",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_proc_list[gm->mp.sender],
            Log_mp_error[gm->mp.error]);
#endif

            } else {

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_quiet.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                sm_state = state_quiet;
                reset_state_vars( __FILE__, __LINE__ );
            }
            break;

        default :

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_DEBUG,"[%s][%s][%d].default on gm->mp.message[%d].",
                Log_proc_list[this_proc], __FILE__, __LINE__, gm->mp.message);
    }
#endif

            break;
    }
    return;
}

///////////////////////////////////////////
void h_state_stamp_grab( GENERIC_MSG *igm )
{
    GENERIC_MSG *ogm;
    SMCM *smcm;
    PSCM *pscm;
    int  ret_value;

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING h_state_stamp_grab( gm[%x] message[%s] ).",
                Log_proc_list[this_proc], igm,
                Log_msg_list[igm->mp.message]);
    }
#endif

    switch (igm->mp.message)
    {
        case SM_COMMAND_MSG :
            smcm = (SMCM *) &(igm->data[0]);

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%d].h_state_stamp_grab( smcm->command[%s] ).",
                Log_proc_list[this_proc], __LINE__,
                Log_sm_cmd_list[smcm->command]);
    }
#endif

            switch (smcm->command)
            {
                case sm_start :
                    igm->mp.error = MP_BADCOMMAND;
                    break;

                case sm_stop :
                    reset_subsystems();
                    reset_state_vars( __FILE__, __LINE__ );

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_quiet.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                    sm_state = state_quiet;
                    break;

                case sm_return_state :
                    smcm->state = sm_state;
                    break;

                default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on smcm->command[%d].",
            Log_proc_list[this_proc], __FILE__, __LINE__, smcm->command);
#endif

                    igm->mp.error = MP_BADCOMMAND;
                    break;
            }
            break;

        case PS_COMMAND_MSG :
            pscm = (PSCM *) &(igm->data[0]);

            // expected message is reply from PROC_pstamper
            // expected message is reply from PROC_grabber
            ///////////////////////////////////////////////
            switch (igm->mp.sender)
            {
                case PROC_pstamper :
                    pstamper = igm->mp.error;
                    break;

                case PROC_pgrabber :
                    pgrabber = igm->mp.error;
                    break;

                default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].h_state_stamp_grab received PS_COMMAND_MSG from[%s].",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_msg_list[igm->mp.sender]);
#endif

                    break;
            }

            if ( (pstamper == MP_NOERROR) && (pgrabber == MP_NOERROR) )
            {
                // can re-use inbound message buffer, because it was reply
                //////////////////////////////////////////////////////////
                pscm->command = ps_push_part;
                igm->mp.receiver = PROC_ppusher;
                igm->mp.message = PS_COMMAND_MSG;
                ret_value = msg_send( igm, __FILE__, __LINE__ );

#ifdef LOG_ENABLE
    if (lfptr->smachine_flag)
    {
        Log(LOG_SMACHINE,"[%s][%s][%d].new state = state_push.",
                Log_proc_list[this_proc], __FILE__, __LINE__);
    }
#endif

                sm_state = state_push;
            }
            break;

        default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].default on igm->mp.message[%d].",
            Log_proc_list[this_proc], __FILE__, __LINE__, igm->mp.message);
#endif

            break;
    }
    return;
}

/***** END OF FILE HERE *****/
/****************************/

