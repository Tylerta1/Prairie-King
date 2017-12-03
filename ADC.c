// Provide functions that initialize ADC0/ADC1
/*****************************************
WE ARE USING PORT E PIN 3 AND 4 TO GRAB 
ANALOG INPUT TO CONVERT TO DIGITAL.
ADC0 - ONE JOYSTICK THAT READS 2 INPUTS


*****************************************/
// Last Modified: 11/23/2017 
// Student names: Danny and Tyler

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// For Joystick, let
void ADC_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x10;      // 1) activate clock for Port E 
  while((SYSCTL_PRGPIO_R&0x10) == 0){};
  GPIO_PORTE_DIR_R &= ~0x0F;      // 2) make PE0-3 input
  GPIO_PORTE_AFSEL_R |= 0x0F;     // 3) enable alternate fun on PE0-3
  GPIO_PORTE_DEN_R &= ~0x0F;      // 4) disable digital I/O on PE0-3
  GPIO_PORTE_AMSEL_R |= 0x0F;     // 5) enable analog fun on PE0-3
  SYSCTL_RCGCADC_R |= 0x03;       // 6) activate ADC1-ADC0
  int delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
		
		
		
  ADC1_PC_R = 0x01;               // 7) configure for 125K 
  ADC1_SSPRI_R = 0x0123;          // 8) Seq 3 is highest priority
  ADC1_ACTSS_R &= ~0x000C;        // 9) disable sample sequencer 2-3
  ADC1_EMUX_R &= ~0xFF00;         // 10) seq2-3 is software trigger
  ADC1_SSMUX3_R = (ADC1_SSMUX3_R&0xFFFFFFF0);  // 11) Ain0(PE3)
	ADC1_SSMUX2_R = (ADC1_SSMUX2_R&0xFFFFFFF0)+1;  // 11) Ain1(PE2)
	ADC1_SSCTL2_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
	ADC1_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC1_IM_R &= ~0x000C;           // 13) disable SS2-SS3 interrupts
  ADC1_ACTSS_R |= 0x000C;         // 14) enable sample sequencer 2-3

		
	ADC0_PC_R = 0x01;               // 7) configure for 125K 
  ADC0_SSPRI_R = 0x0123;          // 8) Seq 3 is highest priority
  ADC0_ACTSS_R &= ~0x000C;        // 9) disable sample sequencer 2-3
  ADC0_EMUX_R &= ~0xFF00;         // 10) seq2 is software trigger
 	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+2;  // 11) Ain2(PE1)
	ADC0_SSMUX2_R = (ADC0_SSMUX2_R&0xFFFFFFF0)+3;  // 11) Ain3(PE0)
	ADC0_SSCTL2_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
	ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x000C;           // 13) disable SS2-SS3 interrupts
  ADC0_ACTSS_R |= 0x000C;         // 14) enable sample sequencer 2-3

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
void ADC_In(int *data){  
	ADC0_PSSI_R = 0x0008; //start ADC
	while((ADC0_RIS_R&0x08) == 0){ //busy-wait
	};
	data[0] = ADC0_SSFIFO3_R&0xFFF; // read data, PE4, data[0] has horizontal or vertical, TBD
	ADC0_ISC_R = 0x0008; //clear flag
	while((ADC0_RIS_R&0x04) == 0){ //busy-wait
	};
	data[1] = ADC0_SSFIFO2_R&0xFFF; // read data, PE3, data[1] has horizontal or vertical, TBD
	ADC0_ISC_R = 0x0004; //clear flag
}


