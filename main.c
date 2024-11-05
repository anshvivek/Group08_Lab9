#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

int main(void)
{
    
    SYSCTL_RCGCGPIO_R |= 0x20;
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTF_CR_R = 0x1F;             /* make PORTF configurable */
    GPIO_PORTF_DEN_R = 0x1F;            /* set PORTF pins 4 : 0 pins */
    GPIO_PORTF_DIR_R = 0x0E;            /*  */
    GPIO_PORTF_PUR_R = 0x11;            /* PORTF0 and PORTF4 are pulled up */

    SYSCTL_RCGCGPIO_R |= 0x02;
    GPIO_PORTB_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTB_DEN_R = 0x0C;
    GPIO_PORTB_AFSEL_R = 0x0C;
    GPIO_PORTB_ODR_R = 0x08;
    GPIO_PORTB_PCTL_R = 0x3300;
    GPIO_PORTB_PUR_R = 0x0C;
	
    SYSCTL_RCGCGPIO_R |= 0x01;
    GPIO_PORTA_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTA_DEN_R = 0xC0;
    GPIO_PORTA_AFSEL_R = 0xC0;
    GPIO_PORTA_ODR_R = 0x80;
    GPIO_PORTA_PCTL_R = 0x33000000;

    SYSCTL_RCGCI2C_R = 0x03;

    // rx
    I2C1_MCR_R = 0x20;
    I2C1_SOAR_R = 0x3B;
    I2C1_SCSR_R = 0x01;

    // tx
    I2C0_MCR_R = 0x10;
    I2C0_MTPR_R = 0x09;
    I2C0_MSA_R = 0x76;

    while(1){

        I2C0_MDR_R = 0xAF;


        while(I2C0_MCS_R & 0x40){
            ;
        }
        I2C0_MCS_R = 0x07; // start transmit

        while (I2C0_MCS_R & 0x04){ 
            ;// check for error
        }
        if ((I2C0_MCS_R & 0x04) == 0x00){

            ;
        }
        else if ((I2C0_MCS_R & 0x04) != 0x00){
            GPIO_PORTF_DATA_R = 0x02;
        }
////////////////////////////////////////////////////////////////////////////////////////////////////
        while((I2C1_SCSR_R & 0x01) == 0){
            ;// wait for RREQ
        }
        if(I2C1_SDR_R == 0xAF){
            GPIO_PORTF_DATA_R = 0x04;
        }
    }
    return 0;
}
