/****************************
 *
 * FILE : pstcl.c
 *
 * DESC : Part Stamper TCL process entry point
 *
 * DATE : 27.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: pstcl.c,v $
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
 *  Revision 1.5  2003/10/09 21:50:05  tmike
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
 *  Revision 1.4  2003/10/07 22:52:13  tmike
 *  fixed sendcmd.c build_sm_msg to support new command sm_io_value_change
 *  added state enumeration, general cleanup
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
 *  Revision 1.2  2003/09/29 14:17:10  tmike
 *  added sendcmd to pstcl tester
 *  
 *  Revision 1.1  2003/09/27 22:46:48  tmike
 *  TCL process added with rt and showsched commands
 *  add State Machine process
 *  add I/O monitor
 *  pgrabber process coded, not tested
 *  
 *
 ****************************/

/***** CONDITIONAL COMPILATION MACROS *****/
/******************************************/

/***** INCLUDE FILES *****/
/*************************/

#include <tcl8.6/tcl.h>

#include "message.h"
#include "process.h"
#include "memory.h"
#include "log.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static char *cvsid = "$Id: pstcl.c,v 1.8 2003/10/10 20:57:09 tmike Exp $";

PROC_LIST this_proc;
LogFlags *lfptr;
int *ioptr;

/***** EXTERNAL REFERNECES *****/
/*******************************/
extern int rtCmd(ClientData, Tcl_Interp *, int, char *[]);
extern int showschedCmd(ClientData, Tcl_Interp *, int, char *[]);

extern int sendcmdCmd(ClientData, Tcl_Interp *, int, char *[]);

/***** CODE STARTS HERE *****/
/****************************/

/////////////////
void pstcl_init()
{
    // identify myself
    //////////////////
    this_proc = PROC_tcl;

    // grab shm pointer to process_table
    // grab shm pointer to message_table
    ////////////////////////////////////
    ps_shm_init();
    memcpy(proc_table,mem_table[SHM_process].shm_ptr,PROC_SIZE);
    ps_msg_init();

#ifdef LOG_ENABLE
    Log(LOG_INIT,"[%s].EXITING pstcl_init().",Log_proc_list[this_proc]);
#endif

}

/*
* Tcl_AppInit is called from Tcl_Main
* after the Tcl interpreter has been created,
* and before the script file
* or interactive command loop is entered.
*/
///////////////////////////////////
int Tcl_AppInit(Tcl_Interp *interp)
{
    // Tcl_Init reads init.tcl from the Tcl script library
    //////////////////////////////////////////////////////
    if (Tcl_Init(interp) == TCL_ERROR) {
        return TCL_ERROR;
    }

    // Register application-specific commands
    /////////////////////////////////////////

    // RT module command : rt
    /////////////////////////
    Tcl_CreateCommand(interp, "rt", rtCmd,
            (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    // RT module command : showsched
    ////////////////////////////////
    Tcl_CreateCommand(interp, "showsched", showschedCmd,
            (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    // IPC MSG module command : sendcmd
    ///////////////////////////////////
    Tcl_CreateCommand(interp, "sendcmd", sendcmdCmd,
            (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    // Define the start-up filename. This file is read in
    // case the program is run interactively.
    /////////////////////////////////////////////////////
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.mytcl",
        TCL_GLOBAL_ONLY);

    // Link to Log control variables
    ////////////////////////////////
    Tcl_LinkVar(interp, "trace_flag", (char *) &(lfptr->trace_flag), TCL_LINK_INT);
    Tcl_LinkVar(interp, "msg_flag", (char *) &(lfptr->msg_flag), TCL_LINK_INT);
    Tcl_LinkVar(interp, "simulate_flag", (char *) &(lfptr->simulate_flag), TCL_LINK_INT);
    Tcl_LinkVar(interp, "library_flag", (char *) &(lfptr->library_flag), TCL_LINK_INT);
    Tcl_LinkVar(interp, "smachine_flag", (char *) &(lfptr->smachine_flag), TCL_LINK_INT);
    Tcl_LinkVar(interp, "pgrabber_flag", (char *) &(lfptr->pgrabber_flag), TCL_LINK_INT);
    Tcl_LinkVar(interp, "ppusher_flag", (char *) &(lfptr->ppusher_flag), TCL_LINK_INT);
    Tcl_LinkVar(interp, "pstamper_flag", (char *) &(lfptr->pstamper_flag), TCL_LINK_INT);

    return TCL_OK;
}

////////////////////////////
main(int argc, char *argv[])
{
    /*
     * Initialize your application here.
     *
     * Then initialize and run Tcl.
     */

    pstcl_init();

    Tcl_Main(argc, argv, Tcl_AppInit);

    exit(0);
}

/***** END OF FILE HERE *****/
/****************************/

