#include <gtk/gtk.h>

int main(int argc, char* argv[]){
    GtkWidget* window;

    /* 初期化処理 */
    gtk_init(&argc, &argv);

    /* ウィンドウの設定 */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show(window);

    /* イベントループ */
    gtk_main();

    return 0;
}
