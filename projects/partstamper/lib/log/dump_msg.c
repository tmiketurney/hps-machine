/****************************
 *
 * FILE : dump_msg.c
 *
 * DESC : System message dump utilities
 *
 * DATE : 23.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: dump_msg.c,v $
 *  Revision 1.10  2003/10/10 05:44:41  tmike
 *  State Machine appears functional (need to detail review Log file)
 *      I/O task removed
 *      I/O table in SHM
 *      LogFlags in SHM
 *
 *  Revision 1.9  2003/10/09 22:57:39  tmike
 *  Log file generated, with single Tcl command
 *  to StateMachine process : return state
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
 *  Revision 1.7  2003/10/07 22:48:18  tmike
 *  Removed dump_msg_table, general cleanup
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
 *  Revision 1.4  2003/09/29 18:53:45  tmike
 *  TCL sendcmd working for IO_CMD_MESSAGE
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
 *  Revision 1.1  2003/09/23 23:59:01  tmike
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

#include "message.h"
#include "process.h"
#include "log.h"
#include "io.h"

#include "smachine.h"
#include "pstamper.h"
#include "ui.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static char *cvsid = "$Id: dump_msg.c,v 1.10 2003/10/10 05:44:41 tmike Exp $";

/***** EXTERNAL REFERENCES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

/////////////////////////////////////////
static void dump_mp( MESSAGE_PACKET *mp )
{
#ifdef LOG_ENABLE
    Log(LOG_MSG,".....<mp>.message[%s].sender[%s].receiver[%s].error[%s].",
        Log_msg_list[mp->message], Log_proc_list[mp->sender],
        Log_proc_list[mp->receiver], Log_mp_error[mp->error]);
#endif
}

//////////////////////////////////////////
static void dump_ps_msg( GENERIC_MSG *gm )
{
#ifdef LOG_ENABLE
    PSCM *pscm = (PSCM *) &(gm->data[0]);

    dump_mp( &(gm->mp) );
    Log(LOG_MSG,".....<ps>.command[%s].state[%d].",
            Log_ps_cmd_list[pscm->command], pscm->state);

#endif
}

//////////////////////////////////////////
static void dump_gm_msg( GENERIC_MSG *gm )
{
#ifdef LOG_ENABLE
    dump_mp( &(gm->mp) );
    Log(LOG_MSG,".....<gm>.data[%x].", gm);
#endif
}

//////////////////////////////////////////
static void dump_sm_msg( GENERIC_MSG *gm )
{
    SMCM *smcm = (SMCM *) &(gm->data[0]);

#ifdef LOG_ENABLE
    dump_mp( &(gm->mp) );
    Log(LOG_MSG,".....<sm>.command[%s].continuous[%d].state[%s].bit[%s].value[%d].",
            Log_sm_cmd_list[smcm->command], smcm->continuous,
            Log_sm_state[smcm->state],
            Log_io_bit_list[smcm->bit], smcm->value);
#endif

}

//////////////////////////////////////////
static void dump_ui_msg( GENERIC_MSG *gm )
{
#ifdef LOG_ENABLE

#endif
}

////////////////////////////////
void dump_msg( GENERIC_MSG *gm )
{
#ifdef LOG_ENABLE
    switch (gm->mp.message)
    {
        case PS_COMMAND_MSG :
            dump_ps_msg( gm );
            break;

        case SM_COMMAND_MSG :
            dump_sm_msg( gm );
            break;

        case UI_COMMAND_MSG :
            dump_ui_msg( gm );
            break;

        case PS_GENERIC_MSG :
            dump_gm_msg( gm );
            break;

        default :
            Log(LOG_DEBUG,"[%s][%s][%d].default on message type[%d].",
                    Log_proc_list[this_proc], __FILE__, __LINE__, gm->mp.message);
            break;
    }
#endif
}

/***** END OF FILE HERE *****/
/****************************/

