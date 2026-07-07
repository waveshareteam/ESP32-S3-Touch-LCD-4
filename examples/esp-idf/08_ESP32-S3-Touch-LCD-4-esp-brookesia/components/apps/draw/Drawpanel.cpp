#include "Drawpanel.hpp"
#include "lvgl.h"

LV_IMG_DECLARE(img_app_drawpanel);

lv_obj_t *coordinate_label;
lv_obj_t *draw_canvas;
lv_color_t *canvas_buffer;
lv_point_t last_point = { -1, -1 };

Drawpanel::Drawpanel() : ESP_Brookesia_PhoneApp("Drawpanel", &img_app_drawpanel, true)
{
}

Drawpanel::~Drawpanel()
{
    lv_mem_free(canvas_buffer);
}

bool Drawpanel::run(void)
{
    lv_area_t area = getVisualArea();
    int _width = area.x2 - area.x1;
    int _height = area.y2 - area.y1;

    lv_obj_t *panel_obj = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel_obj, _width, _height);
    lv_obj_align(panel_obj, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_event_cb(panel_obj, touch_event_cb, LV_EVENT_PRESSING, this);
    lv_obj_add_event_cb(panel_obj, touch_start_cb, LV_EVENT_PRESSED, this);

    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLL_CHAIN_VER);

    coordinate_label = lv_label_create(lv_scr_act());
    lv_label_set_text(coordinate_label, "X: 0, Y: 0");
    lv_obj_align(coordinate_label, LV_ALIGN_TOP_MID, 0, 0);

    // 创建画布对象
    draw_canvas = lv_canvas_create(lv_scr_act());
    lv_obj_set_size(draw_canvas, _width, _height);
    lv_obj_align(draw_canvas, LV_ALIGN_TOP_LEFT, 0, 0);

    // 分配画布缓冲区
    canvas_buffer = (lv_color_t *)lv_mem_alloc(_width * _height * sizeof(lv_color_t));
    lv_canvas_set_buffer(draw_canvas, canvas_buffer, _width, _height, LV_IMG_CF_TRUE_COLOR);
    lv_canvas_fill_bg(draw_canvas, lv_color_make(255, 255, 255), LV_OPA_COVER);

    return true;
}

bool Drawpanel::back(void)
{
    notifyCoreClosed();
    return true;
}

bool Drawpanel::close(void)
{
    return true;
}

bool Drawpanel::init(void)
{
    return true;
}

void Drawpanel::touch_event_cb(lv_event_t *e)
{
    lv_indev_t *indev = lv_indev_get_act();
    lv_point_t point;
    lv_indev_get_point(indev, &point);

    if (last_point.x != -1 && last_point.y != -1)
    {
        lv_point_t line_points[] = {last_point, point};
        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        line_dsc.color = lv_color_make(255, 0, 0);
        line_dsc.width = 2;
        lv_canvas_draw_line(draw_canvas, line_points, 2, &line_dsc);
    }
    last_point = point;

    char coordinate_text[20];
    snprintf(coordinate_text, sizeof(coordinate_text), "X: %d, Y: %d", point.x, point.y);
    lv_label_set_text(coordinate_label, coordinate_text);
}

void Drawpanel::touch_start_cb(lv_event_t *e)
{
    last_point.x = -1;
    last_point.y = -1;
}
