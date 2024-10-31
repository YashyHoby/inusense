#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <MP.h>
#define MEMSIZE 8        // Meridim array maximum setting (8bit)
int16_t *meridian_addr;  // Shared memory address for meridian

#define subcore1 1

void setup()
{
  MP.begin(subcore1); // サブコアでタイマ割込まわしてるせいでSerial.print()使えへんデバッグ時は消してくださいごめんね
  delay(1000);

  Serial.begin(115200);
  delay(1000);

  /* Allocate Shared Memory */
  uint16_t *addr = (uint16_t *)MP.AllocSharedMemory(MEMSIZE);
  if(!addr)
  {
    Serial.println("Error: out of memory");
    return;
  }

  Serial.printf("SharedMemory Address: @%08lx\n", (uint32_t)addr);
  meridian_addr = &addr[0]; // Globally maintain the start address of shared memory

  /* Initialize shared memoty */
  for (int i=0; i<MEMSIZE; i++)
  {
    addr[i]=0x0000;
  }

  /* Send shared memory address to SubCore */
  int8_t msgid = 10;
  MP.Send(msgid, addr, subcore1);

  // /* shared memory check */
  // for (int i = 0; i < MEMSIZE; i++) {
  //   if (addr[i] != 0x0000) {
  //     Serial.printf("Error: @%08lx\n", (uint32_t)&addr[i]);
  //     while (1);
  //   }
  // }
  // Serial.println("SharedMemory Check: OK!");
}

void loop()
{
  setAnimation(1);  // Start animation No.1
  delay(2000);
  setAnimation(0);  // turn OFF neopixel
  delay(2000);
}

/* set Neopixel animation (0-11). animations detail is discribed in <Neopixel_subcore/animations.h> */
/* if you want to change animation scene, plz chenge matrix data defined in <Neopixel_subcore/animations.cpp>*/
void setAnimation(uint8_t animeNum)
{
  if (0 <= animeNum <= 11)
  {
    meridian_addr[0]=animeNum;
  }
}
