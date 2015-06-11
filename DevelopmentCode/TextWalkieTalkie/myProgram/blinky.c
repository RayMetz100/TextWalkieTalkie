/*****************************************************************************
* 
Product: Simple Blinky example
* 
Last Updated for Version: 5.4.0
* 
Date of the Last Update:  2015-03-16
*
*                    
Q u a n t u m     L e a P s
*                    ---------------------------
*                    
innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
*
* 
This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* 
Contact information:
* Web:   www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/


#include "qpc.h"

#include "blinky.h"

#include "bsp.h"

//Q_DEFINE_THIS_FILE



/*..........................................................................*/


typedef struct {     
	/* the Blinky active object */
    
	QActive super;   
	/* inherit QActive */

    
	QTimeEvt timeEvt; 
	/* private time event generator */

} 
Blinky;



static Blinky l_blinky; 

/* the Blinky active object */


QActive * const AO_Blinky = &l_blinky.super;



/* hierarchical state machine ... */

static QState Blinky_initial(Blinky * const me, QEvt const * const e);

static QState Blinky_0     (Blinky * const me, QEvt const * const e);
static QState Blinky_1     (Blinky * const me, QEvt const * const e);

static QState Blinky_4     (Blinky * const me, QEvt const * const e);
static QState Blinky_5     (Blinky * const me, QEvt const * const e);


/*..........................................................................*/


void Blinky_ctor(void) {

    Blinky * const me = &l_blinky;

    QActive_ctor(&me->super, Q_STATE_CAST(&Blinky_initial));

    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0U);

}



/* HSM definition ----------------------------------------------------------*/

QState Blinky_initial(Blinky * const me, QEvt const * const e) 
{

    (void)e; 

    /* avoid compiler warning about unused parameter */


    QS_OBJ_DICTIONARY(&l_blinky);

    QS_OBJ_DICTIONARY(&l_blinky.timeEvt);


    QS_FUN_DICTIONARY(&QHsm_top);

    QS_FUN_DICTIONARY(&Blinky_initial);

    QS_FUN_DICTIONARY(&Blinky_off);

    QS_FUN_DICTIONARY(&Blinky_on);
    QS_SIG_DICTIONARY(DUMMY_SIG,     (void *)0);

    /* global signal */

    QS_SIG_DICTIONARY(TIMEOUT_SIG,   me);  

    /* signal just for Blinky */



    /* arm the time event to expire in half a second and every half second */


    QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC/4U, BSP_TICKS_PER_SEC/4U);


    return Q_TRAN(&Blinky_0);

}


/*..........................................................................*/

QState Blinky_0(Blinky * const me, QEvt const * const e) 
{
    QState status;


    switch (e->sig) {
        case Q_ENTRY_SIG: {
					
				  	BSP_buttonR0On();
					
					  if(BSP_buttonCS1() == 1)
              BSP_ledAOn();
						else
							BSP_ledAOff();
            BSP_ledBOff();
            BSP_ledCOff();
            BSP_ledDOff();
            BSP_ledEOff();
					
					  if(BSP_buttonCS1() == 1)
              BSP_led0On();
						else
							BSP_led0Off();
            BSP_led1Off();
            BSP_led2Off();
            BSP_led3Off();
            BSP_led4Off();
            BSP_led5Off();
            BSP_led6Off();
            BSP_led7Off();

            status = Q_HANDLED();

            break;

        }
        case TIMEOUT_SIG: {
            status = Q_TRAN(&Blinky_1);

            break;

        }
        default: {
            status = Q_SUPER(&QHsm_top);

            break;

        }
    }
    
return status;
}



/*..........................................................................*/

QState Blinky_1(Blinky * const me, QEvt const * const e) 
{

    QState status;

    switch (e->sig) 
    {

        case Q_ENTRY_SIG: 
        {
				  	BSP_buttonR0Off();

  					BSP_ledAOff();
            BSP_ledBOff();
            BSP_ledCOff();
            BSP_ledDOn();
            BSP_ledEOn();
					
            BSP_led0On();
            BSP_led1On();
            BSP_led2Off();
            BSP_led3Off();
            BSP_led4Off();
            BSP_led5Off();
            BSP_led6Off();
            BSP_led7Off();


            status = Q_HANDLED();

            break;

        }

        case TIMEOUT_SIG: 
        {

            status = Q_TRAN(&Blinky_4);


            break;

        }

        default: 
        {

            status = Q_SUPER(&QHsm_top);

            break;

        }

    }

    return status;

}

QState Blinky_4(Blinky * const me, QEvt const * const e) 
{

    QState status;

    switch (e->sig) 
    {

        case Q_ENTRY_SIG: 
        {
            BSP_ledAOff();
            BSP_ledBOff();
            BSP_ledCOn();
            BSP_ledDOn();
            BSP_ledEOn();
					
            BSP_led0Off();
            BSP_led1Off();
            BSP_led2Off();
            BSP_led3Off();
            BSP_led4Off();
            BSP_led5On();
            BSP_led6On();
            BSP_led7On();


            status = Q_HANDLED();

            break;

        }

        case TIMEOUT_SIG: 
        {

            status = Q_TRAN(&Blinky_5);


            break;

        }

        default: 
        {

            status = Q_SUPER(&QHsm_top);

            break;

        }

    }

    return status;

}

QState Blinky_5(Blinky * const me, QEvt const * const e) 
{

    QState status;

    switch (e->sig) 
    {

        case Q_ENTRY_SIG: 
        {
            BSP_ledAOn();
            BSP_ledBOn();
            BSP_ledCOn();
            BSP_ledDOn();
            BSP_ledEOff();
					
            BSP_led0Off();
            BSP_led1Off();
            BSP_led2Off();
            BSP_led3Off();
            BSP_led4On();
            BSP_led5On();
            BSP_led6On();
            BSP_led7On();


            status = Q_HANDLED();

            break;

        }

        case TIMEOUT_SIG: 
        {

            status = Q_TRAN(&Blinky_0);


            break;

        }

        default: 
        {

            status = Q_SUPER(&QHsm_top);

            break;

        }

    }

    return status;

}

