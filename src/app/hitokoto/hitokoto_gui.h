#ifndef _HITOKOTO_GUI_H
#define _HITOKOTO_GUI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define HITOKOTO_IMG_WIDTH 240
#define HITOKOTO_IMG_HEIGHT 240

/*
 * 外部变量声明
 */
extern const lv_img_dsc_t loading_anim;

typedef struct
{
    lv_obj_t *hitokoto_image;      // 一言图片框
    lv_obj_t *hitokoto_status_bar;  // 状态栏
    lv_obj_t *hitokoto_status_text; // 状态栏显示文本
    lv_obj_t *hitokoto_loading;     // 加载动画
} hitokoto_scr_obj_t;

extern hitokoto_scr_obj_t g_hitokoto_scr_obj;

/**
 * @brief 创建一个空白的屏幕
 */
void hitokoto_gui_init(void);

/**
 * @brief 设置图片显示
 * @param imgbuf 图片缓冲区
 * @param img_len 图片长度
 */
void hitokoto_gui_set_image(const uint8_t *imgbuf, uint32_t img_len);

/**
 * @brief 显示加载动画
 * @param show true显示，false隐藏
 */
void hitokoto_gui_show_loading(bool show);

/**
 * @brief 设置状态栏文本
 * @param text 状态文本
 */
void hitokoto_gui_set_status_text(const char *text);

/**
 * @brief 删除页面
 */
void hitokoto_gui_delete(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 