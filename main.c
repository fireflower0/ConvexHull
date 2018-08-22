#include "cvxfield.h"

#define CVX_MAIN_TITLE   "main_window"
#define CVX_MAIN_WIDTH   800
#define CVX_MAIN_HEIGHT  600
#define CVX_MAIN_GBCOLOR "White"

int main(int argc, char* argv[]){
    GtkWidget* window;
    CvxField*  field;

    /* 初期化処理 */
    gtk_init(&argc, &argv);

    /* ウィンドウを生成 */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    /* destroyイベントでコールバック関数 gtk_main_quit() を結びつける */
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* 描画領域の設定 */
    field = cvx_field_new(GTK_WINDOW(window), CVX_MAIN_TITLE, CVX_MAIN_WIDTH, CVX_MAIN_HEIGHT);
    
    /* ウィジェットを表示 */
    gtk_widget_show_all(window);

    /* GDKレベルのウィンドウ操作(背景を白く変更) */
    {
        GdkColor   color;
        GdkScreen* screen;

        gdk_color_parse(CVX_MAIN_GBCOLOR, &color);
        screen = gdk_display_get_default_screen(gdk_display_get_default());
        gdk_rgb_find_color(gdk_screen_get_rgb_colormap(screen), &color);
        gdk_window_set_background(field->canvas->window, &color);
    }

    /* イベントループ */
    gtk_main();

    return 0;
}
