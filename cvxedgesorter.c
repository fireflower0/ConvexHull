#include <math.h>
#include "cvxnode.h"

static double   gx, gy;  /* 重心の座標 */
static CvxNode* node0;   /* 基準とするノードへのポインタ */

/* 最も右端(X座標の値が最大)のノードを探す */
static void cvx_edge_sorter_find_right_end(GList* list){
    CvxNode* node;
    CvxNode* tmp_node = NULL;
    int x = 0;

    while(list != NULL){
        node = (CvxNode*)list->data;
        if(node->x > x){
            x = node->x;
            tmp_node = node;
        }
        list = g_list_next(list);
    }
    node0 = tmp_node;
}

/* 全てのノードを走査し、重心座標を計算 */
static void cvx_edge_sorter_find_gravity_center(GList* list){
    CvxNode* node;
    int i = 0, x = 0, y = 0;

    while(list != NULL){
        node = (CvxNode*)list->data;
        x += node->x;
        y += node->y;
        i++;
        list = g_list_next(list);
    }

    gx = (double)x / (double)i;
    gy = (double)y / (double)i;
}

/* 引数で与えるノードと変数node0で表されるノード、それぞれが重心を見込む角度を求める */
/* ベクトルのなす角度はそれぞれ内積と外積の組み合わせで計算 */
static double cvx_edge_sorter_calc_angle(CvxNode* node1){
    double vec0_x, vec0_y, vec1_x, vec1_y;
    double dot_prod, cross_prod;
    double theta;

    if(node0 == node1){
        return 0.0;
    }else{
        vec0_x = node0->x - gx;
        vec0_y = node0->y - gy;
        vec1_x = node1->x - gx;
        vec1_y = node1->y - gy;
        dot_prod = vec0_x * vec1_x + vec0_y * vec1_y;
        cross_prod = vec0_x * vec1_y - vec0_y * vec1_x;

        theta = atan2(cross_prod, dot_prod);
        return ((theta > 0) ? theta : theta + 2 * M_PI);
    }
}

/* 基準線に対する各ノードの角度を計算 */
static gint cvx_edge_sorter_compare_function(gconstpointer a, gconstpointer b){
    CvxNode* node_a = (CvxNode*)a;
    CvxNode* node_b = (CvxNode*)b;
    double angle_a = cvx_edge_sorter_calc_angle(node_a);
    double angle_b = cvx_edge_sorter_calc_angle(node_b);

    return ((angle_a < angle_b) ? 1 : -1);
}

/* 引数で与えたリストが持つノードを並び替えた結果を返す */
GList* cvx_edge_sorter_sort_list(GList* list){
    /* 基準ノードのポインタと重心の座標を設定 */
    cvx_edge_sorter_find_right_end(list);
    cvx_edge_sorter_find_gravity_center(list);
    
    /* 角度によるノードの並び替え */
    list = g_list_sort(list, cvx_edge_sorter_compare_function);

    return list;
}
