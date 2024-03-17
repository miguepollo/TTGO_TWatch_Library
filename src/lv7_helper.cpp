#if  defined(LILYGO_WATCH_LVGL) && defined(LILYGO_WATCH_HAS_DISPLAY)
    void lvgl_whirling(uint8_t rot)
    {
        tft->setRotation(rot);
        disp_drv.hor_res = tft->width();
        disp_drv.ver_res = tft->height();
        lv_disp_drv_update(lv_disp_get_default(), &disp_drv);
    }


private:
#ifdef  TWATCH_USE_PSRAM_ALLOC_LVGL
    lv_color_t *buf1 = nullptr;
#ifdef  TWATCH_LVGL_DOUBLE_BUFFER
    lv_color_t *buf2 = nullptr;
#endif
#endif

public:
    bool lvgl_begin()
    {
        if (tft == nullptr) {
            return false;
        }
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


        disp_drv.hor_res = tft->width();
        disp_drv.ver_res = tft->height();
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


#if defined(LILYGO_WATCH_2020_V1) || defined(LILYGO_WATCH_2020_V2) || defined(LILYGO_WATCH_2020_V3)
#define LILYGO_WATCH_LVGL_FS_SPIFFS
#endif

#ifdef LILYGO_WATCH_LVGL_FS
#if  defined(LILYGO_WATCH_LVGL_FS_SPIFFS)
        SPIFFS.begin(true, "/fs");
#else
        //TODO:
#endif  /*LILYGO_WATCH_LVGL_FS_SPIFFS*/

        lv_fs_if_init();

#ifdef LILYGO_WATCH_LVGL_DECODER
        lv_png_init();
#endif

#endif  /*LILYGO_WATCH_LVGL_FS*/

        tickTicker = new Ticker();
        startLvglTick();
        return true;
    }

    void startLvglTick()
    {
        tickTicker->attach_ms(5, []() {
            lv_tick_inc(5);
        });
    }

    void stopLvglTick()
    {
        tickTicker->detach();
    }

#endif  /*LILYGO_WATCH_LVGL*/
