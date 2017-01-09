/****************************
 *
 * FILE : rt_showsched.c
 *
 * DESC : showsched TCL command handler
 *
 * DATE : 27.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: rt_showsched.c,v $
 *  Revision 1.2  2003/09/30 05:01:18  tmike
 *  Major Project Milestone
 *      All processes except UI coded and running
 *      All CMD messages except UI handled in pstcl
 *      Local library dependency fixed
 *      Add Copyright notice to all files
 *      expand -4 to all files
 *      State Machine needs coding
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

#define USE_INTERP_RESULT

/***** INCLUDE FILES *****/
/*************************/

#include <sched.h>
#include <stdlib.h>
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

//MODULE_LICENSE("GPL");

static char *cvsid = "$Id: rt_showsched.c,v 1.2 2003/09/30 05:01:18 tmike Exp $";

/***** EXTERNAL REFERNECES *****/
/*******************************/

/***** CODE STARTS HERE *****/
/****************************/

///////////////////////////////////////////////////////////
int showschedCmd(ClientData clientData, Tcl_Interp *interp,
                 int argc, char* argv[])
{
    struct sched_param p;
    pid_t pid;
    int policy;
    int prio;
    char * cpolicy;

    if (argc < 2) {
        pid = 0;
    } else {
        pid = atoi(argv[1]);
    }

    policy = sched_getscheduler(pid);
    sched_getparam(pid,&p);
    prio = p.sched_priority;

    cpolicy = POLICY(policy);

    sprintf(interp->result,"Process %d: %s at priority %d",pid,cpolicy,prio);
    return TCL_OK;
}

/***** END OF FILE HERE *****/
/****************************/

