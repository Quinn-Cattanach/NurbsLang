#include "nurbs_evaluate.h"

vec3f_wgsl derivative(const nurbs<1> &curve, float u) {
    vec3f_wgsl numerator(0, 0, 0);
    float denominator = 0.0;
    vec3f_wgsl numerator_deriv(0, 0, 0);
    float denominator_deriv = 0.0;

    const size_t degree = curve.degree[0];

    for (size_t i = 0; i < curve.control.size(); ++i) {
        float w = curve.weight.empty() ? 1.0f : curve.weight[i];
        float N = curve.basis(0, i, u, degree);

        float dN = 0.0f;
        if (degree > 0) {
            float denom1 = curve.knot[0][i + degree] - curve.knot[0][i];
            float denom2 = curve.knot[0][i + degree + 1] - curve.knot[0][i + 1];
            float term1 = denom1 != 0 ? degree / denom1 * curve.basis(0, i, u, degree-1) : 0.0f;
            float term2 = denom2 != 0 ? degree / denom2 * curve.basis(0, i+1, u, degree-1) : 0.0f;
            dN = term1 - term2;
        }

        numerator += N * w * curve.control[i];
        denominator += N * w;

        numerator_deriv += dN * w * curve.control[i];
        denominator_deriv += dN * w;
    }

    if (denominator == 0) return vec3f_wgsl();

    return (numerator_deriv * denominator - numerator * denominator_deriv) / (denominator * denominator);
}
