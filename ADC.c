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
	SYSCTL_RCGCGPIO_R |= 0x10; //Port E clock turned on
	while((SYSCTL_RCGCGPIO_R&0x10) == 0){}; //delay
	GPIO_PORTE_DIR_R &= ~0x18;  //PE4/PE3 Input
	GPIO_PORTE_AFSEL_R |= 0x18; // Enable alternate for PE4/PE3
	GPIO_PORTE_DEN_R &= ~0x18; // Disable digital I/O for PE4/PE3
	GPIO_PORTE_AMSEL_R |= 0x18; // Turn on analog for PE4/PE3
	
	SYSCTL_RCGCADC_R |= 0x01; // Activate ADC0/1
	while((SYSCTL_RCGCADC_R&0x03) == 0){} //delay
	
	ADC0_PC_R &= ~0xF;    // clear max sample rate field
	ADC0_PC_R |= 0x01; 		//configure for 125K
	ADC0_SSPRI_R = 0x0123; //Seq 3 is highest priority
	ADC0_ACTSS_R &= ~0x000C; //disable sample sequencer 2-3
	ADC0_EMUX_R &= ~0xFF00;  //seq2-3 is software trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R&0xFFFFFFF0)+9; // Ain9(PE4)
 	ADC0_SSMUX2_R = (ADC0_SSMUX2_R&0xFFFFFFF0);  // 11) Ain0(PE3)
	ADC0_SSCTL3_R = 0x0006; // no Ts0 D0, yes IE0, END0
	ADC0_SSCTL2_R = 0x0006; // no TS0 D0, yes IE0 END0
	ADC0_SAC_R |= 0x6; //Averaging hardware
	ADC0_IM_R &= ~0x000C; //disable SS2-3 interrupts
	ADC0_ACTSS_R |= 0x000C; //enable sample sequencer 2-3
		

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
	uint32_t data;
	ADC0_PSSI_R = 0x0008; //start ADC
	while((ADC0_RIS_R&0x08) == 0){ //busy-wait
	};
	data = ADC0_SSFIFO3_R&0xFFF; // read data
	ADC0_ISC_R = 0x0008; //clear flag
  return data; //return data
}


