#include <stdlib.h>
#include "cvxfield.h"

static gboolean cvx_field_expose_event(GtkWidget* widget, GdkEventExpose* event, gpointer user_data){
    CvxField*  field = (CvxField*)user_data;
    GdkWindow* drawable = widget->window;
    cairo_t*   cr = gdk_cairo_create(drawable);

    cvx_node_render(field->node, cr);
    cairo_destroy(cr);

    return FALSE;
}

/* 描画領域の設定 */
CvxField* cvx_field_new(GtkWindow* window, gchar* title, guint width, guint height){
    CvxField*  retvar;
    GtkWidget* canvas;

    /* メモリ領域確保 */
    retvar = (CvxField*)malloc(sizeof(CvxField));
    if(retvar == NULL) return NULL;

    /* ウィンドウのタイトルを設定 */
    gtk_window_set_title(window, title);
    
    /* ウィンドウの幅・高さを設定 */
    gtk_widget_set_size_request(GTK_WIDGET(window), width, height);
    
    retvar->window = window;

    /* 描画領域インスタンスを生成 */
    canvas = gtk_drawing_area_new();

    /* 描画領域をウィンドウに追加 */
    gtk_container_add(GTK_CONTAINER(window), canvas);

    /* ウィンドウが描画されるべきタイミング(※)で発生する expose-event で、 */
    /* コールバック関数 cvx_field_expose_event() を結びつける               */
    g_signal_connect(G_OBJECT(canvas), "expose-event", G_CALLBACK(cvx_field_expose_event), retvar);
    
    retvar->canvas = canvas;

    /* ノードインスタンスを生成 */
    retvar->node = cvx_node_new(retvar, width / 2, height / 2);

    return retvar;
}
