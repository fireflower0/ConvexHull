#ifndef __CVX_ALGORITHM_H__
#define __CVX_ALGORITHM_H__
#include <gtk/gtk.h>
#include <stdlib.h>

G_BEGIN_DECLS

typedef struct _CvxAlgorithm CvxAlgorithm;

#include "cvxfield.h"

struct _CvxAlgorithm {
    CvxField*  field;
    GtkWidget* controller;
};

CvxAlgorithm* cvx_algorithm_new(CvxField* field, size_t objsize);
GtkWidget*    cvx_algorithm_get_controller(CvxAlgorithm* algorithm);

G_END_DECLS

#endif // __CVX_ALGORITHM_H__
