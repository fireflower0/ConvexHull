#include <math.h>
#include "cvxutility.h"

/* ベクトルの角度を計算する */
double cvx_utility_calc_angle(double v0_x, double v0_y, double v1_x, double v1_y){
    double dot_prod   = v0_x * v1_x + v0_y * v1_y;
    double cross_prod = v0_x * v1_y - v0_y * v1_x;
    double theta      = atan2(cross_prod, dot_prod);
    
    return ((theta > 0) ? theta : theta + 2 * M_PI);
}
