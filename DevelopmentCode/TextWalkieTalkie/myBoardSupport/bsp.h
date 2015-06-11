/*****************************************************************************
* Product: "Blinky" example
* Last Updated for Version: 5.4.0
* Date of the Last Update:  2015-03-07
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. state-machine.com.
*
* This program is open source software: you can redistribute it and/or
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
* Contact information:
* Web  : http://www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#ifndef bsp_h
#define bsp_h

#define BSP_TICKS_PER_SEC    100U

void BSP_init(void);
void BSP_ledAOff(void);
void BSP_ledAOn (void);
void BSP_ledBOff(void);
void BSP_ledBOn (void);
void BSP_ledCOff(void);
void BSP_ledCOn (void);
void BSP_ledDOff(void);
void BSP_ledDOn (void);
void BSP_ledEOff(void);
void BSP_ledEOn (void);

void BSP_led0Off(void);
void BSP_led0On (void);
void BSP_led1Off(void);
void BSP_led1On (void);
void BSP_led2Off(void);
void BSP_led2On (void);
void BSP_led3Off(void);
void BSP_led3On (void);
void BSP_led4Off(void);
void BSP_led4On (void);
void BSP_led5Off(void);
void BSP_led5On (void);
void BSP_led6Off(void);
void BSP_led6On (void);
void BSP_led7Off(void);
void BSP_led7On (void);


void BSP_buttonR0Off(void);
void BSP_buttonR0On(void);

void BSP_buttonR1(void);
void BSP_buttonR2(void);
void BSP_buttonR3(void);

int BSP_buttonCS1(void);
int BSP_buttonCS2(void);
int BSP_buttonCS3(void);
int BSP_buttonCS4(void);


#endif /* bsp_h */
