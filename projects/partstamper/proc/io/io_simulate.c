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

static char *cvsid = "$Id: io_simulate.c,v 1.3 2003/10/07 22:49:05 tmike Exp $";

/***** EXTERNAL REFERNECES *****/
/*******************************/

extern IO_TABLE io_table[IO_LAST];

/***** CODE STARTS HERE *****/
/****************************/

//////////////////////////////////
int io_simulate( GENERIC_MSG *gm )
{
    IOCM *iocm = (IOCM *) &(gm->data[0]);

    Log(LOG_SIMULATE,"[%d].io_simulate( command[%d].bit[%d] ).",
        this_proc, iocm->command, iocm->bit);

    switch (iocm->bit)
    {
        case cylAretracted :
        case cylAextended  :
        case cylBretracted :
        case cylBextended  :
        case cylCretracted :
        case cylCextended  :
            break;

        case cylAsolenoid :
            if (iocm->command == io_turn_off)
            {
                io_table[cylAretracted].value = 1;
                io_table[cylAextended].value  = 0;
            } else {
                io_table[cylAretracted].value = 0;
                io_table[cylAextended].value  = 1;
            }
            break;

        case cylBsolenoid :
            if (iocm->command == io_turn_off)
            {
                io_table[cylBretracted].value = 1;
                io_table[cylBextended].value  = 0;
            } else {
                io_table[cylBretracted].value = 0;
                io_table[cylBextended].value  = 1;
            }
            break;

        case cylCsolenoid :
            if (iocm->command == io_turn_off)
            {
                io_table[cylCretracted].value = 1;
                io_table[cylCextended].value  = 0;
            } else {
                io_table[cylCretracted].value = 0;
                io_table[cylCextended].value  = 1;
            }
            break;

        case airpressure :
            break;

        case estop :
            break;

        default :
            Log(LOG_DEBUG,"[%d][%s][%d].io_simulate(default : bit[%d]).",
                    this_proc, __FILE__, __LINE__, iocm->bit);
            return 1;
            break;
    }
    return 0;
}

/***** END OF FILE HERE *****/
/****************************/

