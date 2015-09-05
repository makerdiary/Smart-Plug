


#include "znp_hal.h"
#include "bsp.h"
#include "rtk.h"


static short ssp_empty_rxfifo(void)
{
    short dummy = 0;

    // Delete all data that is in RxFifo of the SSP device
    while ((SSP->SR) & 0x04)
    {
        dummy = SSP->DR;
    }
    // return dummy otherwise the compiler optimizes the while loop away
    return dummy;
}

void halSpiInitZnp()
{
    // Init the SSP device which is the interface to the CC2530ZNP
    SSP_ENABLE_DEV();
    SSP_ENABLE_SCK();
   
#ifdef SSP_NORMAL_SPEED   //if set normal spi speed 2MHz
    SSP->CPSR = 0x02 ;
    SSP->CR0 &= 0x00FF;
    switch(SSP_GET_PCLK())
    {
    case 12000000:
        SSP->CR0 = SSP_FRAME_MODE | (0x02<<8) ;
        break;
    case 24000000:
        SSP->CR0 = SSP_FRAME_MODE | (0x05<<8) ;
        break;
    case 36000000:
        SSP->CR0 = SSP_FRAME_MODE | (0x08<<8) ;
        break;
    case 48000000:
        SSP->CR0 = SSP_FRAME_MODE | (0x0b<<8) ;
        break;
    default:
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
        char msgbuf[] = "The SPI PCLK is invalid !";
        DPRINT_NOARG(msgbuf);
#endif
        break;      
  }
#else //if set max spi speed 4MHz
    SSP->CPSR = 0x03;
    SSP->CR0 &= 0x00FF;
    switch(SSP_GET_PCLK())
    {
    case 12000000:
        SSP->CR0 = SSP_FRAME_MODE | (0x00<<8);
        break;
    case 24000000:
        SSP->CR0 = SSP_FRAME_MODE | (0x01<<8);
        break;
    case 36000000:
        SSP->CR0 = SSP_FRAME_MODE | (0x02<<8);
        break;
    case 48000000:
        SSP->CR0 = SSP_FRAME_MODE | (0x03<<8);
        break;
    default:
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
        char msgbuf[] = "The SPI PCLK is invalid !";
        DPRINT_NOARG(msgbuf);
#endif
        break;
  }
#endif
  
    SSP->CR1 &=~(1<<1); //disable spi first
    SSP->CR1 &=~(1<<2); //set spi master mode
    SSP->CR1 |=(1<<1);  //enable spi interface
    ssp_empty_rxfifo(); //clear all data in rx fifo
}

void znp_hal_init(void)
{
  
    gpio_alloc(ZNP_MRDY_PORTPIN , PCB_MODE_PULLNONE, GPIO_OUTPUT);
    gpio_alloc(ZNP_SRDY_PORTPIN , PCB_MODE_PULLUP  , GPIO_INPUT );
    gpio_alloc(ZNP_CFG0_PORTPIN , PCB_MODE_PULLNONE, GPIO_OUTPUT);
    gpio_alloc(ZNP_CFG1_PORTPIN , PCB_MODE_PULLNONE, GPIO_OUTPUT);
    gpio_alloc(ZNP_RESET_PORTPIN, PCB_MODE_PULLNONE, GPIO_OUTPUT);
  	
    // Setup SSP SCK func to drive SPI clock pin of CC2530ZNP
    pcb_alloc(SSP_SCLK_PORTPIN, PCB_FUNC_2, PCB_MODE_PULLUP );

    // Setup SSP MISO func to connect to MISO pin of CC2530ZNP
    pcb_alloc( SSP_MISO_PORTPIN, PCB_FUNC_1, PCB_MODE_PULLDOWN );

    // Setup SSP MOSI func to connect to MOSI pin of CC2530ZNP
    pcb_alloc( SSP_MOSI_PORTPIN, PCB_FUNC_1, PCB_MODE_PULLDOWN );

    // Setup the SSEL line
    pcb_alloc( SSP_SSEL_PORTPIN, PCB_FUNC_1, PCB_MODE_PULLUP );
  
    ZNP_CFG0_SET(); //use the 32kHz crystal connected to the CC2530-ZNP
    ZNP_CFG1_SET(); //use SPI MODE on CC2530ZNP,always used!!!
  
    SPI_SS_CLEAR();
}

void delayMs(unsigned int ms)
{
    //rtk_tsk_sleep(ms);
    while(ms--)
    {
        delayUs(1000);
    }
}

void delayUs(unsigned int us)
{
  while(us--)
  {
#if (LPC_CORE_CLOCKSPEED_HZ == 12000000)
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
#endif

#if ( LPC_CORE_CLOCKSPEED_HZ == 24000000)
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
  asm("nop"); asm("nop");
#endif

#if ( LPC_CORE_CLOCKSPEED_HZ == 36000000)
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop");
#endif
  
#if ( LPC_CORE_CLOCKSPEED_HZ == 48000000)
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop");
#endif   
  }
}

void spiWrite(unsigned char *bytes, unsigned char numBytes)
{
    rtk_int_disable();
    while(!(SSP->SR & 0x01));  //wait until TxFIFO is empty
    ssp_empty_rxfifo();       //clear RxFIFO
    while(numBytes--)
    {
        SSP->DR = *bytes;
        while(!(SSP->SR & 0x04)); //WAIT for a character to be received, if any
        *bytes++ = SSP->DR;       //read bytes
    }
    rtk_int_enable();
}