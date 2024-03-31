#include "lv7_helper.h"
#include "lvgl.h"
#include <Ticker.h>
#include <event_groups.h>
#if  defined(LILYGO_WATCH_LVGL)
    Ticker *tickTicker = nullptr;
#endif  /*LILYGO_WATCH_LVGL*/

#if (defined(LILYGO_WATCH_2020_V1) || defined(LILYGO_WATCH_2020_V2) ||  defined(LILYGO_WATCH_2020_S3) || defined(LILYGO_WATCH_2020_V3)|| defined(LILYGO_WATCH_2019_WITH_TOUCH)) &&  defined(LILYGO_WATCH_LVGL)
    /*
    Interrupt polling is only compatible with 2020-V1, 2020-V2, others are not currently adapted
    */
    static void TOUCH_IRQ_HANDLE(void)
    {
        portBASE_TYPE task_woken;
        if (_ttgo->_tpEvent) {
            xEventGroupSetBitsFromISR(_ttgo->_tpEvent, TOUCH_IRQ_BIT, &task_woken);
            if ( task_woken == pdTRUE ) {
                portYIELD_FROM_ISR();
            }
        }
    }
#endif  /*LILYGO_WATCH_2020_V1 & LILYGO_WATCH_2020_V2*/

#include "focaltech.h"
#include <Arduino.h>
#include "LilyGoLib.h"
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
#ifdef LILYGO_WATCH_LVGL
#endif
#ifdef LILYGO_WATCH_LVGL
#include <Ticker.h>
#endif
#if  defined(LILYGO_WATCH_LVGL) && defined(LILYGO_WATCH_HAS_DISPLAY)
    lv_disp_drv_t disp_drv;
#endif  /*LILYGO_WATCH_LVGL & LILYGO_WATCH_HAS_DISPLAY*/
#if  defined(LILYGO_WATCH_LVGL) && defined(LILYGO_WATCH_HAS_DISPLAY)

    static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
    {
        uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) ;
        // Use DMA for transfer
#if defined(ENABLE_LVGL_FLUSH_DMA)
        tft.startWrite();
        tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
        tft.pushPixelsDMA(( uint16_t *)color_p, size);
        tft.endWrite();
#else
        tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
        tft.pushColors(( uint16_t *)color_p, size, false);
#endif
        lv_disp_flush_ready(disp_drv);
    }

    void lvgl_whirling(uint8_t rot)
    {
        tft.setRotation(rot);
        disp_drv.hor_res = tft.width();
        disp_drv.ver_res = tft.height();
        lv_disp_drv_update(lv_disp_get_default(), &disp_drv);
    }




#ifdef  TWATCH_USE_PSRAM_ALLOC_LVGL
    lv_color_t *buf1 = nullptr;
#ifdef  TWATCH_LVGL_DOUBLE_BUFFER
    lv_color_t *buf2 = nullptr;
#endif
#endif

    bool lvgl_begin()
    {

        lv_init();
        lv_indev_drv_t indev_drv;
        lv_disp_drv_init(&disp_drv);
        static lv_disp_buf_t disp_buf;

#ifdef  TWATCH_USE_PSRAM_ALLOC_LVGL
        if (psramFound()) {
            buf1 = (lv_color_t *)ps_calloc(sizeof(lv_color_t), LVGL_BUFFER_SIZE);
        } else {
            buf1 = (lv_color_t *)calloc(sizeof(lv_color_t), LVGL_BUFFER_SIZE);
        }
        if (!buf1) {
            log_e("buf1 alloc failed\n");
            return false;
        }
#ifdef  TWATCH_LVGL_DOUBLE_BUFFER
        if (psramFound()) {
            buf2 = (lv_color_t *)ps_calloc(sizeof(lv_color_t), LVGL_BUFFER_SIZE);
        } else {
            buf2 = (lv_color_t *)calloc(sizeof(lv_color_t), LVGL_BUFFER_SIZE);
        }
        if (!buf2) {
            log_e("buf2 alloc failed\n");
            free(buf1);
            return false;
        }
#endif  /*TWATCH_LVGL_DOUBLE_BUFFER*/
#else
        static lv_color_t buf1[LVGL_BUFFER_SIZE];
#endif  /*TWATCH_LVGL_DOUBLE_BUFFER*/


#ifdef  TWATCH_LVGL_DOUBLE_BUFFER
        lv_disp_buf_init(&disp_buf, buf1, buf2, LVGL_BUFFER_SIZE);
#else
        lv_disp_buf_init(&disp_buf, buf1, NULL, LVGL_BUFFER_SIZE);
#endif


        disp_drv.hor_res = tft.width();
        disp_drv.ver_res = tft.height();
        disp_drv.flush_cb = disp_flush;
        /*Set a display buffer*/
        disp_drv.buffer = &disp_buf;
        lv_disp_drv_register(&disp_drv);

#if  defined(LILYGO_WATCH_HAS_TOUCH)
        /*Register a touchpad input device*/
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read_cb = touchpad_read;
        lv_indev_drv_register(&indev_drv);
#endif  /*LILYGO_WATCH_HAS_TOUCH*/


#if defined(LILYGO_WATCH_2020_S3)
#define LILYGO_WATCH_LVGL_FS_SPIFFS
#endif

#ifdef LILYGO_WATCH_LVGL_FS
#if  defined(LILYGO_WATCH_LVGL_FS_SPIFFS)
        SPIFFS.begin(true, "/fs");
#else
        //TODO:
#endif  /*LILYGO_WATCH_LVGL_FS_SPIFFS*/





#endif  /*LILYGO_WATCH_LVGL_FS*/
    tickTicker = new Ticker();
    void startLvglTick();
    {
        tickTicker->attach_ms(5, []() {
            lv_tick_inc(5);
        });
    }

        startLvglTick();
        return true;
    }



    void stopLvglTick()
    {
        tickTicker->detach();
    }

