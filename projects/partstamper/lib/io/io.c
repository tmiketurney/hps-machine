/****************************
 *
 * FILE : io.c
 *
 * DESC : I/O Monitor library, provides following entrypoints:
 *      value_of
 *      turn_off
 *      turn_on
 *
 * DATE : 26.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: io.c,v $
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
 *  Revision 1.8  2003/10/10 05:44:41  tmike
 *  State Machine appears functional (need to detail review Log file)
 *      I/O task removed
 *      I/O table in SHM
 *      LogFlags in SHM
 *
 *  Revision 1.7  2003/10/09 22:57:39  tmike
 *  Log file generated, with single Tcl command
 *  to StateMachine process : return state
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
 *  Revision 1.5  2003/10/07 22:47:38  tmike
 *  added SM_COMMAND_MSG to airpressure and estop
 *
 *  Revision 1.4  2003/09/30 05:01:18  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
 *
 *  Revision 1.3  2003/09/29 14:17:08  tmike
 *  added sendcmd to pstcl tester
 *  
 *  Revision 1.2  2003/09/27 22:46:48  tmike
 *  TCL process added with rt and showsched commands
 *  add State Machine process
 *  add I/O monitor
 *  pgrabber process coded, not tested
 *  
 *  Revision 1.1  2003/09/26 21:55:27  tmike
 *  merged dump library with new log library
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

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static char *cvsid = "$Id: io.c,v 1.9 2003/10/10 20:57:09 tmike Exp $";

static PFI io_activator[IO_LAST] = {
    NULL,        // cylAretracted
    NULL,        // cylAextended
    io_simulate, // cylAsolenoid
    NULL,        // cylBretracted
    NULL,        // cylBextended
    io_simulate, // cylBsolenoid
    NULL,        // cylCretracted
    NULL,        // cylCextended
    io_simulate, // cylCsolenoid
    io_simulate, // airpressure
    io_simulate  // estop
};

int io_table[IO_LAST];

int io_table_reset[IO_LAST] = {
    1, // cylAretracted
    0, // cylAextended
    0, // cylAsolenoid
    1, // cylBretracted
    0, // cylBextended
    0, // cylBsolenoid
    1, // cylCretracted
    0, // cylCextended
    0, // cylCsolenoid
    1, // airpressure
    0  // estop
};

/***** EXTERNAL REFERENCES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

///////////////////////////////
int turn_off( IO_BIT_LIST bit )
{
    GENERIC_MSG *gm;
    SMCM *smcm;
    int ret_value;

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING turn_off( %s ).",
                Log_proc_list[this_proc],
                Log_io_bit_list[bit]);
    }
#endif

    if (io_activator[bit] != NULL)
        ret_value = ( * io_activator[bit]) ( io_turn_off, bit );

    if (ret_value != -1)
    {
        ioptr[bit] = 0;
        
        if ( (bit == airpressure) || (bit == estop) )
        {
            gm = generic_msg_init( SM_COMMAND_MSG, __FILE__, __LINE__ );
            if (gm == NULL)
            {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].generic_msg_init return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__);
#endif
                ret_value = -1;

            } else {

                gm->mp.receiver = PROC_smachine;
                gm->mp.message = SM_COMMAND_MSG;

                smcm = (SMCM *) &(gm->data[0]);
                smcm->command = sm_io_value_change;
                smcm->bit = bit;
                smcm->value = 0;

                ret_value = msg_send( gm, __FILE__, __LINE__ );
                generic_msg_free( gm, __FILE__, __LINE__ );

                gm = msg_receive( SM_COMMAND_MSG, __FILE__, __LINE__ );
                generic_msg_free( gm, __FILE__, __LINE__ );

                ret_value = 0;
            }
        }
    } else {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].turn_off bit[%s] activator failed.",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_io_bit_list[bit]);
#endif

    }

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING turn_off( %s ).[%d].",
                Log_proc_list[this_proc],
                Log_io_bit_list[bit], ret_value);
    }
#endif

    return ret_value;

}

//////////////////////////////
int turn_on( IO_BIT_LIST bit )
{
    GENERIC_MSG *gm;
    SMCM *smcm;
    int ret_value;

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING turn_on( %s ).",
                Log_proc_list[this_proc],
                Log_io_bit_list[bit]);
    }
#endif

    if (io_activator[bit] != NULL)
        ret_value = ( * io_activator[bit]) ( io_turn_on, bit );

    if (ret_value != -1)
    {
        ioptr[bit] = 1;
        
        if ( (bit == airpressure) || (bit == estop) )
        {
            gm = generic_msg_init( SM_COMMAND_MSG, __FILE__, __LINE__ );
            if (gm == NULL)
            {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].generic_msg_init return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__);
#endif
                ret_value = -1;

            } else {

                gm->mp.receiver = PROC_smachine;
                gm->mp.message = SM_COMMAND_MSG;

                smcm = (SMCM *) &(gm->data[0]);
                smcm->command = sm_io_value_change;
                smcm->bit = bit;
                smcm->value = 1;

                ret_value = msg_send( gm, __FILE__, __LINE__ );
                generic_msg_free( gm, __FILE__, __LINE__ );

                gm = msg_receive( SM_COMMAND_MSG, __FILE__, __LINE__ );
                generic_msg_free( gm, __FILE__, __LINE__ );

                ret_value = 1;
            }
        }
    } else {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].turn_on bit[%s] activator failed.",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_io_bit_list[bit]);
#endif

    }

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING turn_on( %s ).[%d].",
                Log_proc_list[this_proc],
                Log_io_bit_list[bit], ret_value);
    }
#endif

    return ret_value;
}

///////////////////////////////
int value_of( IO_BIT_LIST bit )
{
    int ret_value;

    if (io_activator[bit] != NULL)
        ret_value = ( * io_activator[bit]) ( io_value_of, bit );

    if (ret_value == -1)
    {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].value_of bit[%s] activator failed.",
            Log_proc_list[this_proc], __FILE__, __LINE__,
            Log_io_bit_list[bit]);
#endif

    } else
        ret_value = ioptr[bit];

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING value_of( %s ).[%d].",
                Log_proc_list[this_proc],
                Log_io_bit_list[bit], ret_value);
    }
#endif

    return ret_value;
}

/////////////////////////////////////////////
int wait_until( IO_BIT_LIST bit, int target )
{
    int ret_value;

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].ENTERING wait_until(%s.%d).",
                Log_proc_list[this_proc], Log_io_bit_list[bit], target);
    }
#endif

    while ( (ret_value = value_of(bit)) != target )
    {
        if ( ret_value == -1 )
            break;
        else
            sleep(1);
    }

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING wait_until(%s.%d).[%d].",
                Log_proc_list[this_proc], Log_io_bit_list[bit], target, ret_value);
    }
#endif

    return ret_value;
}

/////////////////////////////////////
void c_io_report( IO_BIT_LIST *bits )
{
    int i;

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].c_io_report( %x ).",
                Log_proc_list[this_proc], bits);
    }
#endif

    for (i=0; i<IO_LAST; i++)
        bits[i] = ioptr[i];
}

//////////////////////////////////
void c_io_reset( )
{
    int i;

#ifdef LOG_ENABLE
    if (lfptr->library_flag)
    {
        Log(LOG_TRACE,"[%s].c_io_reset( ).",
                Log_proc_list[this_proc]);
    }
#endif

    for (i=0; i<IO_LAST; i++)
    {
        ioptr[i] = io_table_reset[i];

        // TODO should use activator table as well
        //////////////////////////////////////////

    }
}


/***** END OF FILE HERE *****/
/****************************/

