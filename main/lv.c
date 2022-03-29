/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include "lv.h"
#include <time.h>
int chooseScreen = 0;
int data_test = 5;
/**********************
 *   APPLICATION MAIN
 **********************/
void launchLVGL()
{
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 6, NULL, 0, NULL, 1);
}

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */

static void guiTask()
{

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820 || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306

    /* Actual size in pixels, not bytes. */
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
    xSemaphoreGive(I2CSema);

    /* Create the demo application */

    /**********************************
     *  Creation of screen and set up *
     **********************************/
    lv_obj_t *scr1 = lv_obj_create(NULL, NULL);
    lv_obj_t *scr2 = lv_obj_create(NULL, NULL);
    lv_obj_t *scr3 = lv_obj_create(NULL, NULL);
    lv_obj_t *scr4 = lv_obj_create(NULL, NULL);
    lv_obj_t *scr5 = lv_obj_create(NULL, NULL);
    lv_obj_t *scr6 = lv_obj_create(NULL, NULL);
    lv_obj_t *scr7 = lv_obj_create(NULL, NULL);
    lv_obj_t *scr8 = lv_obj_create(NULL, NULL);
    lv_obj_t *scr9 = lv_obj_create(NULL, NULL);

    static lv_style_t style_Screen;
    lv_style_init(&style_Screen);
    lv_style_set_bg_color(&style_Screen, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_add_style(scr1, LV_OBJ_PART_MAIN, &style_Screen);
    lv_obj_add_style(scr2, LV_OBJ_PART_MAIN, &style_Screen);
    lv_obj_add_style(scr3, LV_OBJ_PART_MAIN, &style_Screen);
    lv_obj_add_style(scr4, LV_OBJ_PART_MAIN, &style_Screen);
    lv_obj_add_style(scr5, LV_OBJ_PART_MAIN, &style_Screen);
    lv_obj_add_style(scr6, LV_OBJ_PART_MAIN, &style_Screen);
    lv_obj_add_style(scr7, LV_OBJ_PART_MAIN, &style_Screen);
    lv_obj_add_style(scr8, LV_OBJ_PART_MAIN, &style_Screen);
    lv_obj_add_style(scr9, LV_OBJ_PART_MAIN, &style_Screen);

    /**********************************
     *  set object for screen 1 *
     **********************************/
    lv_obj_t *b_wx = lv_bar_create(scr1, NULL);
    lv_obj_t *b_wy = lv_bar_create(scr1, NULL);
    lv_obj_t *b_wz = lv_bar_create(scr1, NULL);
    lv_obj_t *L_legend_Bwx = lv_label_create(scr1, NULL);
    lv_obj_t *L_legend_Bwy = lv_label_create(scr1, NULL);
    lv_obj_t *L_legend_Bwz = lv_label_create(scr1, NULL);
    lv_obj_t *L_legend_chart_BG = lv_label_create(scr1, NULL);
    lv_obj_t *L_legend_Bwx_u = lv_label_create(b_wx, NULL);
    lv_obj_t *L_legend_Bwy_u = lv_label_create(b_wy, NULL);
    lv_obj_t *L_legend_Bwz_u = lv_label_create(b_wz, NULL);
    lv_obj_t *Value_wx = lv_label_create(b_wx, NULL);
    lv_obj_t *Value_wy = lv_label_create(b_wy, NULL);
    lv_obj_t *Value_wz = lv_label_create(b_wz, NULL);

    lv_bar_set_range(b_wx, -720, 720);
    lv_bar_set_range(b_wy, -720, 720);
    lv_bar_set_range(b_wz, -720, 720);

    lv_obj_set_size(b_wx, 200, 20);
    lv_obj_set_size(b_wy, 200, 20);
    lv_obj_set_size(b_wz, 200, 20);

    lv_obj_align(b_wx, NULL, LV_ALIGN_CENTER, 0, -60);
    lv_obj_align(b_wy, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(b_wz, NULL, LV_ALIGN_CENTER, 0, 60);

    lv_obj_align(L_legend_Bwx, NULL, LV_ALIGN_CENTER, 10, -40);
    lv_obj_align(L_legend_Bwy, NULL, LV_ALIGN_CENTER, 10, 20);
    lv_obj_align(L_legend_Bwz, NULL, LV_ALIGN_CENTER, 10, 80);
    lv_obj_align(L_legend_chart_BG, NULL, LV_ALIGN_IN_TOP_MID, -80, 0);

    lv_obj_align(L_legend_Bwx_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);
    lv_obj_align(L_legend_Bwy_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);
    lv_obj_align(L_legend_Bwz_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_align(Value_wx, NULL, LV_ALIGN_CENTER, 3, 0);
    lv_obj_align(Value_wy, NULL, LV_ALIGN_CENTER, 3, 0);
    lv_obj_align(Value_wz, NULL, LV_ALIGN_CENTER, 3, 0);

    lv_label_set_text(L_legend_Bwx, "wx");
    lv_label_set_text(L_legend_Bwy, "wy");
    lv_label_set_text(L_legend_Bwz, "wz");
    lv_label_set_text(L_legend_chart_BG, "Gyroscope en temps reel");

    lv_label_set_text(L_legend_Bwx_u, "-720                                720");
    lv_label_set_text(L_legend_Bwy_u, "-720                                720");
    lv_label_set_text(L_legend_Bwz_u, "-720                                720");

    /**********************************
     *  set object for screen 2 *
     **********************************/
    lv_obj_t *chart_gyro;
    lv_obj_t *wx = lv_obj_create(scr2, NULL);
    lv_obj_t *wy = lv_obj_create(scr2, NULL);
    lv_obj_t *wz = lv_obj_create(scr2, NULL);
    lv_obj_t *L_legend_Gx = lv_label_create(scr2, NULL);
    lv_obj_t *L_legend_Gy = lv_label_create(scr2, NULL);
    lv_obj_t *L_legend_chart_G = lv_label_create(scr2, NULL);
    lv_obj_t *Title_screen = lv_label_create(scr2, NULL);
    static lv_style_t style_Red;
    static lv_style_t style_Green;
    static lv_style_t style_Blue;

    lv_style_init(&style_Red);
    lv_style_set_bg_color(&style_Red, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_add_style(wx, LV_OBJ_PART_MAIN, &style_Red);

    lv_style_init(&style_Green);
    lv_style_set_bg_color(&style_Green, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_obj_add_style(wy, LV_OBJ_PART_MAIN, &style_Green);

    lv_style_init(&style_Blue);
    lv_style_set_bg_color(&style_Blue, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(wz, LV_OBJ_PART_MAIN, &style_Blue);

    chart_gyro = lv_chart_create(scr2, NULL);
    lv_chart_set_range(chart_gyro, 0, 1440);
    lv_obj_set_size(chart_gyro, 290, 180);
    lv_chart_set_point_count(chart_gyro, 30);
    lv_obj_align(chart_gyro, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -20);
    lv_chart_set_type(chart_gyro, LV_CHART_TYPE_LINE); /*Show lines and points too*/

    lv_chart_series_t *ser1_gyro = lv_chart_add_series(chart_gyro, LV_COLOR_RED);
    lv_chart_series_t *ser2_gyro = lv_chart_add_series(chart_gyro, LV_COLOR_GREEN);
    lv_chart_series_t *ser3_gyro = lv_chart_add_series(chart_gyro, LV_COLOR_BLACK);

    lv_obj_set_size(wx, 20, 20);
    lv_obj_set_size(wy, 20, 20);
    lv_obj_set_size(wz, 20, 20);

    lv_obj_align(wx, NULL, LV_ALIGN_IN_TOP_LEFT, 25, 0);
    lv_obj_align(wy, NULL, LV_ALIGN_IN_TOP_LEFT, 90, 0);
    lv_obj_align(wz, NULL, LV_ALIGN_IN_TOP_LEFT, 150, 0);

    lv_obj_align(L_legend_chart_G, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_align(L_legend_Gx, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_obj_align(L_legend_Gy, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 12);
    lv_obj_align(Title_screen, NULL, LV_ALIGN_IN_TOP_RIGHT, -100, 0);

    lv_label_set_text_fmt(L_legend_chart_G, "Gx           Gy          Gz");
    lv_label_set_text_fmt(L_legend_Gx, "T+3s              T+2s              T+1s                T");
    lv_label_set_text_fmt(L_legend_Gy, "\n\n720\n\n360 m/s\n\n 0 m/s\n\n-360 m/s\n\n-720 m/s");
    lv_label_set_text_fmt(Title_screen, "Gyroscope");

    /**********************************
     *  set object for screen 3 *
     **********************************/

    lv_obj_t *b_ax = lv_bar_create(scr3, NULL);
    lv_obj_t *b_ay = lv_bar_create(scr3, NULL);
    lv_obj_t *b_az = lv_bar_create(scr3, NULL);
    lv_obj_t *L_legend_Bax = lv_label_create(scr3, NULL);
    lv_obj_t *L_legend_Bay = lv_label_create(scr3, NULL);
    lv_obj_t *L_legend_Baz = lv_label_create(scr3, NULL);
    lv_obj_t *L_legend_chart_BA = lv_label_create(scr3, NULL);
    lv_obj_t *L_legend_Bax_u = lv_label_create(b_ax, NULL);
    lv_obj_t *L_legend_Bay_u = lv_label_create(b_ay, NULL);
    lv_obj_t *L_legend_Baz_u = lv_label_create(b_az, NULL);
    lv_obj_t *Value_ax = lv_label_create(b_ax, NULL);
    lv_obj_t *Value_ay = lv_label_create(b_ay, NULL);
    lv_obj_t *Value_az = lv_label_create(b_az, NULL);

    lv_bar_set_range(b_ax, -200, 200);
    lv_bar_set_range(b_ay, -200, 200);
    lv_bar_set_range(b_az, -200, 200);

    lv_obj_set_size(b_ax, 200, 20);
    lv_obj_set_size(b_ay, 200, 20);
    lv_obj_set_size(b_az, 200, 20);

    lv_obj_align(b_ax, NULL, LV_ALIGN_CENTER, 0, -60);
    lv_obj_align(b_ay, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(b_az, NULL, LV_ALIGN_CENTER, 0, 60);

    lv_obj_align(L_legend_Bax, NULL, LV_ALIGN_CENTER, 10, -40);
    lv_obj_align(L_legend_Bay, NULL, LV_ALIGN_CENTER, 10, 20);
    lv_obj_align(L_legend_Baz, NULL, LV_ALIGN_CENTER, 10, 80);
    lv_obj_align(L_legend_chart_BA, NULL, LV_ALIGN_IN_TOP_MID, -83, 0);

    lv_obj_align(L_legend_Bax_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);
    lv_obj_align(L_legend_Bay_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);
    lv_obj_align(L_legend_Baz_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_align(Value_ax, NULL, LV_ALIGN_CENTER, 3, 0);
    lv_obj_align(Value_ay, NULL, LV_ALIGN_CENTER, 3, 0);
    lv_obj_align(Value_az, NULL, LV_ALIGN_CENTER, 3, 0);

    lv_label_set_text(L_legend_Bax, "ax");
    lv_label_set_text(L_legend_Bay, "ay");
    lv_label_set_text(L_legend_Baz, "az");
    lv_label_set_text(L_legend_chart_BA, "Accelerometre en temps reel");

    lv_label_set_text(L_legend_Bax_u, "-2                                       2");
    lv_label_set_text(L_legend_Bay_u, "-2                                       2");
    lv_label_set_text(L_legend_Baz_u, "-2                                       2");

    /**********************************
     *  set object for screen 4 *
     **********************************/

    lv_obj_t *chart_acc;
    lv_obj_t *L_legend_chart_A = lv_label_create(scr4, NULL);
    lv_obj_t *L_legend_Ax = lv_label_create(scr4, NULL);
    lv_obj_t *L_legend_Ay = lv_label_create(scr4, NULL);
    lv_obj_t *Ax_c = lv_obj_create(scr4, NULL);
    lv_obj_t *Ay_c = lv_obj_create(scr4, NULL);
    lv_obj_t *Az_c = lv_obj_create(scr4, NULL);
    lv_obj_t *Title_screen_acc = lv_label_create(scr4, NULL);

    lv_style_init(&style_Red);
    lv_style_set_bg_color(&style_Red, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_add_style(Ax_c, LV_OBJ_PART_MAIN, &style_Red);

    lv_style_init(&style_Green);
    lv_style_set_bg_color(&style_Green, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_obj_add_style(Ay_c, LV_OBJ_PART_MAIN, &style_Green);

    lv_style_init(&style_Blue);
    lv_style_set_bg_color(&style_Blue, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(Az_c, LV_OBJ_PART_MAIN, &style_Blue);

    chart_acc = lv_chart_create(scr4, NULL);
    lv_chart_set_range(chart_acc, 0, 500);
    lv_obj_set_size(chart_acc, 290, 180);
    lv_chart_set_point_count(chart_acc, 30);
    lv_obj_align(chart_acc, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -20);
    lv_chart_set_type(chart_acc, LV_CHART_TYPE_LINE); /*Show lines and points too*/

    lv_chart_series_t *ser1_acc = lv_chart_add_series(chart_acc, LV_COLOR_RED);
    lv_chart_series_t *ser2_acc = lv_chart_add_series(chart_acc, LV_COLOR_GREEN);
    lv_chart_series_t *ser3_acc = lv_chart_add_series(chart_acc, LV_COLOR_BLACK);

    lv_obj_set_size(Ax_c, 20, 20);
    lv_obj_set_size(Ay_c, 20, 20);
    lv_obj_set_size(Az_c, 20, 20);

    lv_obj_align(Ax_c, NULL, LV_ALIGN_IN_TOP_LEFT, 25, 0);
    lv_obj_align(Ay_c, NULL, LV_ALIGN_IN_TOP_LEFT, 90, 0);
    lv_obj_align(Az_c, NULL, LV_ALIGN_IN_TOP_LEFT, 150, 0);

    lv_obj_align(L_legend_chart_A, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_align(L_legend_Ax, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_obj_align(L_legend_Ay, NULL, LV_ALIGN_IN_TOP_RIGHT, 5, 12);
    lv_obj_align(Title_screen_acc, NULL, LV_ALIGN_IN_TOP_RIGHT, -100, 0);

    lv_label_set_text_fmt(L_legend_chart_A, "Ax           Ay          Az");
    lv_label_set_text_fmt(L_legend_Ax, "T+3s              T+2s              T+1s                T");
    lv_label_set_text_fmt(L_legend_Ay, "\n\n 2g\n\n 1g\n\n 0g\n\n-1g\n\n-2g");
    lv_label_set_text_fmt(Title_screen_acc, "Accelerometre");

    /**********************************
     *  set object for screen 5 *
     **********************************/

    lv_obj_t *b_mx = lv_bar_create(scr5, NULL);
    lv_obj_t *b_my = lv_bar_create(scr5, NULL);
    lv_obj_t *b_mz = lv_bar_create(scr5, NULL);
    lv_obj_t *L_legend_Bmx = lv_label_create(scr5, NULL);
    lv_obj_t *L_legend_Bmy = lv_label_create(scr5, NULL);
    lv_obj_t *L_legend_Bmz = lv_label_create(scr5, NULL);
    lv_obj_t *L_legend_chart_Bm = lv_label_create(scr5, NULL);
    lv_obj_t *L_legend_Bmx_u = lv_label_create(b_mx, NULL);
    lv_obj_t *L_legend_Bmy_u = lv_label_create(b_my, NULL);
    lv_obj_t *L_legend_Bmz_u = lv_label_create(b_mz, NULL);
    lv_obj_t *Value_mx = lv_label_create(b_mx, NULL);
    lv_obj_t *Value_my = lv_label_create(b_my, NULL);
    lv_obj_t *Value_mz = lv_label_create(b_mz, NULL);

    lv_bar_set_range(b_mx, -680, 680);
    lv_bar_set_range(b_my, -680, 680);
    lv_bar_set_range(b_mz, -680, 680);

    lv_obj_set_size(b_mx, 200, 20);
    lv_obj_set_size(b_my, 200, 20);
    lv_obj_set_size(b_mz, 200, 20);

    lv_obj_align(b_mx, NULL, LV_ALIGN_CENTER, 0, -60);
    lv_obj_align(b_my, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(b_mz, NULL, LV_ALIGN_CENTER, 0, 60);

    lv_obj_align(L_legend_Bmx, NULL, LV_ALIGN_CENTER, 10, -40);
    lv_obj_align(L_legend_Bmy, NULL, LV_ALIGN_CENTER, 10, 20);
    lv_obj_align(L_legend_Bmz, NULL, LV_ALIGN_CENTER, 10, 80);
    lv_obj_align(L_legend_chart_Bm, NULL, LV_ALIGN_IN_TOP_MID, -83, 0);

    lv_obj_align(L_legend_Bmx_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);
    lv_obj_align(L_legend_Bmy_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);
    lv_obj_align(L_legend_Bmz_u, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_align(Value_mx, NULL, LV_ALIGN_CENTER, 3, 0);
    lv_obj_align(Value_my, NULL, LV_ALIGN_CENTER, 3, 0);
    lv_obj_align(Value_mz, NULL, LV_ALIGN_CENTER, 3, 0);

    lv_label_set_text(L_legend_Bmx, "mx");
    lv_label_set_text(L_legend_Bmy, "my");
    lv_label_set_text(L_legend_Bmz, "mz");
    lv_label_set_text(L_legend_chart_Bm, "Magnetometre en temps reel");

    lv_label_set_text(L_legend_Bmx_u, "-685                                685");
    lv_label_set_text(L_legend_Bmy_u, "-685                                685");
    lv_label_set_text(L_legend_Bmz_u, "-685                                685");
    /**********************************
     *  set object for screen 6 *
     **********************************/

    lv_obj_t *temp = lv_gauge_create(scr6, NULL);
    lv_obj_t *humidity = lv_gauge_create(scr6, NULL);
    lv_obj_t *legend_temp_humi = lv_label_create(scr6, NULL);
    lv_obj_t *L_templ = lv_label_create(scr6, NULL);
    lv_obj_t *L_humil = lv_label_create(scr6, NULL);

    lv_obj_set_size(temp, 150, 150);
    lv_obj_align(temp, NULL, LV_ALIGN_CENTER, -80, 0);
    lv_gauge_set_range(temp, -50, 50);
    lv_gauge_set_critical_value(temp, 40);
    lv_gauge_set_scale(temp, 290, 50, 5);

    lv_obj_set_size(humidity, 150, 150);
    lv_obj_align(humidity, NULL, LV_ALIGN_CENTER, 80, 0);
    lv_gauge_set_range(humidity, 0, 100);
    lv_gauge_set_critical_value(humidity, 90);
    lv_gauge_set_scale(humidity, 290, 50, 5);

    lv_obj_align(legend_temp_humi, NULL, LV_ALIGN_IN_TOP_LEFT, 35, 0);
    lv_label_set_text_fmt(legend_temp_humi, "Temperature et humidite");

    lv_obj_align(L_templ, NULL, LV_ALIGN_CENTER, -140, 90);
    lv_label_set_text_fmt(L_templ, "Temperature (°c)");

    lv_obj_align(L_humil, NULL, LV_ALIGN_CENTER, 45, 90);
    lv_label_set_text_fmt(L_humil, "Humidite (/%%)");

    /**********************************
     *  set object for screen 7 *
     **********************************/

    lv_obj_t *pressure_gauge = lv_gauge_create(scr7, NULL);
    lv_obj_t *L_legend_pressure = lv_label_create(scr7, NULL);
    lv_obj_set_size(pressure_gauge, 200, 200);
    lv_obj_align(pressure_gauge, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_gauge_set_range(pressure_gauge, 990, 1020);
    lv_gauge_set_critical_value(pressure_gauge, 1021);
    lv_gauge_set_scale(pressure_gauge, 270, 25, 5);

    lv_obj_align(L_legend_pressure, NULL, LV_ALIGN_IN_TOP_MID, -22, 0);
    lv_label_set_text_fmt(L_legend_pressure, "Pression");

    /**********************************
     *  set object for screen 8 *
     *********************************/
    static lv_style_t style_Police;
    lv_style_init(&style_Police);

    lv_obj_t *Time = lv_label_create(scr8, NULL);
    lv_obj_set_size(Time, 50, 30);
    lv_obj_set_pos(Time, 30, 250);
    lv_obj_align(Time, NULL, LV_ALIGN_IN_LEFT_MID, 30, 0);

    /**********************************
     *  set object for screen 8 *
     *********************************/

    lv_obj_t *step_bar = lv_bar_create(scr9, NULL);
    lv_obj_t *Legend_screen = lv_label_create(scr9, NULL);
    lv_obj_t *Value_steps = lv_label_create(step_bar, NULL);

    lv_bar_set_range(step_bar, 0, 1000);

    lv_obj_set_size(step_bar, 200, 20);

    lv_obj_align(step_bar, NULL, LV_ALIGN_CENTER, 0, -60);

    lv_obj_align(Legend_screen, NULL, LV_ALIGN_IN_TOP_MID, -63, 0);

    lv_obj_align(Value_steps, NULL, LV_ALIGN_CENTER, 3, 0);

    lv_label_set_text(Legend_screen, "Compteur de pas");

    while (1)
    {
        lv_task_handler();

        if (chooseScreen == 0)
        {
            lv_scr_load(scr1);
            xQueueReceive(dataMouvement_Queue_Screen, &DM_Buff, portMAX_DELAY);
            lv_bar_set_value(b_wx, DM_Buff.Dgyro_x, NULL);
            lv_bar_set_value(b_wy, DM_Buff.Dgyro_y, NULL);
            lv_bar_set_value(b_wz, DM_Buff.Dgyro_z, NULL);
            lv_label_set_text_fmt(Value_wx, "%.2f", DM_Buff.Dgyro_x);
            lv_label_set_text_fmt(Value_wy, "%.2f", DM_Buff.Dgyro_x);
            lv_label_set_text_fmt(Value_wz, "%.2f", DM_Buff.Dgyro_x);
        }
        else if (chooseScreen == 1)
        {

            lv_scr_load(scr2);
            xQueueReceive(dataMouvement_Queue_Screen, &DM_Buff, portMAX_DELAY);
            lv_chart_refresh(chart_gyro);
            lv_chart_set_next(chart_gyro, ser1_gyro, DM_Buff.Dgyro_x + 720);
            lv_chart_set_next(chart_gyro, ser2_gyro, DM_Buff.Dgyro_y + 720);
            lv_chart_set_next(chart_gyro, ser3_gyro, DM_Buff.Dgyro_z + 720);
        }
        else if (chooseScreen == 2)
        {
            lv_scr_load(scr3);
            xQueueReceive(dataMouvement_Queue_Screen, &DM_Buff, portMAX_DELAY);
            lv_bar_set_value(b_ax, DM_Buff.Dacc_x * 100, NULL);
            lv_bar_set_value(b_ay, DM_Buff.Dacc_y * 100, NULL);
            lv_bar_set_value(b_az, DM_Buff.Dacc_z * 100, NULL);
            lv_label_set_text_fmt(Value_ax, "%.2f", DM_Buff.Dacc_x);
            lv_label_set_text_fmt(Value_ay, "%.2f", DM_Buff.Dacc_y);
            lv_label_set_text_fmt(Value_az, "%.2f", DM_Buff.Dacc_z);
        }

        else if (chooseScreen == 3)
        {
            lv_scr_load(scr4);
            xQueueReceive(dataMouvement_Queue_Screen, &DM_Buff, portMAX_DELAY);
            lv_chart_refresh(chart_acc);
            lv_chart_set_next(chart_acc, ser1_acc, DM_Buff.Dacc_x * 100 + 250);
            lv_chart_set_next(chart_acc, ser2_acc, DM_Buff.Dacc_y * 100 + 250);
            lv_chart_set_next(chart_acc, ser3_acc, DM_Buff.Dacc_z * 100 + 250);
        }
        else if (chooseScreen == 4)
        {
            lv_scr_load(scr5);
            xQueueReceive(dataMagn_Queue_Screen, &DMA_Buff, portMAX_DELAY);
            lv_bar_set_value(b_mx, DMA_Buff.Dmagn_x, NULL);
            lv_bar_set_value(b_my, DMA_Buff.Dmagn_y, NULL);
            lv_bar_set_value(b_mz, DMA_Buff.Dmagn_z, NULL);
            lv_label_set_text_fmt(Value_mx, "%.2f", DMA_Buff.Dmagn_x);
            lv_label_set_text_fmt(Value_my, "%.2f", DMA_Buff.Dmagn_y);
            lv_label_set_text_fmt(Value_mz, "%.2f", DMA_Buff.Dmagn_z);
        }
        else if (chooseScreen == 5)
        {
            lv_scr_load(scr6);
            xQueueReceive(dataTempHumi_Queue_Screen, &DTH_Buff, portMAX_DELAY);
            lv_gauge_set_value(humidity, 0, DTH_Buff.Dtemp);
            lv_gauge_set_value(humidity, 0, DTH_Buff.Dhumi);
        }
        else if (chooseScreen == 6)
        {
            lv_scr_load(scr7);
            xQueueReceive(dataPressur_Queue_Screen, &DP_Buff, portMAX_DELAY);
            lv_gauge_set_value(pressure_gauge, 0, (int)DP_Buff.Dpressure);
            lv_task_handler();
        }
        else if (chooseScreen == 7)
        {
            lv_scr_load(scr8);
            char strftime_buf[64];
            xQueueReceive(dataTime_Queue_Screen, &timeinfo, portMAX_DELAY);
            time_t now;
            time(&now);
            // Set timezone to China Standard Time
            setenv("TZ", "CST-2", 1);
            tzset();
            localtime_r(&now, &timeinfo);
            strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
            printf("%s\n", strftime_buf);
            lv_label_set_text_fmt(Time, "%s", strftime_buf);
        }
        else if (chooseScreen == 8)
        {
            lv_scr_load(scr9);
            xQueueReceive(dataMouvement_Queue_Screen, &DM_Buff, portMAX_DELAY);
            lv_bar_set_value(step_bar, DM_Buff.steps, NULL);
            lv_label_set_text_fmt(Value_steps, "%d", DM_Buff.steps);
        }
    }
}

static void lv_tick_task(void *arg)
{
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}