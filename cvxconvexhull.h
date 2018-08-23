#ifndef __CVX_CONVEX_HULL_H__
#define __CVX_CONVEX_HULL_H__
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _CvxConvexHull CvxConvexHull;

#include "cvxalgorithm.h"
#include "cvxnode.h"

struct _CvxConvexHull {
    CvxAlgorithm algo;

    CvxNode*     start_node;
    GList*       convex_nodes;

    // for temporary use
    double       x, y;
};

#define CVX_CONVEX_HULL_MESSAGE      "凸法 (逐次構成法)"
#define CVX_CONVEX_HULL_BUTTON_LABEL " 計算 "

CvxAlgorithm* cvx_convex_hull_new(CvxField* field);

G_END_DECLS

#endif // __CVX_CONVEX_HULL_H__
