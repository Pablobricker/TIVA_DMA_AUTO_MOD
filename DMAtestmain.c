// DMAtestmain.c
// Runs on LM4F120/TM4C123
// Used to test the software triggerred DMA block transfer.
// Jonathan Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2014
   Section 6.4.5, Program 6.1

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "DMASoftware.h"

#include "tm4c1294ncpdt.h"

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
const int32_t COLORWHEEL[8] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};
#define SIZE 128
//#define SIZE 10
uint32_t SrcBuf[SIZE],DestBuf[SIZE];
int main(void){  volatile uint32_t delay; uint32_t i,t;
  //PLL_Init();  // now running at 80 MHz
  SYSCTL_RCGCGPIO_R |= 0x20; // enable Port F clock
  delay = SYSCTL_RCGCGPIO_R;              // allow time to finish 
  GPIO_PORTF_DIR_R |= 0x01;    // make PF0 output (PF0,F4 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0x01; // disable alt funct on PF0
  GPIO_PORTF_DEN_R |= 0x01;    // enable digital I/O on PF0
                               // configure PF0,F4 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FFF0)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;      // disable analog functionality on PF
  //LEDS = 0;
  GPIO_PORTF_DATA_R = 0;        // turn all LED off
  DMA_Init();  // initialize DMA channel 30 for software transfer
  Flash_Init();
  t = 0;

  for(i=0;i<SIZE;i++){
        SrcBuf[i] = i;
        DestBuf[i] = 0;
      }
//  while(DMA_Status()); // wait for idle
//      DMA_Transfer(SrcBuf,DestBuf,SIZE);

  while(DMA_Status()); // wait for idle
      DMA_Transfer(0x30,DestBuf,SIZE);

      //TRansferencias de flash comprobadas xd
      GPIO_PORTF_DATA_R = 0X01;
  while(1){
//    LEDS = COLORWHEEL[t&0x07];
//    t = t+1;
//    for(delay = 0; delay < 600000; delay++){
//    }
  }
}
