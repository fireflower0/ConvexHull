#include <stdlib.h>
#include "cvxnodelist.h"
#include "cvxedgesorter.h"

/* ノードをランダムな位置に生成 */
static GList* cvx_node_list_create_random_nodes(CvxField* field, guint num, guint max_x, guint max_y){
    int i, x, y;
    GList* list = NULL;
    time_t t;
    CvxNode* node;

    time(&t);        /* 時刻情報の取得 */
    srand48((int)t); /* 時刻情報によるランダム・シードの設定 */

    for(i = 0; i < num; i++){
        x = max_x * 0.05 + drand48() * max_x * 0.9;
        y = max_y * 0.05 + drand48() * max_y * 0.9;

        node = cvx_node_new(field, x, y);
        if(node == NULL) return NULL;
        list = g_list_append(list, node);
    }

    list = cvx_edge_sorter_sort_list(list);
    
    return list;
}

/* ノードリストのコンストラクタ */
CvxNodeList* cvx_node_list_new(CvxField* field, guint max_x, guint max_y){
    CvxNodeList* retvar;

    retvar = (CvxNodeList*)malloc(sizeof(CvxNodeList));
    if(retvar == NULL) return NULL;

    retvar->field = field;
    retvar->nodes = cvx_node_list_create_random_nodes(field, CVX_NODE_LIST_NODES_NUM, max_x, max_y);

    return retvar;
}

/* 指定された座標に存在するノードをピックアップ */
CvxNode* cvx_node_list_pick_node(CvxNodeList* node_list, gint x, gint y){
    GList* arg_list = node_list->nodes;

    while(arg_list != NULL){
        CvxNode* arg = (CvxNode*)(arg_list->data);
        if(cvx_node_is_inside_p(arg, x, y)){
            return arg;
        }else{
            arg_list = g_list_next(arg_list);
        }
    }

    return NULL;
}

/* 描画ルーチン */
void cvx_node_list_render(CvxNodeList* node_list, cairo_t* cr){
    if(node_list->nodes != NULL){
        g_list_foreach(node_list->nodes, (GFunc)cvx_node_render, cr);
    }
}
