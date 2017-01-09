/****************************
 *
 * FILE : rt_rt.c
 *
 * DESC : rt TCL command handler
 *
 * DATE : 27.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: rt_rt.c,v $
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <tcl8.6/tcl.h>

#include "rt.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

static char *cvsid = "$Id: rt_rt.c,v 1.3 2003/09/30 05:01:18 tmike Exp $";

char *rtHelp[] = {
    "usage:  rt [-f] [-v] prio -p PID\n\n",
    "where:\n",
    "  -f sets the scheduling policy to SCHED_FIFO\n",
    "  -v turns on verbose flag\n",
    "   prio specifies the realtime priority\n",
    "  -p PID  specifies an existing process to modify\n",
    "SCHED_RR policy is used unless -f is specified or prio is 0.\n\n",
    NULL
};

int s_policy = SCHED_RR;
int s_priority = -1;
int s_pid = 0;
int verbose = 0;

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

/**************************/
int rtCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[])
{
    struct sched_param prio_struct;
    int error;

    /* check for no parameters */
    /***************************/
    if (argc == 1) {
        help_and_exit( rtHelp );
        return TCL_OK;
    } else
        error = parse_cl(argc, argv);

    if (error == TCL_ERROR)
        /* early return from command line parse */
        /****************************************/
        return TCL_OK;

    /* sanity checking... */
    /**********************/
    if ( (s_priority != 0) && (s_policy == SCHED_OTHER) ) {
        s_policy = SCHED_RR;
        if (verbose)
            printf("Defaulting sched policy to %s\n", POLICY(s_policy));
    }

    if ( (s_priority == 0 ) && (s_policy != SCHED_OTHER) ) {
        s_policy = SCHED_OTHER;
        if (verbose)
            printf("Defaulting sched policy to %s\n", POLICY(s_policy));
    }

    prio_struct.sched_priority = s_priority;

    if (verbose)
        printf("rt: scheduling PID[%d] at Priority[%d], policy is %s\n",
                s_pid, s_priority, POLICY(s_policy));

    if ( sched_setscheduler(s_pid, s_policy, &prio_struct) ) {
        help_and_exit( rtHelp );
        return TCL_OK;
    }
}

///////////////////////////////////
void help_and_exit( char *table[] )
{
    char **p=&table[0];

    while ( *p != NULL ) {
        printf( *p++ );
    }
}

/***** END OF FILE HERE *****/
/****************************/

