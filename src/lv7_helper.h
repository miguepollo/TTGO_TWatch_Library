#include "lvgl.h"
#include "focaltech.h"

public:
bool lvgl_begin();
void startLvglTick();
void stopLvglTick();
void lvgl_whirling(uint8_t rot);
static void TOUCH_IRQ_HANDLE(void);
void disableTouchIRQ();
void enableTouchIRQ();
private:
