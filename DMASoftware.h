// DMASoftware.c
// Runs on TM4C1294
#include <stdint.h>
#include "tm4c1294ncpdt.h"


// Tabla de control para el microcontrolado uDMA. Esta tabla debe contenee 128 espacios alineados en localidades de 32-bits.
// cada canal tiene un espacio para la direccion de origen, destino, el control de palabra y un espacio reservado.
uint32_t ucControlTable[256] __attribute__ ((aligned(1024)));
// Las localidades correspondientes al canal 30, usado para generar solicitudes por medio de software son
// 120, 121 y 122 para el origen, desnino y el DMACHCTL respectivamente
#define CH30 (30*4)
#define BIT30 0x40000000
// ************DMA_Init*****************
// Inicializa el controlador uDMA para realizar transferenicas de memoria Flash a memoria RAM
// Necesita ser llamada antes de realizar la solicitud por software
// Inputs:  ninguna
// Outputs: ninguna
void DMA_Init(void){  int i;
  volatile uint32_t delay; 
  for(i=0; i<256; i++){
    ucControlTable[i] = 0;
  }
  SYSCTL_RCGCDMA_R = 0x01;    //0) Reloj al modulo uDMA 1
  delay = SYSCTL_RCGCDMA_R;   //retardo para que termine de establecerse
  UDMA_CFG_R = 0x01;          //1) Habilitar operacion de DMA
  UDMA_CTLBASE_R = (uint32_t)ucControlTable;  //2) Ubicar la tabla de control para los canales
  //3) default Encoding is 0x00 (software request) for Chanel30->DMACHMAP3 Register
  UDMA_PRIOCLR_R = BIT30;     //4) No hay prioridades
  UDMA_ALTCLR_R = BIT30;      //5) Canal 30  usa la estructura de control primaria
  UDMA_USEBURSTCLR_R = BIT30; //6) Canal 30 responde a solicitudes individuales y en rafagas
  UDMA_REQMASKCLR_R = BIT30;  //7) Deshabilita al dma a responder solicitudes del canal  30
}


//*************Flash_Init*****************
//Inicializa el controlador periferico Flash para realizar transferencias 
//con direccion de origen en memoria Flash
void Flash_Init(void){

    while((FLASH_PP_R & 0x10000000)== 0);   //0) Revisa si el controlador periferico de flash permite transferencias de datos
    FLASH_DMASZ_R |= 0X01;                  //1) Tamano maximo de lectura de flash en (kB) 2KB = 2*(SZ +1) KB 
    FLASH_DMAST_R = 0X00000020;             //2) Direccion de inicio de donde tiene acceso el uDMA para leer
}
// ************DMA_Transfer*****************
// configura las direcciones de origen y destino de las transferencias solicitadas
// transferencias de 32 bits individuales
// La direccion del origen se incrementa en 32 bits, al igual que la direccion destino
// Entradas: source Apuntador de un buffer en RAM que contiene valores de 0 a 255 o direccion en Flash
//          destination Apuntador de un buffer en RAM donde se almacena la copia
//          count numero de palabras a transferir (max 1024 palabras)
// Esta rutina no espera la finalizacion de la transferencia
//Se deben haber habilitado todos los modulos antes (uDMA)

void DMA_Transfer(uint32_t *source, uint32_t *destination, uint32_t count){ 
  ucControlTable[CH30]   = (uint32_t)source+count*4-1;       // direccion origen
  ucControlTable[CH30+1] = (uint32_t)destination+count*4-1;  // direccion destino
  ucControlTable[CH30+2] = 0xAA00C002+((count-1)<<4);             // DMA Channel Control Word (DMACHCTL)
/* DMACHCTL          Bits    Value Description
   DSTINC            31:30   10    32-bit incremento en la direccion destino
   DSTSIZE           29:28   10    32-bit tamaño de dato en la localidad destino
   SRCINC            27:26   10    32-bit incremento en la direccion origen
   SRCSIZE           25:24   10    32-bit tamaño de dato en la localidad origen
   reserved          23:18   0     Reserved  
   ARBSIZE           17:14   0011  Arbitrajecada 8 transferencias
   XFERSIZE          13:4  count-1 conteo de transferencias realizadas
   NXTUSEBURST       3       0     N/A para este tipo de transferencia
   XFERMODE          2:0     010   modo Auto-solicitud
  */
  UDMA_ENASET_R = BIT30;  // canal 30 del uDMA habilitado
  UDMA_SWREQ_R = BIT30;   // instruccion de solicitud por software  
  // bit 30 en UDMA_ENASET_R se hace cero cuando la transferencia termina
  // bits 2:0 ucControlTable[CH30+2] se hacen cero cuando la transferencia termina
  //  existe la interrupcion 44 en el vector 60 del NVIC (0x0000.00F0) que permite 
  //una rutina de interrupcion cuando la transferencia por software termina
}

// ************DMA_Status*****************
//Indica el estado de la transferencia
uint32_t DMA_Status(void){ 
  return (UDMA_ENASET_R&BIT30);  // 1 si la transferencia del canal 30 sigue activa
                                 // 0 si ha terminado o no ha epezado
}

