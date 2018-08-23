#ifndef __CVX_EDGE_SORTER_H__
#define __CVX_EDGE_SORTER_H__
#include <gtk/gtk.h>
#include "cvxnode.h"

G_BEGIN_DECLS

void     cvx_edge_sorter_find_nodes(GList* list, CvxNode* target, GList** prev, GList** next);
CvxNode* cvx_edge_sorter_find_right_end(GList* list);
GList*   cvx_edge_sorter_sort_list(GList* list);

G_END_DECLS

#endif // __CVX_EDGE_SORTER_H__
