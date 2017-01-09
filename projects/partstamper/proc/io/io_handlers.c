/****************************
 *
 * FILE : io_handlers.c
 *
 * DESC : I/O command handler entry points
 *
 * DATE : 26.September.2003     Copyright (c) Tools Made Tough
 *
 * CVS :
 *
 *  $Log: io_handlers.c,v $
 *  Revision 1.5  2003/10/07 22:49:05  tmike
 *  added SM_COMMAND_MSG to airpressure and estop
 *
 *  Revision 1.4  2003/09/30 21:14:59  tmike
 *  minor cleanup
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
 *  Revision 1.2  2003/09/29 18:53:51  tmike
 *  TCL sendcmd working for IO_CMD_MESSAGE
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

#include "message.h"
#include "process.h"
#include "log.h"

#include "io.h"
#include "smachine.h"

/***** MACROS *****/
/******************/

/***** TYPEDEFS *****/
/********************/

/***** FORWARD REFERENCES *****/
/******************************/

/***** EXTERNAL REFERNECES *****/
/*******************************/

extern int io_simulate( GENERIC_MSG * );

/***** LOCAL VARIABLES *****/
/***************************/

static char *cvsid = "$Id: io_handlers.c,v 1.5 2003/10/07 22:49:05 tmike Exp $";

IO_TABLE io_table[IO_LAST];

static IO_TABLE io_table_reset[IO_LAST] = {
    { 1, NULL },        // cylAretracted
    { 0, NULL },        // cylAextended
    { 0, io_simulate }, // cylAsolenoid
    { 1, NULL },        // cylBretracted
    { 0, NULL },        // cylBextended
    { 0, io_simulate }, // cylBsolenoid
    { 1, NULL },        // cylCretracted
    { 0, NULL },        // cylCextended
    { 0, io_simulate }, // cylCsolenoid
    { 0, io_simulate }, // airpressure
    { 0, io_simulate }  // estop
};

/***** CODE STARTS HERE *****/
/****************************/

/////////////////////////////////////
void h_io_turn_off( GENERIC_MSG *igm )
{
    IOCM *iocm = (IOCM *) &(igm->data[0]);
    int ret_value;
    GENERIC_MSG *ogm;
    SMCM *smcm;

    if (io_table[iocm->bit].activator)
        ret_value = ( * io_table[iocm->bit].activator) ( igm );

    if (ret_value == 0)
    {
        io_table[iocm->bit].value = 0;
        
        if ( (iocm->bit == airpressure) || (iocm->bit == estop) )
        {
            ogm = generic_msg_init( SM_COMMAND_MSG );
            if (ogm == NULL)
            {
                Log(LOG_ERROR,"[%d][%s][%d].generic_msg_init return NULL.",
                        this_proc, __FILE__, __LINE__);
                igm->mp.error = MP_NULLRETURN;
            } else {
                smcm = (SMCM *) &(ogm->data[0]);
                smcm->command = sm_io_value_change;
                smcm->iobl = iocm->bit;
                smcm->value = 0;
                ogm->mp.receiver = PROC_smachine;
                ogm->mp.message = SM_COMMAND_MSG;
                msg_send( ogm );
                generic_msg_free( ogm );
                ogm = msg_receive( SM_COMMAND_MSG );
                generic_msg_free( ogm );
            }
        }
    } else {
        Log(LOG_ERROR,"[%d][%s][%d].h_io_turn_off bit[%d] activator failed.",
                this_proc, __FILE__, __LINE__, iocm->bit);
    }
}

////////////////////////////////////
void h_io_turn_on( GENERIC_MSG *igm )
{
    IOCM *iocm = (IOCM *) &(igm->data[0]);
    int ret_value;
    GENERIC_MSG *ogm;
    SMCM *smcm;

    if (io_table[iocm->bit].activator)
        ret_value = ( * io_table[iocm->bit].activator) ( igm );

    if (ret_value == 0)
    {
        io_table[iocm->bit].value = 1;

        if ( (iocm->bit == airpressure) || (iocm->bit == estop) )
        {
            ogm = generic_msg_init( SM_COMMAND_MSG );
            if (ogm == NULL)
            {
                Log(LOG_ERROR,"[%d][%s][%d].generic_msg_init return NULL.",
                        this_proc, __FILE__, __LINE__);
                igm->mp.error = MP_NULLRETURN;
            } else {
                smcm = (SMCM *) &(ogm->data[0]);
                smcm->command = sm_io_value_change;
                smcm->iobl = iocm->bit;
                smcm->value = 1;
                ogm->mp.receiver = PROC_smachine;
                ogm->mp.message = SM_COMMAND_MSG;
                msg_send( ogm );
                generic_msg_free( ogm );
                ogm = msg_receive( SM_COMMAND_MSG );
                generic_msg_free( ogm );
            }
        }
    } else {
        Log(LOG_ERROR,"[%d][%s][%d].h_io_turn_on bit[%d] activator failed.",
                this_proc, __FILE__, __LINE__, iocm->bit);
    }
}

/////////////////////////////////////
void h_io_value_of( GENERIC_MSG *gm )
{
    IOCM *iocm = (IOCM *) &(gm->data[0]);
    int ret_value = 0;

    if (io_table[iocm->bit].activator)
        ret_value = ( * io_table[iocm->bit].activator) ( gm );

    if (ret_value == 0)
        iocm->value = io_table[iocm->bit].value;
    else {
        Log(LOG_ERROR,"[%d][%s][%d].h_io_value_of bit[%d] activator failed.",
                this_proc, __FILE__, __LINE__, iocm->bit);
    }
}

///////////////////////////////////
void h_io_report( GENERIC_MSG *gm )
{
    IOCM *iocm = (IOCM *) &(gm->data[0]);
    int i;

    for (i=0; i<IO_LAST; i++)
        iocm->report[i] = io_table[i].value;
}

//////////////////////////////////
void h_io_reset( GENERIC_MSG *gm )
{
    int i;
    for (i=0; i<IO_LAST; i++)
    {
        io_table[i].value = io_table_reset[i].value;
        io_table[i].activator = io_table_reset[i].activator;
    }
}

/***** END OF FILE HERE *****/
/****************************/

