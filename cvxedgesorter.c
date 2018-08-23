#include "cvxnode.h"
#include "cvxedgesorter.h"
#include "cvxutility.h"

static double   gx, gy;  /* 重心の座標 */

void cvx_edge_sorter_find_nodes(GList* list, CvxNode* target, GList** prev, GList** next){
    GList* p = g_list_last(list);
    GList* n = g_list_first(list);
    GList* list_ptr = list;

    double theta = cvx_utility_calc_angle(1.0, 0.0, target->x - gx, target->y - gy);
    double phi;

    while(list_ptr){
        CvxNode* node = (CvxNode*)list_ptr->data;
        phi = cvx_utility_calc_angle(1.0, 0.0, node->x - gx, node->y - gy);
        if(phi < theta) break;
        p = list_ptr;
        n = list_ptr = g_list_next(list_ptr);
    }

    if(n == NULL){
        n = g_list_first(list);
    }
    *prev = p;
    *next = n;
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

    vec0_x = 1.0;
    vec0_y = 0.0;
    vec1_x = node1->x - gx;
    vec1_y = node1->y - gy;

    return ((vec1_x * vec1_x + vec1_y * vec1_y == 0.0) ? 0.0 : cvx_utility_calc_angle(vec0_x, vec0_y, vec1_x, vec1_y));
}

/* 基準線に対する各ノードの角度を計算 */
static gint cvx_edge_sorter_compare_function(gconstpointer a, gconstpointer b){
    CvxNode* node_a = (CvxNode*)a;
    CvxNode* node_b = (CvxNode*)b;
    double angle_a = cvx_edge_sorter_calc_angle(node_a);
    double angle_b = cvx_edge_sorter_calc_angle(node_b);

    return ((angle_a < angle_b) ? 1 : -1);
}

/* 最も右端(X座標の値が最大)のノードを探す */
CvxNode* cvx_edge_sorter_find_right_end(GList* list){
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

    return tmp_node;
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
