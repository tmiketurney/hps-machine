/****************************
 *
 * FILE : rt_parse.c
 *
 * DESC : rt TCL command parser
 *
 * DATE : 27.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: rt_parse.c,v $
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
#include <sched.h>
#include <tcl8.6/tcl.h>

#include "rt.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

typedef enum {
    State0_pre,
        State1_pri,
        State2_post
} Parse_State;

/***** FORWARD REFERENCES *****/
/******************************/

/***** LOCAL VARIABLES *****/
/***************************/

//MODULE_LICENSE("GPL");

static char *cvsid = "$Id: rt_parse.c,v 1.3 2003/09/30 05:01:18 tmike Exp $";

/***** EXTERNAL REFERNECES *****/
/*******************************/

extern char *rtHelp[];

/***** CODE STARTS HERE *****/
/****************************/

/***********************************/
int parse_cl(int argc, char *argv[])
{
    int i, length;
    Parse_State state = State0_pre;

    /* command line parsing */
    /************************/
    for (i=1; i<argc; i++) {
        
        /* first check for '-' parameters */
        /**********************************/
        if (argv[i][0] == '-') {
            switch (state) {

                case State0_pre:
                    switch (argv[i][1]) {
                        case 'f':
                            s_policy = SCHED_FIFO;
                            break;
                        case 'v':
                            verbose = 1;
                            break;
                        default:
                            help_and_exit( rtHelp );
                            return TCL_ERROR;
                            break;
                    }
                    break;

                case State1_pri:
                    switch (argv[i][1]) {
                        case 'p':
                            if (argc > i) {
                                s_pid = atoi(argv[++i]);
                                state = State2_post;
                            } else {
                                help_and_exit( rtHelp );
                                return TCL_ERROR;
                            }
                            break;
                        default:
                            help_and_exit( rtHelp );
                            return TCL_ERROR;
                            break;
                    }
                    break;


                default:
                    help_and_exit( rtHelp );
                    return TCL_ERROR;
                    break;
            }

        /* next argument better be a number... */
        /***************************************/
        } else if (isdigit(argv[i][0])) {

            s_priority = atoi(argv[i]);
            state = State1_pri;

        } else {
        
            help_and_exit( rtHelp );
            return TCL_ERROR;

        }

    }
    return TCL_OK;
}

/***** END OF FILE HERE *****/
/****************************/

