#include "global.h"

#include "main.h"

typedef struct RfuStruct
{
   s32 unk_0;
   u8 unk_4;
   u8 unk_5;
   u8 unk_6;
   u8 unk_7;
   u8 unk_8;
   u8 unk_9;
   u8 timerSelect;
   u8 unk_b;
   u32 unk_c;
   vu8 unk_10;
   u8 unk_11;
   vu16 unk_12;
   vu8 msMode;
   u8 unk_15;
   u8 unk_16;
   u8 unk_17;
   void * callbackM;
   void * callbackS;
   u32 callbackID;
   void * unk_24;
   void * unk_28;
   vu8 unk_2c;
   u8 padding[3];
} RfuStruct;

typedef struct RfuIntrStruct
{
   u8 unk28Data[0x74];
   u8 unk24Data[0x74];
   u8 block1[0x960];
   u8 block2[0x30];
} RfuIntrStruct;

typedef struct RfuState
{
   RfuStruct *rfuStruct;
} RfuState;

extern IntrFunc IntrSIO32();
extern struct RfuState gRfuState;
void STWI_init_Callback_M();
void STWI_init_Callback_S();
void STWI_set_Callback_M(void * callback);
void STWI_set_Callback_S(void * callback);
extern void STWI_intr_timer();

void STWI_init_all(RfuIntrStruct *interruptStruct, IntrFunc *interrupt, bool8 copyInterruptToRam)
{
   struct RfuStruct *rfuStructTemp;
   struct RfuStruct **rfuStructPtr;
   u16 ime_temp;
   int ret;
   
   // If we're copying our interrupt into RAM, DMA it to block1 and use
   // block2 for our RfuStruct, otherwise block1 holds the RfuStruct.
   // interrupt usually is a pointer to gIntrTable[1]
   if (copyInterruptToRam == TRUE)
   {
      *interrupt = (IntrFunc)(&interruptStruct->block1);
      DmaCopy16(3, &IntrSIO32, (void*)(&interruptStruct->block1), 0x960);
      
      gRfuState.rfuStruct = (struct RfuStruct*)(&interruptStruct->block2);
   }
   else
   {
      *interrupt = (IntrFunc)&IntrSIO32;
      gRfuState.rfuStruct = (struct RfuStruct*)(&interruptStruct->block1);
   }
   
   rfuStructPtr = (struct RfuStruct**)&gRfuState.rfuStruct;
   (*rfuStructPtr)->unk_28 = (void*)&interruptStruct->unk28Data;
   (*rfuStructPtr)->unk_24 = (void*)(&interruptStruct->unk24Data);
   (*rfuStructPtr)->msMode = 1;
   
   (*rfuStructPtr)->unk_0 = 0;
   (*rfuStructPtr)->unk_4 = 0;
   (*rfuStructPtr)->unk_5 = 0;
   (*rfuStructPtr)->unk_7 = 0;
   (*rfuStructPtr)->unk_8 = 0;
   (*rfuStructPtr)->unk_9 = 0;
   (*rfuStructPtr)->unk_c = 0;
   (*rfuStructPtr)->unk_10 = 0;
   
   // Don't @ me
   rfuStructTemp = *rfuStructPtr;
   rfuStructTemp->unk_12 = 0;
   rfuStructTemp->unk_15 = 0;

   (*rfuStructPtr)->unk_2c = 0;
   
   REG_RCNT = 0x100; //TODO: mystery bit?
   REG_SIOCNT = SIO_INTR_ENABLE | SIO_32BIT_MODE | SIO_115200_BPS;
   STWI_init_Callback_M();
   STWI_init_Callback_S();
   
   IntrEnable(INTR_FLAG_SERIAL);
}

void STWI_init_timer(IntrFunc *interrupt, int timerSelect)
{
   *interrupt = &STWI_intr_timer;
   gRfuState.rfuStruct->timerSelect = timerSelect;
   
   IntrEnable(INTR_FLAG_TIMER0 << gRfuState.rfuStruct->timerSelect); 
}

void AgbRFU_SoftReset()
{
   struct RfuStruct **rfuStructPtr;
   struct RfuStruct *rfuStructTemp;

   REG_RCNT = 0x8000;
   REG_RCNT = 0x80A0; // all these bits are undocumented
   
   {
      vu16 *timerL = &REG_TMCNT_L(gRfuState.rfuStruct->timerSelect);
      vu16 *timerH = &REG_TMCNT_H(gRfuState.rfuStruct->timerSelect);

      *timerH = 0;
      *timerL = 0;
      *timerH = 0x83;
      
      while (*timerL <= 0x11)
      {
         REG_RCNT = 0x80A2;
      }
      
      *timerH = 3;
   }
   REG_RCNT = 0x80A0;
   REG_SIOCNT = SIO_INTR_ENABLE | SIO_32BIT_MODE | SIO_115200_BPS;
   
   rfuStructPtr = (struct RfuStruct**)&gRfuState.rfuStruct;
   
   (*rfuStructPtr)->unk_0 = 0;
   (*rfuStructPtr)->unk_4 = 0;
   (*rfuStructPtr)->unk_5 = 0;
   (*rfuStructPtr)->unk_6 = 0;
   (*rfuStructPtr)->unk_7 = 0;
   (*rfuStructPtr)->unk_8 = 0;
   (*rfuStructPtr)->unk_9 = 0;
   (*rfuStructPtr)->unk_c = 0;
   (*rfuStructPtr)->unk_10 = 0;
   
   // Yeah this is the second time, there's probably something in the struct that I'm missing
   rfuStructTemp = *rfuStructPtr;
   rfuStructTemp->unk_12 = 0;
   rfuStructTemp->msMode = 1;
   (*rfuStructPtr)->unk_15 = 0;

   (*rfuStructPtr)->unk_2c = 0;
}

void STWI_set_MS_mode(u8 mode)
{
   gRfuState.rfuStruct->msMode = mode;
}

u32 STWI_read_status(u8 index)
{
   int result;
   switch(index)
   {
      case 0:
         return gRfuState.rfuStruct->unk_12;
      case 1:
         return gRfuState.rfuStruct->msMode;
      case 2:
         // something got inlined here?
         //TODO: figure this one out
         result = (gRfuState.rfuStruct->unk_0);
         __asm__("lsl r0, r0, #16");
         __asm__("lsr r0, r0, #16");
         break;
         
      case 3:
         return gRfuState.rfuStruct->unk_6;
         break;
      default:
         return 0xFFFF;
         break;
   }
   return result;
}

void STWI_init_Callback_M()
{
   STWI_set_Callback_M(0);
}

void STWI_init_Callback_S()
{
   STWI_set_Callback_S(0);
}

void STWI_set_Callback_M(void * callback)
{
   gRfuState.rfuStruct->callbackM = callback;
}

void STWI_set_Callback_S(void * callback)
{
   gRfuState.rfuStruct->callbackS = callback;
}

void STWI_set_Callback_ID(u32 id)
{
   gRfuState.rfuStruct->callbackID = id;
}

