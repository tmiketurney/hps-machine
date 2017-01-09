/****************************
 *
 * FILE : sendcmd.c
 *
 * DESC : sendcmd message TCL command handler
 *
 * DATE : 27.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: sendcmd.c,v $
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
 *  Revision 1.7  2003/10/09 21:50:05  tmike
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
 *  Revision 1.6  2003/10/07 22:52:13  tmike
 *  fixed sendcmd.c build_sm_msg to support new command sm_io_value_change
 *  added state enumeration, general cleanup
 *
 *  Revision 1.5  2003/09/30 21:14:59  tmike
 *  minor cleanup
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
 *  Revision 1.3  2003/09/29 18:53:51  tmike
 *  TCL sendcmd working for IO_CMD_MESSAGE
 *  
 *  Revision 1.2  2003/09/29 15:51:28  tmike
 *  fixed NULL on end of help table
 *  
 *  Revision 1.1  2003/09/29 14:17:10  tmike
 *  added sendcmd to pstcl tester
 *  
 *
 ****************************/

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

#define USE_INTERP_RESULT

/***** INCLUDE FILES *****/
/*************************/

#include <tcl8.6/tcl.h>

#include "message.h"
#include "process.h"
#include "log.h"

#include "io.h"
#include "pstamper.h"
#include "smachine.h"
#include "ui.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static char *sendcmd_help[] = {
    "sendcmd <MSG_TYPE> [message parameters]\n",
    " MSG_TYPE is a required parameter\n",
    " if no other parameters, interactive mode default\n",
    NULL
};

static char *io_help[] = {
    "io <command> [<bit>]\n",
    " command [ off[0] | on[1] | value[2] | report[3] | reset[4] ]\n",
    " bit [ 0..10 ]\n",
    NULL
};

static char *smachine_help[] = {
    "sendcmd SMCM <command> [<continuous>] [<io_bit> <value>]\n",
    " command [ sm_start[0] | sm_stop[1] | sm_return_state[2] | sm_reset[3] | sm_io_value_change[4] ]\n",
    "  [continuous operation flag]\n",
    "  [bit[0..10] value(0|1)]\n",
    NULL
};

static char *pstamper_help[] = {
    "sendcmd PSCM <command>\n",
    " command :\n",
    "           ps_grab_part[0]  | ps_push_part[1]    | ps_stamp_part[2]\n",
    "           ps_stamp_grab[3] | ps_return_state[4] | ps_reset[5] ]\n",
    NULL
};

static char *ui_help[] = {
    "sendcmd UICM <command>\n",
    " command [ ui_reset[0] ]\n",
    NULL
};

static char *cvsid = "$Id: sendcmd.c,v 1.10 2003/10/10 20:57:09 tmike Exp $";

static char interp_result[MSGBUF_SIZE];

static char msg_buffer[0x100];

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

//////////////////////////
// Command Message Parsers
//////////////////////////

#if 0
///////////////////////////////////////////
static void parse_io_msg( GENERIC_MSG *gm )
{
    MESSAGE_PACKET *mp = (MESSAGE_PACKET *) &(gm->mp);
    IOCM *iocm = (IOCM *) &(gm->data[0]);

    sprintf(interp_result,"\n..MP.message[%d].sender[%d].receiver[%d].error[%d].",
        mp->message, mp->sender, mp->receiver, mp->error);

    sprintf(msg_buffer,"\n..IOCM.command[%d].bit[%d].value[%d].",
        iocm->command, iocm->bit, iocm->value);
    strcat( interp_result, msg_buffer );

    sprintf(msg_buffer,"\n...cylAretracted[%d].cylAextended[%d].cylAsolenoid[%d].",
        iocm->report[cylAretracted], iocm->report[cylAextended], iocm->report[cylAsolenoid]);
    strcat( interp_result, msg_buffer );

    sprintf(msg_buffer,"\n...cylBretracted[%d].cylBextended[%d].cylBsolenoid[%d].",
        iocm->report[cylBretracted], iocm->report[cylBextended], iocm->report[cylBsolenoid]);
    strcat( interp_result, msg_buffer );

    sprintf(msg_buffer,"\n...cylCretracted[%d].cylCextended[%d].cylCsolenoid[%d].",
        iocm->report[cylCretracted], iocm->report[cylCextended], iocm->report[cylCsolenoid]);
    strcat( interp_result, msg_buffer );

    sprintf(msg_buffer,"\n...airpressure[%d].estop[%d].",
        iocm->report[airpressure], iocm->report[estop]);
    strcat( interp_result, msg_buffer );

}
#endif

///////////////////////////////////////////
static void parse_ui_msg( GENERIC_MSG *gm )
{
    MESSAGE_PACKET *mp = (MESSAGE_PACKET *) &(gm->mp);
    UICM *uicm = (UICM *) &(gm->data[0]);

    sprintf(interp_result,"\n..MP.message[%d].sender[%d].receiver[%d].error[%d].",
            mp->message, mp->sender, mp->receiver, mp->error);

    sprintf(msg_buffer,"\n..UICM.command[%d].state[%d].", uicm->command, uicm->state);
    strcat( interp_result, msg_buffer );

}

