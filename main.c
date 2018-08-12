#include <gtk/gtk.h>

#define CVX_MAIN_TITLE  "main_window"
#define CVX_MAIN_WIDTH  400
#define CVX_MAIN_HEIGHT 300

int main(int argc, char* argv[]){
    GtkWidget* window;
    GtkWidget* canvas;

    /* 初期化処理 */
    gtk_init(&argc, &argv);

    /* ウィンドウの設定 */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* 描画領域ウィジェット作成 */
    canvas = gtk_drawing_area_new();

    /* ウィジェットの入れ子を定義 */
    gtk_container_add(GTK_CONTAINER(window), canvas);

    /* ウィンドウのタイトルを設定 */
    gtk_window_set_title(GTK_WINDOW(window), CVX_MAIN_TITLE);

    /* ウィンドウの大きさを指定 */
    gtk_widget_set_size_request(GTK_WIDGET(window), CVX_MAIN_WIDTH, CVX_MAIN_HEIGHT);

    /* ウィジェットを表示 */
    gtk_widget_show_all(window);

    /* イベントループ */
    gtk_main();

    return 0;
}
