/****************************
 *
 * FILE : io_simulate.c
 *
 * DESC : I/O simulator
 *
 * DATE : 29.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: io_simulate.c,v $
 *  Revision 1.2  2003/10/10 20:57:09  tmike
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
 *  Revision 1.1  2003/10/10 05:44:41  tmike
 *  State Machine appears functional (need to detail review Log file)
 *      I/O task removed
 *      I/O table in SHM
 *      LogFlags in SHM
 *
 *  Revision 1.3  2003/10/07 22:49:05  tmike
 *  added SM_COMMAND_MSG to airpressure and estop
 *
 *  Revision 1.2  2003/09/30 21:14:59  tmike
 *  minor cleanup
 *
 *  Revision 1.1  2003/09/30 05:01:18  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
 *
 *
 ****************************/

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** INCLUDE FILES *****/
/*************************/

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

static char *cvsid = "$Id: io_simulate.c,v 1.2 2003/10/10 20:57:09 tmike Exp $";

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

//////////////////////////////////////////////////////
int io_simulate( IO_CMD_LIST command, IO_BIT_LIST bit)
{
    int ret_value = -1;

    switch (bit)
    {
        case cylAretracted :
        case cylAextended  :
        case cylBretracted :
        case cylBextended  :
        case cylCretracted :
        case cylCextended  :
            ret_value = 2;
            break;

        case cylAsolenoid :
            if (command == io_turn_off)
            {
                ioptr[cylAretracted] = 1;
                ioptr[cylAextended]  = 0;
                ret_value = 0;
            } else {
                ioptr[cylAretracted] = 0;
                ioptr[cylAextended]  = 1;
                ret_value = 1;
            }
            break;

        case cylBsolenoid :
            if (command == io_turn_off)
            {
                ioptr[cylBretracted] = 1;
                ioptr[cylBextended]  = 0;
                ret_value = 0;
            } else {
                ioptr[cylBretracted] = 0;
                ioptr[cylBextended]  = 1;
                ret_value = 1;
            }
            break;

        case cylCsolenoid :
            if (command == io_turn_off)
            {
                ioptr[cylCretracted] = 1;
                ioptr[cylCextended]  = 0;
                ret_value = 0;
            } else {
                ioptr[cylCretracted] = 0;
                ioptr[cylCextended]  = 1;
                ret_value = 1;
            }
            break;

        case airpressure :
            ret_value = 2;
            break;

        case estop :
            ret_value = 2;
            break;

        default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].io_simulate(default : bit[%d]).",
            Log_proc_list[this_proc], __FILE__, __LINE__, bit);
#endif

            break;
    }

#ifdef LOG_ENABLE
    if (lfptr->simulate_flag)
    {
        Log(LOG_TRACE,"[%s].EXITING io_simulate( command[%s].bit[%s] ).[%d].",
                Log_proc_list[this_proc],
                Log_io_cmd_list[command],
                Log_io_bit_list[bit], ret_value);
    }
#endif

    return ret_value;
}

/***** END OF FILE HERE *****/
/****************************/