/////////////////////////////////////////////////
static void parse_smachine_msg( GENERIC_MSG *gm )
{
    MESSAGE_PACKET *mp = (MESSAGE_PACKET *) &(gm->mp);
    SMCM *smcm = (SMCM *) &(gm->data[0]);

    sprintf(interp_result,"..MP.message[%s].sender[%s].receiver[%s].error[%s].",
            Log_msg_list[mp->message], Log_proc_list[mp->sender],
            Log_proc_list[mp->receiver], Log_mp_error[mp->error]);
    sprintf(msg_buffer,"\n..SMCM.command[%s].continuous[%d].state[%s].bit[%s].value[%d].",
            Log_sm_cmd_list[smcm->command], smcm->continuous,
            Log_sm_state[smcm->state],
            Log_io_bit_list[smcm->bit], smcm->value);
    strcat( interp_result, msg_buffer );

}

/////////////////////////////////////////////////
static void parse_pstamper_msg( GENERIC_MSG *gm )
{
    MESSAGE_PACKET *mp = (MESSAGE_PACKET *) &(gm->mp);
    PSCM *pscm = (PSCM *) &(gm->data[0]);

    sprintf(interp_result,"..MP.message[%s].sender[%s].receiver[%s].error[%s].",
            Log_msg_list[mp->message], Log_proc_list[mp->sender],
            Log_proc_list[mp->receiver], Log_mp_error[mp->error]);
    sprintf(msg_buffer,"\n..PSCM.command[%s].state[%d].",
            Log_ps_cmd_list[pscm->command], pscm->state);
    strcat( interp_result, msg_buffer );

}


///////////////////////////
// Command Message Builders
///////////////////////////

#if 0
/////////////////////////////////////////////////////////
static int build_io_msg(int interactive, GENERIC_MSG *gm,
              int argc, char *argv[])
{
    IOCM *iocm = (IOCM *) &(gm->data[0]);
    char *s;
    int ret_value;

    if ( ! interactive )
    {
        switch (argc)
        {
            case 3 :
                // io_reset, io_report
                //////////////////////
                iocm->command = atoi(argv[2]);
                iocm->bit = -1;
                iocm->value = -1;
                ret_value = TCL_OK;
                break;

            case 4 :
                // io_turn_off, io_turn_on, io_value_of
                ///////////////////////////////////////
                iocm->command = atoi(argv[2]);
                iocm->bit = atoi(argv[3]);
                iocm->value = -1;
                ret_value = TCL_OK;
                break;

            default :

                help_and_exit( io_help );
                ret_value = TCL_ERROR;
        }
        return ret_value;
    }

    // prompt for command parameters
    ////////////////////////////////
    fprintf(stdout,"Please input I/O command -> ");
    s = fgets( interp_result, sizeof(interp_result), stdin );
    iocm->command = atoi( s );

    switch (iocm->command)
    {
        case io_turn_off :
        case io_turn_on  :
        case io_value_of :
            fprintf(stdout,"Please input bit -> ");
            s = fgets( interp_result, sizeof(interp_result), stdin );
            iocm->bit = atoi( s );
            break;

        default        :
            iocm->bit = -1;
            break;
    }

    return TCL_OK;
}
#endif

/////////////////////////////////////////////////////////
static int build_ui_msg(int interactive, GENERIC_MSG *gm,
                              int argc, char *argv[])
{
    return TCL_ERROR;
}

///////////////////////////////////////////////////////////////
static int build_smachine_msg(int interactive, GENERIC_MSG *gm,
                              int argc, char *argv[])
{
    SMCM *smcm = (SMCM *) &(gm->data[0]);
    char *s;
    int ret_value;

    if ( ! interactive )
    {
        switch (argc)
        {
            case 3 :
                smcm->command = atoi(argv[2]);
                smcm->continuous = -1;
                smcm->state = state_last;
                smcm->bit = IO_LAST;
                smcm->value = -1;
                ret_value = TCL_OK;
                break;

            case 4 :
                smcm->command = atoi(argv[2]);
                smcm->continuous = atoi(argv[3]);
                smcm->state = state_last;
                smcm->bit = IO_LAST;
                smcm->value = -1;
                ret_value = TCL_OK;
                break;

            case 5 :
                smcm->command = atoi(argv[2]);
                smcm->continuous = -1;
                smcm->state = state_last;
                smcm->bit = atoi(argv[3]);
                smcm->value = atoi(argv[4]);
                ret_value = TCL_OK;
                break;

            default :

                help_and_exit( smachine_help );
                ret_value = TCL_ERROR;
                break;
        }

        return ret_value;

    }

    // prompt for command parameters
    ////////////////////////////////
    fprintf(stdout,"Please input State Machine command [0..4] -> ");
    s = fgets( interp_result, sizeof(interp_result), stdin );
    smcm->command = atoi( s );
    smcm->state = state_last;
    smcm->continuous = -1;
    smcm->bit = IO_LAST;
    smcm->value = -1;

    if (smcm->command == sm_start)
    {
        fprintf(stdout,"Please input Continuous flag [0|1] -> ");
        s = fgets( interp_result, sizeof(interp_result), stdin );
        smcm->continuous = atoi( s );
    } else if (smcm->command == sm_io_value_change) {
        fprintf(stdout,"Please input I/O bit to change [0..10] -> ");
        s = fgets( interp_result, sizeof(interp_result), stdin );
        smcm->bit = atoi( s );
        fprintf(stdout,"Please input I/O value [0|1] -> ");
        s = fgets( interp_result, sizeof(interp_result), stdin );
        smcm->value = atoi( s );
    }

    return TCL_OK;

}

