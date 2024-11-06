#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

void ISR();
// void I2CSlaveHandler();

uint32_t TxData = 0x0000;

int main(void)
{
    //Port F
    SYSCTL_RCGCGPIO_R |= 0x20;
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTF_CR_R = 0x1F;             /* make PORTF configurable */
    GPIO_PORTF_DEN_R = 0x1F;            /* set PORTF pins 4 : 0 pins */
    GPIO_PORTF_DIR_R = 0x0E;            /*  */
    GPIO_PORTF_PUR_R = 0x11;            /* PORTF0 and PORTF4 are pulled up */
    //Port B
    SYSCTL_RCGCGPIO_R |= 0x02;
    GPIO_PORTB_LOCK_R = 0x4C4F434B;     
    GPIO_PORTB_DEN_R = 0x0C;
    GPIO_PORTB_AFSEL_R = 0x0C;
    GPIO_PORTB_ODR_R = 0x08;
    GPIO_PORTB_PCTL_R = 0x3300;
    GPIO_PORTB_PUR_R = 0x0C;
    //Port A
    SYSCTL_RCGCGPIO_R |= 0x01;
    GPIO_PORTA_LOCK_R = 0x4C4F434B;     
    GPIO_PORTA_DEN_R = 0xC0;
    GPIO_PORTA_AFSEL_R = 0xC0;
    GPIO_PORTA_ODR_R = 0x80;
    GPIO_PORTA_PCTL_R = 0x33000000;
	
    SYSCTL_RCGCI2C_R = 0x03;
    NVIC_EN1_R = 0x00000020; // NVIC enable for I2C module 1
	
    // slave rx
    I2C1_MCR_R = 0x20;
    I2C1_SOAR_R = 0x3B;
    I2C1_SCSR_R = 0x01;
    I2C1_SIMR_R = 0x01; // interrupt enabled for data rx at slave

    NVIC_ST_RELOAD_R = 16000; // 1 millisecond
    NVIC_ST_CURRENT_R = 0x00;
    NVIC_ST_CTRL_R = 0x00000007;
	
    // master tx
    I2C0_MCR_R = 0x10;
    I2C0_MTPR_R = 0x09;
    I2C0_MSA_R = 0x00;// initializing

    // uint32_t TxData = 0x0008;
    uint8_t slave_address = 0x60;
	
    
	
    while(1){
        DAC_Tx(TxData, 2, slave_address);
    }
    return 0;
}


void DAC_Tx(uint32_t data, int no_of_bytes, uint8_t s_address){
    
    uint32_t Least_Sig_Byte = (data & 0xFF);
    data = data >> 8;
    data |= (Least_Sig_Byte << 8);

    int sent_bytes = 0;
    I2C0_MSA_R = (s_address << 1);
    I2C0_MDR_R = (data & 0xFF);
	
    if (no_of_bytes == 1){
        I2C0_MCS_R = 0x07;
        sent_bytes ++;}
    else{
        I2C0_MCS_R = 0x03;}
    while (sent_bytes < no_of_bytes){
        sent_bytes ++;
        while(I2C0_MCS_R & 0x01){
            ;// waiting for MCS bit to get cleared (BUSBSY)
        }
        if (I2C0_MCS_R & 0x02){ // MCS error bit
            GPIO_PORTF_DATA_R = 0x02;
            return;}

        I2C0_MDR_R = ( (data >> 8*sent_bytes) & 0xFF );
        if (sent_bytes < no_of_bytes){
            I2C0_MCS_R = 0x01;}
        else{
            I2C0_MCS_R = 0x05;
            while(I2C0_MCS_R & 0x01){
                ;// waiting for MCS bit to get cleared (BUSBSY)
}
            if (I2C0_MCS_R & 0x02){ // MCS error bit
                GPIO_PORTF_DATA_R = 0x02;
            }
        }
    }
}

void I2CSlaveHandler(){
    if(I2C1_SDR_R == 0x01){
        GPIO_PORTF_DATA_R = 0x0A;}
    else if(I2C1_SDR_R == 0x03){
        GPIO_PORTF_DATA_R = 0x08;}    }
    else if(I2C1_SDR_R == 0x02){
        GPIO_PORTF_DATA_R = 0x04;}    }
    I2C1_SICR_R = 0x01;
}
void ISR(){
    GPIO_PORTF_DATA_R ^= 0x04;
    TxData += 10;
    if(TxData >= 4096){
        TxData = 0;
    }
}

