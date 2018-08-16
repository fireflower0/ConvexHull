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

static void cvx_field_set_cursor(CvxField* field, gint x, gint y){
    gboolean      is_node;
    GdkCursorType cursor_type;
    GdkDisplay*   display;
    GdkCursor*    cursor;

    if(field->in_operation){
        cursor_type = GDK_FLEUR;
    }else{
        is_node = cvx_node_is_inside_p(field->node, x, y);
        cursor_type = is_node ? GDK_HAND1 : GDK_ARROW;
    }

    display = gdk_display_get_default();
    cursor  = gdk_cursor_new_for_display(display, cursor_type);

    gdk_window_set_cursor(field->canvas->window, cursor);
}

static gboolean cvx_field_button_pressed(GtkWidget* widget, GdkEventButton* event, gpointer user_data){
    gint      x = event->x, y = event->y;
    CvxField* field = (CvxField*)user_data;

    if(cvx_node_is_inside_p(field->node, x, y)){
        field->in_operation = TRUE;
        cvx_field_set_cursor(field, x, y);
    }

    return FALSE;
}

static gboolean cvx_field_button_released(GtkWidget* widget, GdkEventButton* event, gpointer user_data){
    gint x = event->x, y = event->y;
    CvxField* field = (CvxField*)user_data;

    field->in_operation = FALSE;
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
        field->in_operation = FALSE;
    }

    cvx_field_set_cursor(field, x, y);

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

    gtk_widget_set_events(canvas, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
    g_signal_connect(G_OBJECT(canvas), "motion-notify-event",  G_CALLBACK(cvx_field_mouse_move),      retvar);
    g_signal_connect(G_OBJECT(canvas), "button-press-event",   G_CALLBACK(cvx_field_button_pressed),  retvar);
    g_signal_connect(G_OBJECT(canvas), "button-release-event", G_CALLBACK(cvx_field_button_released), retvar);
    
    retvar->canvas = canvas;

    /* ノードインスタンスを生成 */
    retvar->node = cvx_node_new(retvar, width / 2, height / 2);
    
    retvar->in_operation = FALSE;

    return retvar;
}
