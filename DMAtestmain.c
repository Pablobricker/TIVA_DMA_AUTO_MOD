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
// Enciende un LED de la tarjeta.
//
//*****************************************************************************

#define SIZE 128
//Localidades de destino y origen para transferencias por software
uint32_t SrcBuf = 0x30;   //Flash
//uint32_t SrcBuf[SIZE];  //RAM
uint32_t DestBuf[SIZE];
int main(void){  volatile uint32_t delay; uint32_t i,t;
  //Configura el puerto F como GPIO para visualizar salida LED
  SYSCTL_RCGCGPIO_R |= 0x20; // 0) Reloj
  delay = SYSCTL_RCGCGPIO_R;              // retardo de activacion
  GPIO_PORTF_DIR_R |= 0x01;    // PF0 como salida
  GPIO_PORTF_AFSEL_R &= ~0x01; // deshabilita la funcion alterna de PF0
  GPIO_PORTF_DEN_R |= 0x01;    // modo digital en PF0
                               // configure PF0,F4 as GPIO
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FFF0)+0x00000000;
  GPIO_PORTF_AMSEL_R = 0;      // deshabilita la funcion analógica en PF0
  GPIO_PORTF_DATA_R = 0;        // apaga LED
  DMA_Init();  // inicializa uDMA para una solicitud de transferencia por sofwr
  
  Flash_Init();// inicializa controlador Flash para transferencias de lectura por softwr //Comentar para transferencias sram- sram
  t = 0;

  for(i=0;i<SIZE;i++){
        //SrcBuf[i] = i; //Descomental para transferencias ram - ram
        DestBuf[i] = 0;
      }


  //
  while(DMA_Status()); // espera a que termine la transferencia
      DMA_Transfer(SrcBuf,DestBuf,SIZE); 

      //TRansferencias de flash comprobadas xd
      GPIO_PORTF_DATA_R = 0X01;
  while(1){
//    LEDS = COLORWHEEL[t&0x07];
//    t = t+1;
//    for(delay = 0; delay < 600000; delay++){
//    }
  }
}
