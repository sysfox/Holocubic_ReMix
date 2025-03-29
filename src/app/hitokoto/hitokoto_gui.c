#include "hitokoto_gui.h"
#include "driver/lv_port_indev.h"
#include "driver/lv_port_disp.h"
#include "lvgl.h"

hitokoto_scr_obj_t g_hitokoto_scr_obj;

void hitokoto_gui_init(void)
{
    // 创建屏幕
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);

    // 创建图片显示框
    g_hitokoto_scr_obj.hitokoto_image = lv_img_create(scr, NULL);
    lv_obj_align(g_hitokoto_scr_obj.hitokoto_image, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_auto_size(g_hitokoto_scr_obj.hitokoto_image, true);

    // 创建状态栏
    g_hitokoto_scr_obj.hitokoto_status_bar = lv_obj_create(scr, NULL);
    lv_obj_set_size(g_hitokoto_scr_obj.hitokoto_status_bar, 240, 30);
    lv_obj_align(g_hitokoto_scr_obj.hitokoto_status_bar, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_local_bg_color(g_hitokoto_scr_obj.hitokoto_status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_opa(g_hitokoto_scr_obj.hitokoto_status_bar, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_50);

    // 创建状态文本
    g_hitokoto_scr_obj.hitokoto_status_text = lv_label_create(g_hitokoto_scr_obj.hitokoto_status_bar, NULL);
    lv_label_set_text(g_hitokoto_scr_obj.hitokoto_status_text, "加载中...");
    lv_obj_align(g_hitokoto_scr_obj.hitokoto_status_text, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_text_color(g_hitokoto_scr_obj.hitokoto_status_text, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    // 创建加载动画
    g_hitokoto_scr_obj.hitokoto_loading = lv_spinner_create(scr, NULL);
    lv_obj_set_size(g_hitokoto_scr_obj.hitokoto_loading, 100, 100);
    lv_obj_align(g_hitokoto_scr_obj.hitokoto_loading, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_hidden(g_hitokoto_scr_obj.hitokoto_loading, true);
}

void hitokoto_gui_set_image(const uint8_t *imgbuf, uint32_t img_len)
{
    if (imgbuf == NULL || img_len == 0) {
        return;
    }

    lv_img_dsc_t img_dsc;
    img_dsc.data = imgbuf;
    img_dsc.header.always_zero = 0;
    img_dsc.header.w = HITOKOTO_IMG_WIDTH;
    img_dsc.header.h = HITOKOTO_IMG_HEIGHT;
    img_dsc.header.cf = LV_IMG_CF_RAW;
    
    lv_img_set_src(g_hitokoto_scr_obj.hitokoto_image, &img_dsc);
    lv_obj_align(g_hitokoto_scr_obj.hitokoto_image, NULL, LV_ALIGN_CENTER, 0, 0);
}

void hitokoto_gui_show_loading(bool show)
{
    lv_obj_set_hidden(g_hitokoto_scr_obj.hitokoto_loading, !show);
}

void hitokoto_gui_set_status_text(const char *text)
{
    lv_label_set_text(g_hitokoto_scr_obj.hitokoto_status_text, text);
    lv_obj_align(g_hitokoto_scr_obj.hitokoto_status_text, g_hitokoto_scr_obj.hitokoto_status_bar, LV_ALIGN_CENTER, 0, 0);
}

void hitokoto_gui_delete(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_clean(scr);
} 