#include <stdlib.h>
#include "cvxfield.h"

static gboolean cvx_field_expose_event(GtkWidget* widget, GdkEventExpose* event, gpointer user_data){
    CvxField*  field = (CvxField*)user_data;
    GdkWindow* drawable = widget->window;
    cairo_t*   cr;

    gdk_window_clear(drawable);

    cr = gdk_cairo_create(drawable);
    cvx_polygon_render(field->polygon, cr);      /* 多角形を描画 */
    cvx_node_list_render(field->node_list, cr);
    cairo_destroy(cr);

    return FALSE;
}

/* 操作中か否かを判定 */
static gboolean cvx_field_is_in_operation_p(CvxField* field){
    return (field->active_node != NULL);
}

/* ノード移動操作を開始 */
static void cvx_field_start_operation(CvxField* field, gint x, gint y, CvxNode* node){
    field->active_node = node;
    cvx_node_set_difference(field->active_node, x, y);
}

/* ノード移動操作を終了 */
static void cvx_field_finish_operation(CvxField* field, gint x, gint y){
    field->active_node = NULL;
}

static void cvx_field_set_cursor(CvxField* field, gint x, gint y){
    gboolean      is_node;
    GdkCursorType cursor_type;
    GdkDisplay*   display;
    GdkCursor*    cursor;

    if(cvx_field_is_in_operation_p(field)){
        cursor_type = GDK_FLEUR;
    }else{
        CvxNode* node = cvx_node_list_pick_node(field->node_list, x, y);
        cursor_type = (node != NULL) ? GDK_HAND1 : GDK_ARROW;
    }

    display = gdk_display_get_default();
    cursor  = gdk_cursor_new_for_display(display, cursor_type);

    gdk_window_set_cursor(field->canvas->window, cursor);
}

static gboolean cvx_field_button_pressed(GtkWidget* widget, GdkEventButton* event, gpointer user_data){
    gint      x = event->x, y = event->y;
    CvxField* field = (CvxField*)user_data;
    CvxNode*  node = cvx_node_list_pick_node(field->node_list, x, y);

    if(node != NULL){
        cvx_field_start_operation(field, x, y, node);
        cvx_field_set_cursor(field, x, y);
    }

    return FALSE;
}

static gboolean cvx_field_button_released(GtkWidget* widget, GdkEventButton* event, gpointer user_data){
    gint x = event->x, y = event->y;
    CvxField* field = (CvxField*)user_data;

    cvx_field_finish_operation(field, x, y);
    cvx_field_set_cursor(field, x, y);

    return FALSE;
}

static gboolean cvx_field_mouse_move(GtkWidget* widget, GdkEventMotion* event, gpointer user_data){
    gint       x = event->x, y = event->y;
    gint       x0, y0, width, height, depth;
    CvxField*  field = (CvxField*)user_data;
    GdkWindow* drawable = widget->window;

    gdk_window_get_geometry(drawable, &x0, &y0, &width, &height, &depth);

    if((x < 0) || (y < 0) || (x >= width) || (y >= height)){
        cvx_field_finish_operation(field, x, y);
    }

    if(cvx_field_is_in_operation_p(field)){
        cvx_node_move_to(field->active_node, x, y);
        cvx_field_expose_event(widget, NULL, user_data);
    }

    cvx_field_set_cursor(field, x, y);

    return FALSE;
}

/* キーの値に関する定義をインクルード */
#include <gdk/gdkkeysyms.h>

/* キー押下時のイベント */
static gboolean cvx_field_key_pressed(GtkWidget* widget, GdkEventKey* event, gpointer user_data){
    if((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_q)){
        /* Ctrl + q が押下されたら終了 */
        gtk_main_quit();
    }
    return FALSE;
}

/* イベント設定処理 */
static void cvx_field_set_events_on_canvas(CvxField* field, GtkWidget* canvas){
    /* ウィンドウが描画されるべきタイミング(※)で発生する expose-event で、 */
    /* コールバック関数 cvx_field_expose_event() を結びつける               */
    g_signal_connect(G_OBJECT(canvas), "expose-event", G_CALLBACK(cvx_field_expose_event), field);

    gtk_widget_set_events(canvas, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

    /* マウス操作に関するシグナル設定 */
    g_signal_connect(G_OBJECT(canvas), "motion-notify-event",  G_CALLBACK(cvx_field_mouse_move),      field);
    g_signal_connect(G_OBJECT(canvas), "button-press-event",   G_CALLBACK(cvx_field_button_pressed),  field);
    g_signal_connect(G_OBJECT(canvas), "button-release-event", G_CALLBACK(cvx_field_button_released), field);

    /* キー押下時のシグナル設定 */
    g_signal_connect(G_OBJECT(field->window), "key-press-event", G_CALLBACK(cvx_field_key_pressed), field);
}

/* ウィンドウへパッキングしてウィジェットを並べる */
static void cvx_field_pack_widget(GtkWindow* window, GtkWidget* canvas, GtkWidget* controller){
    /* それぞれのウィジェットを縦に並べるパッキングボックスGtkBoxを利用 */
    GtkWidget* vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    /* 多角形を描画するキャンバス */
    gtk_box_pack_start(GTK_BOX(vbox), canvas, TRUE, TRUE, 0);

    /* コントローラ */
    gtk_box_pack_start(GTK_BOX(vbox), controller, FALSE, FALSE, 0);
}

/* 描画領域の設定 */
CvxField* cvx_field_new(GtkWindow* window, gchar* title, guint width, guint height){
    CvxField*     retvar;
    CvxAlgorithm* algorithm;
    GtkWidget*    canvas;
    GtkWidget*    controller;

    /* メモリ領域確保 */
    retvar = (CvxField*)malloc(sizeof(CvxField));
    if(retvar == NULL) return NULL;

    /* ウィンドウのタイトルを設定 */
    gtk_window_set_title(window, title);
    retvar->window = window;

    /* 描画領域インスタンスを生成 */
    canvas = gtk_drawing_area_new();
    
    /* ウィンドウの幅・高さを設定 */
    gtk_widget_set_size_request(GTK_WIDGET(window), width, height);

    /* イベント設定 */
    cvx_field_set_events_on_canvas(retvar, canvas);
    
    retvar->canvas = canvas;

    /* アルゴリズムのインスタンスを生成 */
    algorithm = cvx_algorithm_new(retvar, sizeof(CvxAlgorithm));

    /* 各アルゴリズムによって提示されるコントローラへのポインタを取得 */
    controller = cvx_algorithm_get_controller(algorithm);

    /* ウィジェット配置 */
    cvx_field_pack_widget(window, canvas, controller);
    
    retvar->algorithm = algorithm;

    /* ノードインスタンスを生成 */
    retvar->node_list = cvx_node_list_new(retvar, width, height);

    /* 多角形の生成処理 */
    retvar->polygon   = cvx_polygon_new(retvar, retvar->node_list->nodes);
    
    retvar->active_node = NULL;
    
    return retvar;
}

void cvx_field_repaint(CvxField* field){
    cvx_field_expose_event(field->canvas, NULL, field);
}
