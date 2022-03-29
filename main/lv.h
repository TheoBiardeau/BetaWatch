#ifndef LV_H
#define LV_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "lv_font/lv_font.h"
#include "dataManagement.h"
#include "Gpio_bw.h"

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
#define LV_TICK_PERIOD_MS 1
#define RAND_MAX 5
SemaphoreHandle_t xGuiSemaphore;
#define LV_SPRINTF_CUSTOM 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask();
void launchLVGL();

/**********************
 *  Variable
 **********************/

int ax;
int ay;
int az;
int gx;
int gy;
int gz;

/**************************
 *  Private buffer struct *
 **************************/
static T_dataMouvement DM_Buff;
static T_dataPressur DP_Buff;
static T_dataTempHumi DTH_Buff;

static struct tm timeinfo;

#endif