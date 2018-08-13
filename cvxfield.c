#include <stdlib.h>
#include "cvxfield.h"

CvxField* cvx_field_new(GtkWindow* window, gchar* title, guint width, guint height){
    CvxField* retval;
    GtkWidget* canvas;

    retval = (CvxField*)malloc(sizeof(CvxField));

    if(retval == NULL) return NULL;

    gtk_window_set_title(window, title);
    gtk_widget_set_size_request(GTK_WIDGET(window), width, height);
    retval->window = window;

    canvas = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), canvas);
    retval->canvas = canvas;

    return retval;
}
