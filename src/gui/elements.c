#include "elements.h"
#include "main.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/input.h>
#include <linux/kd.h>

#define BACKGROUND_COLOR LV_COLOR_MAKE(10, 10, 10);
void gui_elements_background(void)
{
  static lv_style_t style;
  lv_style_copy(&style, &lv_style_plain);
  style.body.shadow.width = 6;
  style.body.padding.hor = 5;                                 /*Set a great horizontal padding*/
  style.body.main_color = BACKGROUND_COLOR;
  style.body.grad_color = BACKGROUND_COLOR;

  /*Create an other container*/
  lv_obj_t * box1;
  box1 = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_style(box1, &style);     /*Set the new style*/
  lv_cont_set_fit(box1, true, false); /*Do not enable the vertical fit */
  //lv_obj_set_height(box2, 55);        /*Set a fix height*/
  lv_obj_set_size(box1, 1680, 1050);
}
#define DRO_BACKGROUND_COLOR LV_COLOR_MAKE(0, 0, 0);
#define DRO_TEXT_COLOR LV_COLOR_MAKE(0, 255, 0);
lv_obj_t *dro_container;
lv_obj_t *x_wcs_dro;
lv_obj_t *y_wcs_dro;
lv_obj_t *z_wcs_dro;
lv_obj_t *x_abs_dro;
lv_obj_t *y_abs_dro;
lv_obj_t *z_abs_dro;
void gui_elements_dro_close()
{
  if (dro_container != NULL)
  {
    lv_obj_del(dro_container);
    dro_container = NULL;
  }
}
void gui_elements_dro_update_wcs(float x, float y, float z)
{
  if (dro_container != NULL)
  {
    char x_text[20];
    char y_text[20];
    char z_text[20];
    sprintf(x_text, "%0.4f", x);
    sprintf(y_text, "%0.4f", y);
    sprintf(z_text, "%0.4f", z);
    lv_label_set_text(x_wcs_dro, x_text);
    lv_label_set_text(y_wcs_dro, y_text);
    lv_label_set_text(z_wcs_dro, z_text);
  }
}
void gui_elements_dro_update_abs(float x, float y, float z)
{
  if (dro_container != NULL)
  {
    char x_text[20];
    char y_text[20];
    char z_text[20];
    sprintf(x_text, "%0.4f", x);
    sprintf(y_text, "%0.4f", y);
    sprintf(z_text, "%0.4f", z);
    lv_label_set_text(x_abs_dro, x_text);
    lv_label_set_text(y_abs_dro, y_text);
    lv_label_set_text(z_abs_dro, z_text);
  }
}
lv_obj_t *gui_elements_dro(void)
{
  static lv_style_t small_text_style;
  lv_style_copy(&small_text_style, &lv_style_plain);
  small_text_style.text.color = DRO_TEXT_COLOR;
  small_text_style.text.font = &lv_font_dejavu_10;

  static lv_style_t text_style;
  lv_style_copy(&text_style, &lv_style_plain);
  text_style.text.color = DRO_TEXT_COLOR;
  text_style.text.font = &lv_font_dejavu_40;

  static lv_style_t style;
  lv_style_copy(&style, &lv_style_plain);
  style.body.shadow.width = 6;
  style.body.padding.hor = 5;
  style.body.main_color = DRO_BACKGROUND_COLOR;
  style.body.grad_color = DRO_BACKGROUND_COLOR;

  /*Create an other container*/
  dro_container = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_style(dro_container, &style);     /*Set the new style*/
  //lv_cont_set_fit(dro_container, true, false); /*Do not enable the vertical fit */
  lv_obj_set_size(dro_container, 400, 300);
  lv_obj_align(dro_container, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
  //Left Side
  lv_obj_t *x_static = lv_label_create(dro_container, NULL);
  lv_obj_set_style(x_static, &text_style);
  lv_label_set_text(x_static, "X");
  lv_obj_align(x_static, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);

  lv_obj_t *y_static = lv_label_create(dro_container, NULL);
  lv_obj_set_style(y_static, &text_style);
  lv_label_set_text(y_static, "Y");
  lv_obj_align(y_static, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 120);

  lv_obj_t *z_static = lv_label_create(dro_container, NULL);
  lv_obj_set_style(z_static, &text_style);
  lv_label_set_text(z_static, "Z");
  lv_obj_align(z_static, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 230);
  //Right side, wcs
  x_wcs_dro = lv_label_create(dro_container, NULL);
  lv_obj_set_style(x_wcs_dro, &text_style);
  lv_label_set_text(x_wcs_dro, "0.0000");
  lv_obj_align(x_wcs_dro, NULL, LV_ALIGN_IN_TOP_RIGHT, -70, 10);

  y_wcs_dro = lv_label_create(dro_container, NULL);
  lv_obj_set_style(y_wcs_dro, &text_style);
  lv_label_set_text(y_wcs_dro, "0.0000");
  lv_obj_align(y_wcs_dro, NULL, LV_ALIGN_IN_TOP_RIGHT, -70, 120);

  z_wcs_dro = lv_label_create(dro_container, NULL);
  lv_obj_set_style(z_wcs_dro, &text_style);
  lv_label_set_text(z_wcs_dro, "0.0000");
  lv_obj_align(z_wcs_dro, NULL, LV_ALIGN_IN_TOP_RIGHT, -70, 230);

  //Left side, abs
  x_abs_dro = lv_label_create(dro_container, NULL);
  lv_obj_set_style(x_abs_dro, &small_text_style);
  lv_label_set_text(x_abs_dro, "0.0000");
  lv_obj_align(x_abs_dro, NULL, LV_ALIGN_IN_TOP_LEFT, 60, 30);

  y_abs_dro = lv_label_create(dro_container, NULL);
  lv_obj_set_style(y_abs_dro, &small_text_style);
  lv_label_set_text(y_abs_dro, "0.0000");
  lv_obj_align(y_abs_dro, NULL, LV_ALIGN_IN_TOP_LEFT, 60, 140);

  z_abs_dro = lv_label_create(dro_container, NULL);
  lv_obj_set_style(z_abs_dro, &small_text_style);
  lv_label_set_text(z_abs_dro, "0.0000");
  lv_obj_align(z_abs_dro, NULL, LV_ALIGN_IN_TOP_LEFT, 60, 250);


  return dro_container;
}
#define JOG_BACKGROUND_COLOR LV_COLOR_MAKE(0, 0, 0);
#define JOG_TEXT_COLOR LV_COLOR_MAKE(0, 255, 0);
lv_obj_t *jog_container;
static lv_res_t slider_action(lv_obj_t * slider)
{
    //printf("New slider value: %d\n", lv_slider_get_value(slider));

    return LV_RES_OK;
}
/*Called when a button is released or long pressed*/
static lv_res_t btnm_action(lv_obj_t * btnm, const char *txt)
{
  printf("Button: %s released\n", txt);

  return LV_RES_OK; /*Return OK because the button matrix is not deleted*/
}
lv_obj_t *gui_elements_jog(void)
{
  lv_obj_t *jog_slider_container = lv_cont_create(lv_scr_act(), NULL);
  //v_obj_set_style(jog_slider_container, &style);     /*Set the new style*/
  //lv_cont_set_fit(dro_container, true, false); /*Do not enable the vertical fit */
  lv_obj_set_size(jog_slider_container, 400, 100);
  lv_obj_align(jog_slider_container, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 370);
  /*Create a bar, an indicator and a knob style*/
  static lv_style_t style_bg;
  static lv_style_t style_indic;
  static lv_style_t style_knob;

  lv_style_copy(&style_bg, &lv_style_pretty);
  style_bg.body.main_color =  LV_COLOR_BLACK;
  style_bg.body.grad_color =  LV_COLOR_GRAY;
  style_bg.body.radius = LV_RADIUS_CIRCLE;
  style_bg.body.border.color = LV_COLOR_WHITE;

  lv_style_copy(&style_indic, &lv_style_pretty);
  style_indic.body.grad_color =  LV_COLOR_GREEN;
  style_indic.body.main_color =  LV_COLOR_LIME;
  style_indic.body.radius = LV_RADIUS_CIRCLE;
  style_indic.body.shadow.width = 10;
  style_indic.body.shadow.color = LV_COLOR_LIME;
  style_indic.body.padding.hor = 3;
  style_indic.body.padding.ver = 3;

  lv_style_copy(&style_knob, &lv_style_pretty);
  style_knob.body.radius = LV_RADIUS_CIRCLE;
  style_knob.body.opa = LV_OPA_70;
  style_knob.body.padding.ver = 10 ;

  /*Create a slider*/
  lv_obj_t * slider2 = lv_slider_create(lv_scr_act(), NULL);
  lv_slider_set_style(slider2, LV_SLIDER_STYLE_BG, &style_bg);
  lv_slider_set_style(slider2, LV_SLIDER_STYLE_INDIC,&style_indic);
  lv_slider_set_style(slider2, LV_SLIDER_STYLE_KNOB, &style_knob);
  lv_slider_set_action(slider2, slider_action);
  lv_obj_align(slider2, NULL, LV_ALIGN_IN_TOP_RIGHT, -100, 410);

  /*Create a label*/
  lv_obj_t * slider2_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(slider2_label, "Jog Speed");
  lv_obj_align(slider2_label, slider2, LV_ALIGN_IN_TOP_MID, 0, -25);

  /*Create a button descriptor string array*/
  static const char * btnm_map[] = {"Contin", "0.1", "0.01", "0.0001", "\n",
                             "X=0", "Y=0", "Z=0", "\n",
                             "\202Probe Z", "Go Home", "\n",
                             "Torch On", "Torch Off", "\n",
                             "Cycle Start", "Pause", "Stop", ""};

  /*Create a new style for the button matrix back ground*/
  static lv_style_t matrix_style_bg;
  lv_style_copy(&matrix_style_bg, &lv_style_plain);
  matrix_style_bg.body.main_color = LV_COLOR_SILVER;
  matrix_style_bg.body.grad_color = LV_COLOR_SILVER;
  matrix_style_bg.body.padding.hor = 0;
  matrix_style_bg.body.padding.ver = 0;
  matrix_style_bg.body.padding.inner = 0;

  /*Create 2 button styles*/
  static lv_style_t style_btn_rel;
  static lv_style_t style_btn_pr;
  lv_style_copy(&style_btn_rel, &lv_style_btn_rel);
  style_btn_rel.body.main_color = LV_COLOR_MAKE(0x30, 0x30, 0x30);
  style_btn_rel.body.grad_color = LV_COLOR_BLACK;
  style_btn_rel.body.border.color = LV_COLOR_SILVER;
  style_btn_rel.body.border.width = 1;
  style_btn_rel.body.border.opa = LV_OPA_50;
  style_btn_rel.body.radius = 0;

  lv_style_copy(&style_btn_pr, &style_btn_rel);
  style_btn_pr.body.main_color = LV_COLOR_MAKE(0x55, 0x96, 0xd8);
  style_btn_pr.body.grad_color = LV_COLOR_MAKE(0x37, 0x62, 0x90);
  style_btn_pr.text.color = LV_COLOR_MAKE(0xbb, 0xd5, 0xf1);

  /*Create a second button matrix with the new styles*/
  lv_obj_t * btnm2 = lv_btnm_create(lv_scr_act(), NULL);
  lv_btnm_set_style(btnm2, LV_BTNM_STYLE_BG, &matrix_style_bg);
  lv_btnm_set_style(btnm2, LV_BTNM_STYLE_BTN_REL, &style_btn_rel);
  lv_btnm_set_style(btnm2, LV_BTNM_STYLE_BTN_PR, &style_btn_pr);
  lv_btnm_set_map(btnm2, btnm_map);
  lv_btnm_set_action(btnm2, btnm_action);
  lv_obj_set_size(btnm2, 400, 600);
  lv_obj_align(btnm2, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -10, 0);

  return jog_container;
}
void gui_elements_jog_close()
{
  if (jog_container != NULL)
  {
    lv_obj_del(jog_container);
    jog_container = NULL;
  }
}
