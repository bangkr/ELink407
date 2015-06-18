#ifndef __WWDG_H
#define __WWDG_H 

void init_wwdg(u8 tr,u8 wr,u32 fprer);
void wwdg_IRQ(FunctionalState NewState);

#endif
