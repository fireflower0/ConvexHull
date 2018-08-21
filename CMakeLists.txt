cmake_minimum_required (VERSION 2.6)
project (cvx)

find_package (PkgConfig)
pkg_search_module (GTK REQUIRED gtk+-2.0)
include_directories (${GTK_INCLUDE_DIRS})

set(gui_SRCS cvxedgesorter.c cvxfield.c cvxnode.c cvxnodelist.c cvxpolygon.c)
set(algo_SRCS cvxalgorithm.c cvxutility.c)

add_executable (cvx main.c ${gui_SRCS} ${algo_SRCS})
target_link_libraries (cvx m ${GTK_LIBRARIES})
