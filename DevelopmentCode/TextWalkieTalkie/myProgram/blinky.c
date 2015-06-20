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
#include "LEDFunctions.h"
#include "ButtonFunctions.h"


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

static QState Blinky_2     (Blinky * const me, QEvt const * const e);
static QState Blinky_3     (Blinky * const me, QEvt const * const e);


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
					
					LEDAllOff();
					
					LEDXYOnSingle(0,0);
					if(ButtonCheckPress(0,0) == 1)
						LEDXYOnSingle(3,0);

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
					
					LEDAllOff();
					LEDXYOnSingle(1,0);
					if(ButtonCheckPress(1,0) == 1)
						LEDXYOnSingle(4,0);

            status = Q_HANDLED();

            break;

        }

        case TIMEOUT_SIG: 
        {

            status = Q_TRAN(&Blinky_2);


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

QState Blinky_2(Blinky * const me, QEvt const * const e) 
{

    QState status;

    switch (e->sig) 
    {

        case Q_ENTRY_SIG: 
        {
					
					LEDAllOff();
					LEDXYOnSingle(1,1);
					if(ButtonCheckPress(1,1) == 1)
						LEDXYOnSingle(4,1);
					
            status = Q_HANDLED();

            break;

        }

        case TIMEOUT_SIG: 
        {

            status = Q_TRAN(&Blinky_3);


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

QState Blinky_3(Blinky * const me, QEvt const * const e) 
{

    QState status;

    switch (e->sig) 
    {

        case Q_ENTRY_SIG: 
        {

					LEDAllOff();
					LEDXYOnSingle(0,1);
					if(ButtonCheckPress(0,1) == 1)
						LEDXYOnSingle(3,1);

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