#endif  /*LILYGO_WATCH_LVGL*/
#if (defined(LILYGO_WATCH_2020_V1) || defined(LILYGO_WATCH_2020_V2) || defined(LILYGO_WATCH_2020_V3)|| defined(LILYGO_WATCH_2020_S3)||defined(LILYGO_WATCH_2019_WITH_TOUCH)) &&  defined(LILYGO_WATCH_LVGL)
    void disableTouchIRQ()
    {
        detachInterrupt(BOARD_TOUCH_INT);
    }

    void enableTouchIRQ()
    {
        /*
            Interrupt polling is only compatible with 2020-V1, 2020-V2 ,2019 , others are not currently adapted
        */
        pinMode(BOARD_TOUCH_INT, INPUT);
        attachInterrupt(BOARD_TOUCH_INT, TOUCH_IRQ_HANDLE, FALLING);
    }
#endif  /*LILYGO_WATCH_2020_V1 & LILYGO_WATCH_2020_V2*/

#if (defined(LILYGO_WATCH_2020_V1) || defined(LILYGO_WATCH_2020_V2) || defined(LILYGO_WATCH_2020_S3) || defined(LILYGO_WATCH_2020_V3)|| defined(LILYGO_WATCH_2019_WITH_TOUCH)) &&  defined(LILYGO_WATCH_LVGL)
        /*
            Interrupt polling is only compatible with 2020-V1, 2020-V2, others are not currently adapted
        */
       void setupTouch(){
        pinMode(BOARD_TOUCH_INT, INPUT);
        attachInterrupt(BOARD_TOUCH_INT, TOUCH_IRQ_HANDLE, FALLING);
       }
#endif  /*LILYGO_WATCH_2020_V1 & LILYGO_WATCH_2020_V2*/






#if defined(LILYGO_WATCH_LVGL) && defined(LILYGO_WATCH_HAS_DISPLAY)


#if defined(LILYGO_WATCH_LVGL) && defined(LILYGO_WATCH_HAS_TOUCH)
    static bool touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
    {

#if (defined(LILYGO_WATCH_2020_V1) || defined(LILYGO_WATCH_2020_V2) || defined(LILYGO_WATCH_2020_V3)|| defined(LILYGO_WATCH_2020_S3)|| defined(LILYGO_WATCH_2019_WITH_TOUCH)) &&  defined(LILYGO_WATCH_LVGL)
        /*
            Interrupt polling is only compatible with 2020-V1, 2020-V2,2019, others are not currently adapted
        */
        static int16_t x, y;
        if (xEventGroupGetBits(_ttgo->_tpEvent) & TOUCH_IRQ_BIT) {
            xEventGroupClearBits(_ttgo->_tpEvent, TOUCH_IRQ_BIT);
            data->state = watch.getTouch(x, y) ?  LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
            data->point.x = x;
            data->point.y = y;
        } else {
            data->state = LV_INDEV_STATE_REL;
            data->point.x = x;
            data->point.y = y;
        }
#else
        data->state = watch.getTouch(data->point.x, data->point.y) ?  LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
#endif /*LILYGO_WATCH_2020_V1 & LILYGO_WATCH_2020_V2*/

        return false; /*Return false because no more to be read*/
    }

#endif /*LILYGO_WATCH_LVGL , LILYGO_WATCH_HAS_TOUCH*/

#endif /*LILYGO_WATCH_LVGL , LILYGO_WATCH_HAS_DISPLAY*/