///////////////////////////////////////////////////////////////
static int build_pstamper_msg(int interactive, GENERIC_MSG *gm,
                              int argc, char *argv[])
{
    PSCM *pscm = (PSCM *) &(gm->data[0]);
    char *s;

    if ( ! interactive )
    {
        if (argc != 4)
        {
            help_and_exit( pstamper_help );
            return TCL_ERROR;
        }
        gm->mp.receiver = atoi(argv[2]);
        pscm->command = atoi(argv[3]);
        pscm->state = -1;

        return TCL_OK;  
    }

    // prompt for command parameters
    ////////////////////////////////
    fprintf(stdout,"Please input Receiving Process [0..2] -> ");
    s = fgets( interp_result, sizeof(interp_result), stdin );
    gm->mp.receiver = atoi( s );

    fprintf(stdout,"Please input Part Stamper command [0..4] -> ");
    s = fgets( interp_result, sizeof(interp_result), stdin );
    pscm->command = atoi( s );

    pscm->state = -1;

    return TCL_OK;

}

/////////////////////////////////////////////////////////
int sendcmdCmd(ClientData clientData, Tcl_Interp *interp,
                 int argc, char* argv[])
{
    GENERIC_MSG *gm;
    int         interactive = 0;
    int         ret_value = TCL_ERROR;
    char        *s;

    // quick check on argc count
    ////////////////////////////
    if (argc < 2) {
        help_and_exit( sendcmd_help );
        return TCL_OK;
    }

    // allocate message buffer
    //////////////////////////
    gm = generic_msg_init( PS_GENERIC_MSG, __FILE__, __LINE__ );

    if (gm == NULL)
    {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].generic_msg_init return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__);
#endif

        return TCL_ERROR;

    } else {
        gm->mp.error = MP_NOERROR;
        gm->mp.message = atoi(argv[1]);
        if (argc == 2)
            interactive = 1;
    }

    switch (gm->mp.message)
    {
        case PS_COMMAND_MSG :
            ret_value = build_pstamper_msg(interactive, gm, argc, argv);
            break;

        case SM_COMMAND_MSG :
            gm->mp.receiver = PROC_smachine;
            ret_value = build_smachine_msg(interactive, gm, argc, argv);
            break;

        case UI_COMMAND_MSG :
            gm->mp.receiver = PROC_ui;
            ret_value = build_ui_msg(interactive, gm, argc, argv);
            break;

        default :

            generic_msg_free( gm, __FILE__, __LINE__ );
            return TCL_ERROR;
            break;
    }

    if (ret_value == TCL_OK)
    {
        // message should be good to go
        // STEP 1:  Send Command Msg
        // STEP 2:  Wait for reply
        ///////////////////////////////
        ret_value = msg_send( gm, __FILE__, __LINE__ );
        generic_msg_free( gm, __FILE__, __LINE__ );
        ret_value = TCL_OK;

        gm = msg_receive( PS_GENERIC_MSG, __FILE__, __LINE__ );
        if (gm == NULL)
        {

#ifdef LOG_ENABLE
    Log(LOG_ERROR,"[%s][%s][%d].msg_receive return NULL.",
            Log_proc_list[this_proc], __FILE__, __LINE__);
#endif

            return TCL_ERROR;

        }

        switch (gm->mp.message)
        {
            case PS_COMMAND_MSG :
                parse_pstamper_msg( gm );
                break;

            case SM_COMMAND_MSG :
                parse_smachine_msg( gm );
                break;

            case UI_COMMAND_MSG :
                parse_ui_msg( gm );
                break;

            default :

#ifdef LOG_ENABLE
    Log(LOG_DEBUG,"[%s][%s][%d].sendcmdCmd default on gm->mp.message[%d]).",
            Log_proc_list[this_proc], __FILE__, __LINE__, gm->mp.message);
#endif

                ret_value = TCL_ERROR;
                break;
        }

        interp->result = interp_result;

    }

    generic_msg_free( gm, __FILE__, __LINE__ );

    return ret_value;
}

/***** END OF FILE HERE *****/
/****************************/

