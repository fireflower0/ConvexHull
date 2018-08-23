#include <math.h>
#include "cvxconvexhull.h"
#include "cvxedgesorter.h"
#include "cvxutility.h"

/* X座標でソートするための判断基準 */
static gint cvx_convex_hull_compare_func_x(gconstpointer a, gconstpointer b){
    CvxNode* node_a = (CvxNode*)a;
    CvxNode* node_b = (CvxNode*)b;

    return ((node_a->x > node_b->x) ? 1 : -1);
}

/* 外積の計算を行い、接点となるノードチェックを行う */
static gboolean cvx_convex_hull_check(CvxNode* node0, CvxNode* node1, CvxNode* node2, CvxNode* node3){
    double vec0_x = node0->x - node1->x, vec0_y = node0->y - node1->y;
    double vec1_x = node2->x - node1->x, vec1_y = node2->y - node1->y;
    double vec2_x = node0->x - node2->x, vec2_y = node0->y - node2->y;
    double vec3_x = node3->x - node2->x, vec3_y = node3->y - node2->y;
    double cross_prod1 = vec0_x * vec1_y - vec0_y * vec1_x;
    double cross_prod2 = vec2_x * vec3_y - vec2_y * vec3_x;

    return (cross_prod1 * cross_prod2 <= 0);
}

/* 凸包内部の点となるノードを削除 */
/* 着目している追加ノードから凸包候補に接線を引けるノードまでを探索し、 */
/* その時点で凸包候補内部に含まれてしまう点を削除する処理               */
static GList* cvx_convex_hull_remove_nodes(GList* convex_nodes, CvxNode* target, GList* prev, GList* next){
    GList* nnext;
    GList* pprev;

    /* ノードを半時計回りに探索し、接点となるノードを探す */
    for(;;){
        nnext = g_list_next(next);
        if(nnext == NULL) nnext = convex_nodes;
        /* 接点となるノードの判定 */
        if(cvx_convex_hull_check(target,
                                 (CvxNode*)prev->data,
                                 (CvxNode*)next->data,
                                 (CvxNode*)nnext->data))
            break;
        convex_nodes = g_list_remove(convex_nodes, next->data);
        next = nnext;
    }

    /* ノードを時計回りに探索し、下側から接する点を探す */
    for(;;){
        pprev = g_list_previous(prev);
        if(pprev == NULL) pprev = g_list_last(convex_nodes);
        /* 接点となるノードの判定 */
        if(cvx_convex_hull_check(target,
                                 (CvxNode*)next->data,
                                 (CvxNode*)prev->data,
                                 (CvxNode*)pprev->data))
            break;
        convex_nodes = g_list_remove(convex_nodes, prev->data);
        prev = pprev;
    }

    return convex_nodes;
}

/* 実際の計算処理を行う */
static GList* cvx_convex_hull_calc(CvxConvexHull* convex, GList* all_nodes){
    int      i;
    GList*   work_list = g_list_copy(all_nodes); /* 作業用リスト、初期値は全てのノードを記録したリストのコピー */
    GList*   list_ptr;
    GList*   convex_nodes = NULL;

    /* 全てのノードをX座標でソート */
    list_ptr = work_list = g_list_sort(work_list, cvx_convex_hull_compare_func_x);

    /* 最初の三角形を定義 */
    for(i = 0; i < 3; i++){
        /* 最初の3点を、凸包の候補として、convex_nodesリストに格納 */
        CvxNode* node = (CvxNode*)list_ptr->data;
        convex_nodes = g_list_append(convex_nodes, node);
        list_ptr = g_list_next(list_ptr);
    }

    /* 残りのノードに対して凸包に含まれるか否かの判定を行う */
    while(list_ptr != NULL){
        CvxNode* target = (CvxNode*)list_ptr->data;
        GList* prev;
        GList* next;

        /* 凸包の候補を半時計回りにソート */
        convex_nodes = cvx_edge_sorter_sort_list(convex_nodes);

        /* 着目するノードを挟むノードを抽出 */
        cvx_edge_sorter_find_nodes(convex_nodes, target, &prev, &next);

        /* 凸包内部の点となるノードを削除 */
        convex_nodes = cvx_convex_hull_remove_nodes(convex_nodes, target, prev, next);

        /* 対象のノードを追加 */
        convex_nodes = g_list_append(convex_nodes, target);

        list_ptr = g_list_next(list_ptr);
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
