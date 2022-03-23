#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "lv_font/lv_font.h"


/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include "lvgl_helpers.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "Gauge"
#define LV_TICK_PERIOD_MS 1
#define RAND_MAX 5
int chooseScreen;
SemaphoreHandle_t xGuiSemaphore;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask();
void launch();

/**********************
 *  Variable
 **********************/

int ax;
int ay;
int az;
int gx;
int gy;
int gz;

//BUFFER
