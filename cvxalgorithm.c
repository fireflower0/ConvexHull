#include "cvxalgorithm.h"

CvxAlgorithm* cvx_algorithm_new(CvxField* field, size_t objsize){
    CvxAlgorithm* retvar = (CvxAlgorithm*)malloc(objsize);
    if(retvar == NULL) return NULL;

    retvar->field = field;
    retvar->controller = NULL;

    return retvar;
}

GtkWidget* cvx_algorithm_get_controller(CvxAlgorithm* algorithm){
    if(algorithm->controller  == NULL){
        g_warning("Control panel is not configured\n");
    }
    return algorithm->controller;
}
