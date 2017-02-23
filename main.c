/***************************************************************************************
 * MAIN.C
 *
 * Description: emulate ECAP using gpi pins
 *
 * Joe Altmaier
 * (C) 2016 360 Yield Center, Inc
 * 
 * Built with Code Composer Studio v6
 **************************************************************************************/

#include <stdint.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <pru_iep.h>
#include "rsc_table_pru.h"
#include "GpiECAP.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

void StartTimestampCounter();
void StopTimestampCounter();
uint32_t ReadTimestamp();

#if 0
	Start timer
	Loop
		read R31
		xor with last value => change
		xor last value w edgeUpDown mask
		and with change
		read TS
		for each bit
			if 1 - store TS into ECAP[bit][iMod4++]
	End loop

	special value:
		edgeUpDown mask
		ECAP table
		last value R31
#endif

T_ECAP *pEcap = (T_ECAP*)SHARED_MEMORY; // Start of shared memory

uint32_t last_sample = 0;
uint32_t tsLast[30];

int main(void)
{
	StartTimestampCounter();

	uint32_t ts = ReadTimestamp();

	pEcap->differential = 0x3FFF; // all differential
	pEcap->edgeUpDown = 0x3FFF; // all rising edge
	pEcap->sample = __R31;
	pEcap->edgeDetected = 0;

	uint32_t trigger = last_sample ^ pEcap->edgeUpDown;

	{ int i;
	for (i=0; i < 30; i++)
	{
		tsLast[i] = ts;
		pEcap->aICap[i] = 0;
		{ int j;
		for (j=0; j < 4; j++)
			pEcap->ECAP[i][j] = 0;
		}
	}}

	while (1)
	{
		uint32_t sample = __R31;
		uint32_t change = (sample ^ pEcap->sample) & 0x3FFFFFFF;

		if (change)
		{
			// Calculate which bits have seen a desired edge (it changed, and we trigger on that change)
			uint32_t edge = change & trigger;

			if (edge)
			{
				uint32_t ts = ReadTimestamp();

				int bit = 1;
				int iBit = 0;
				while (iBit < 30)
				{
					if (edge & bit)
					{
						// Store the ts (or the difference in ts) into the capture table for this bit
						//  and increment the slot index (aICap)
						pEcap->ECAP[iBit][pEcap->aICap[iBit]++] = (pEcap->differential & bit)? (ts - tsLast[iBit]) :ts;
						pEcap->aICap[iBit] %= 4;

						// Next time we calculate the difference from this time
						tsLast[iBit] = ts;
					}
					iBit++;
					bit <<= 1;
				}

				pEcap->edgeDetected |= edge;
			}

			pEcap->sample = sample;
			trigger = sample ^ pEcap->edgeUpDown;
		}
	}

	StopTimestampCounter();

	/* Halt the PRU core */
	__halt();

	/* Should never return */
	return 0;
}

void StartTimestampCounter()
{
	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* Disable counter */
	CT_IEP.TMR_GLB_CFG_bit.CNT_ENABLE = 0;

	/* Reset Count register */
	CT_IEP.TMR_CNT = 0x0;

	/* Clear overflow status register */
	CT_IEP.TMR_GLB_STS_bit.CNT_OVF = 0x1;

	CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 0x1;
	CT_IEP.TMR_GLB_CFG_bit.CMP_INC = 0x1;

	/* Disable compensation */
	CT_IEP.TMR_COMPEN_bit.COMPEN_CNT = 0x0;

	/* Clear the status of all interrupts */
	CT_INTC.SECR0 = 0xFFFFFFFF;
	CT_INTC.SECR1 = 0xFFFFFFFF;

	/* Enable counter */
	CT_IEP.TMR_GLB_CFG = 0x11;
}

void StopTimestampCounter()
{
	/* Disable counter */
	CT_IEP.TMR_GLB_CFG_bit.CNT_ENABLE = 0x0;
}

uint32_t ReadTimestamp()
{
	return CT_IEP.TMR_CNT;
}
