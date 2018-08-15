#include <stdlib.h>
#include "cvxnode.h"

CvxNode* cvx_node_new(CvxField* field, gint x, gint y){
    CvxNode* retval;

    retval = (CvxNode*)malloc(sizeof(CvxNode));
    if(retval == NULL) return NULL;

    retval->field = field;
    retval->x = x;
    retval->y = y;
    retval->width = CVX_NODE_DEFAULT_WIDTH;
    retval->height = CVX_NODE_DEFAULT_HEIGHT;

    return retval;
}

/* ノードを描画 */
void cvx_node_render(CvxNode* node, cairo_t* cr){
    double ulx = node->x - node->width  / 2.0;   // 左上位置X座標
    double uly = node->y - node->height / 2.0;   // 左上位置Y座標

    /* 線の幅を指定 */
    cairo_set_line_width(cr, CVX_NODE_DEFAULT_WIDTH);

    /* シアン色で、長方形を描画 */
    cairo_set_source_rgb(cr, 0.0, 1.0, 1.0);
    cairo_rectangle(cr, ulx, uly, node->width, node->height);

    /* 描画後もパスを保持 */
    cairo_fill_preserve(cr);

    /* 黒色で、辺を描画 */
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_stroke(cr);
}
