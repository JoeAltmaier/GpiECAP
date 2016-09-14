/*
 * GpiECAP.h
 *
 *  Created on: Jul 6, 2016
 *      Author: Joe
 */

#ifndef GPIECAP_H_
#define GPIECAP_H_

typedef struct {
			uint32_t edgeUpDown; // trigger on rising edge (1) or falling edge (0)
			uint32_t differential; // capture ts (false) or dTs(true)
			uint32_t sample;
			uint32_t edgeDetected;
			uint32_t ECAP[30][4];
			int aICap[32];
		} T_ECAP;

#define SHARED_MEMORY 0x00010000;


#endif /* GPIECAP_H_ */
