/****************************
 *
 * FILE : rt.h
 *
 * DESC : RT TCL module interface, supports these commands:
 *      rt
 *      showsched
 *
 * DATE : 27.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: rt.h,v $
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

/***** MACROS *****/
/******************/

#define POLICY(x)  x ? x-1 ? "SCHED_RR" : "SCHED_FIFO" : "SCHED_OTHER"

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

extern void help_and_exit( char ** );
extern int parse_cl(int, char *[]);

/***** EXTERNAL REFERNECES *****/
/*******************************/

extern int s_policy;
extern int s_priority;
extern int s_pid;
extern int verbose;

/***** END OF FILE HERE *****/
/****************************/

