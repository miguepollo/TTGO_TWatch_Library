// File: lv7_helper.h

#include "lvgl.h"
#include "focaltech.h"
#include <Arduino.h>


    bool lvgl_begin();
    void startLvglTick();
    void stopLvglTick();
    void lvgl_whirling(uint8_t rot);
    static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
    void disableTouchIRQ();
    void enableTouchIRQ();
