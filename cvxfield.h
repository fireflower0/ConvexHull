#ifndef __CVX_FIELD_H__
#define __CVX_FIELD_H__
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _CvxField CvxField;

#include "cvxnode.h"
#include "cvxnodelist.h"
#include "cvxpolygon.h"
#include "cvxalgorithm.h"

struct _CvxField{
    GtkWindow*    window;
    GtkWidget*    canvas;

    CvxNodeList*  node_list;
    CvxNode*      active_node;
    CvxPolygon*   polygon;

    CvxAlgorithm* algorithm;
};

CvxField* cvx_field_new(GtkWindow* window, gchar* title, guint width, guint height);
void      cvx_field_repaint(CvxField* field);

G_END_DECLS

#endif // __CVX_FIELD_H__
