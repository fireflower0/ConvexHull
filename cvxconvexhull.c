#include <math.h>
#include "cvxconvexhull.h"
#include "cvxedgesorter.h"
#include "cvxutility.h"

/* 最小の偏角を持つ点を探す */
static CvxNode* cvx_convex_find_least_angle(CvxConvexHull* convex, GList* work_list, CvxNode* node){
    CvxNode* result;
    double   max_angle = 2 * M_PI;  // maximum angle

    while(work_list != NULL){
        CvxNode* target = (CvxNode*)work_list->data;
        double vec0_x = target->x - node->x;
        double vec0_y = target->y - node->y;
        double vec1_x = convex->x;
        double vec1_y = convex->y;
        double angle = cvx_utility_calc_angle(vec0_x, vec0_y, vec1_x, vec1_y);

        if(angle < max_angle){
            max_angle = angle;
            result = target;
        }
        work_list = g_list_next(work_list);
    }
    
    return result;
}

/* 実際の計算処理を行う */
static GList* cvx_convex_hull_calc(CvxConvexHull* convex, GList* all_nodes){
    GList*   work_list = g_list_copy(all_nodes); /* 作業用リスト、初期値は全てのノードを記録したリストのコピー */
    CvxNode* start_node = convex->start_node;
    CvxNode* node = start_node;
    CvxNode* next = NULL;                        /* 包装法で次に凸包の頂点として追加していくノードを管理する */
    GList*   convex_nodes = NULL;

    /* 偏角計算の基準となるベクトル、すなわち包装法で途中まで求まった先端の辺を示す */
    convex->x = 1.0;
    convex->y = 0.0;

    /* 最初のノードに戻るまで処理を行う */
    while(next != start_node){
        next = cvx_convex_find_least_angle(convex, work_list, node);
        
        /* 対象となるノードが探索されたら凸包頂点のリストへ追加 */
        convex_nodes = g_list_append(convex_nodes, next);

        /* 作業用リストからは除外 */
        work_list = g_list_remove(work_list, next);

        convex->x = next->x - node->x;
        convex->y = next->y - node->y;
        node = next;
    }

    g_list_free(work_list);

    return convex_nodes;
}

/* 処理開始から処理後の再描画まで */
static void cvx_convex_hull_calc_start(GtkWidget* button, gpointer user_data){
    CvxAlgorithm*  algorithm = (CvxAlgorithm*)user_data;
    CvxConvexHull* convex    = (CvxConvexHull*)algorithm;
    CvxField*      field     = algorithm->field;
    CvxPolygon*    polygon   = field->polygon;
    GList*         all_nodes = field->node_list->nodes;

    convex->start_node   = cvx_edge_sorter_find_right_end(all_nodes);
    convex->convex_nodes = cvx_convex_hull_calc(convex, all_nodes);
    cvx_polygon_set_shape(polygon, convex->convex_nodes);

    cvx_field_repaint(field);
}

/* ウィンドウ下部に表示される処理起動用のボタンを配したコントロールパネル */
static void cvx_convex_hull_build_controller(CvxAlgorithm* algorithm){
    GtkWidget* controller = gtk_hbox_new(FALSE, 0);
    GtkWidget* label      = gtk_label_new(CVX_CONVEX_HULL_MESSAGE);
    GtkWidget* button     = gtk_button_new_with_label(CVX_CONVEX_HULL_BUTTON_LABEL);

    gtk_box_pack_start(GTK_BOX(controller), label, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(controller), button, FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(cvx_convex_hull_calc_start), algorithm);

    algorithm->controller = controller;
}

/* コンストラクタ */
CvxAlgorithm* cvx_convex_hull_new(CvxField* field){
    CvxAlgorithm* retvar = cvx_algorithm_new(field, sizeof(CvxConvexHull));
    if(retvar == NULL) return NULL;

    cvx_convex_hull_build_controller(retvar);

    return retvar;
}
