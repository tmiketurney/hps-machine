/****************************
 *
 * FILE : log.c
 *
 * DESC : application logging library
 *
 * DATE : 26.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: log.c,v $
 *  Revision 1.8  2003/10/10 20:57:09  tmike
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
 *  Revision 1.7  2003/10/10 05:44:41  tmike
 *  State Machine appears functional (need to detail review Log file)
 *      I/O task removed
 *      I/O table in SHM
 *      LogFlags in SHM
 *
 *  Revision 1.6  2003/10/09 22:57:39  tmike
 *  Log file generated, with single Tcl command
 *  to StateMachine process : return state
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
 *  Revision 1.4  2003/10/07 22:48:18  tmike
 *  Removed dump_msg_table, general cleanup
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
 *  Revision 1.2  2003/09/29 14:17:09  tmike
 *  added sendcmd to pstcl tester
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
#include <time.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/timeb.h>

#include "message.h"
#include "process.h"
#include "memory.h"
#include "log.h"
#include "io.h"

/***** MACROS *****/
/******************/

#define LOC_BUF_SIZE    4096

//#define LOG_PATH "/var/log/partstamper/"
#define LOG_PATH "./"

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static char *cvsid = "$Id: log.c,v 1.8 2003/10/10 20:57:09 tmike Exp $";

#ifdef LOG_ENABLE

// local throwaway buffer
static char localbuffer[LOC_BUF_SIZE];

// filename + path buffer
static char filename[0x1000];

LogFlags logflags;

// Boolean variables that control dynamic tracing
/////////////////////////////////////////////////
LogFlags logflags_reset = {
    1,      // trace_flag
    1,      // msg_flag
    1,      // simulate_flag
    1,      // library_flag
    1,      // smachine_flag
    1,      // pgrabber_flag
    1,      // ppusher_flag
    1       // pstamper_flag
};

char *Log_io_bit_list[] = {
    "cylAretracted",
    "cylAextended",
    "cylAsolenoid",
    "cylBretracted",
    "cylBextended",
    "cylBsolenoid",
    "cylCretracted",
    "cylCextended",
    "cylCsolenoid",
    "airpressure",
    "estop",
    "io_last"
};

char *Log_io_cmd_list[] = {
    "io_turn_off",
    "io_turn_on",
    "io_value_of",
    "io_report",
    "io_reset",
    "io_cmd_last"
};

char *Log_sm_state[] = {
    "state_quiet",
    "state_ehardware",
    "state_grab",
    "state_push",
    "state_stamp",
    "state_stamp_grab",
    "state_last"
};

char *Log_sm_cmd_list[] = {
    "sm_start",
    "sm_stop",
    "sm_return_state",
    "sm_reset",
    "sm_io_value_change",
    "sm_cmd_last"
};

char *Log_ps_cmd_list[] = {
    "ps_grab_part",
    "ps_push_part",
    "ps_stamp_part",
    "ps_return_state",
    "ps_reset",
    "ps_cmd_last"
};

char *Log_ui_cmd_list[] = {
    "ui_reset",
    "ui_return_state",
    "ui_cmd_last"
};

char *Log_msg_list[] = {
    "PS_COMMAND_MSG",
    "SM_COMMAND_MSG",
    "UI_COMMAND_MSG",
    "PS_GENERIC_MSG",
    "LAST_MSG"
};

char *Log_mp_error[] = {
    "MP_NOERROR",
    "MP_NOMESSAGE",
    "MP_NOSENDER",
    "MP_NORECEIVER",
    "MP_BADCOMMAND",
    "MP_NULLRETURN",
    "MP_UNHANDLEDMSG",
    "MP_BADSTATE",
    "MP_LAST"
};

char *Log_proc_list[] = {
    "PROC_pgrabber",
    "PROC_ppusher",
    "PROC_pstamper",
    "PROC_smachine",
    "PROC_ui",
    "PROC_tcl",
    "PROC_main",
    "LAST_PROC"
};

char *Log_shm_list[] = {
    "SHM_process",
    "SHM_log",
    "SHM_io",
    "LAST_SHM"
};

#endif

/***** EXTERNAL REFERENCES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

///////////////////////////////////////////
void Log ( LOG_LIST ll, char *format, ... )
{
    va_list     args;

    // file variables
    FILE        *lfFp;

    // time variables
    struct tm   *lt;
    struct timeb    tstruct;
    time_t      epoch;

    memset(localbuffer, '\0', sizeof(localbuffer));

    switch (ll) {
        case LOG_SMACHINE :
            strcpy(localbuffer,"[ SMACHINE]");
            break;

        case LOG_PGRABBER :
            strcpy(localbuffer,"[ PGRABBER]");
            break;

        case LOG_PPUSHER :
            strcpy(localbuffer,"[  PPUSHER]");
            break;

        case LOG_PSTAMPER :
            strcpy(localbuffer,"[ PSTAMPER]");
            break;

        case LOG_LIBRARY :
            strcpy(localbuffer,"[  LIBRARY]");
            break;

        case LOG_TRACE :
            strcpy(localbuffer,"[    TRACE]");
            break;

        case LOG_ERROR :
            strcpy(localbuffer,"[    ERROR]");
            break;

        case LOG_DEBUG :
            strcpy(localbuffer,"[    DEBUG]");
            break;

        case LOG_MSG :
            strcpy(localbuffer,"[      MSG]");
            break;

        case LOG_SIMULATE :
            strcpy(localbuffer,"[ SIMULATE]");
            break;

        case LOG_INIT :
            strcpy(localbuffer,"[     INIT]");
            break;

        default :
            strcpy(localbuffer,"[  DEFAULT]");
            break;
    }

    // get start of epoch time
    //////////////////////////
    time( &epoch );
    ftime( &tstruct );

    // convert to localtime
    ///////////////////////
    lt = localtime( &epoch );

    // build date buffer (used for filename)
    ////////////////////////////////////////
    memset(filename, '\0', sizeof(filename));
    sprintf(filename, "%s%d%.2d%.2d.log", LOG_PATH, lt->tm_year+1900, lt->tm_mon + 1, lt->tm_mday);

    // build timestamp log entry format buffer
    //////////////////////////////////////////
    sprintf(localbuffer, "%s%.2d:%.2d:%.2d:%.3d:", localbuffer, lt->tm_hour, lt->tm_min, lt->tm_sec, tstruct.millitm);
    sprintf(localbuffer, "%s  %s\n", localbuffer, format);

    // open log file for append
    ///////////////////////////
    lfFp = fopen( filename, "a+" );
    if (lfFp == NULL)
    {
        perror("Log file fopen");
        return;
    } else {
        va_start( args, format );
        vfprintf( lfFp, localbuffer, args );
        fflush( lfFp );
        fclose( lfFp );
        va_end( args );
    }
}

/////////////////////////////
void dump_proc_table(char *s)
{
#ifdef LOG_ENABLE
    int i;
    PROCESS_TABLE *pt = &proc_table[0];

    for (i=0; i<LAST_PROC; i++) {
        Log(LOG_INIT,"[%s]..i[%d].proc_id[%d].que_id[%x].que_key[%x].que_flags[%x].msg_size[%d].message[%d]..",
                Log_proc_list[this_proc], i, pt->proc_id,
                pt->que_id, pt->que_key, pt->que_flags,
                pt->msg_size, pt->message);
        pt++;
    }
#endif
}

/***** END OF FILE HERE *****/
/****************************/

