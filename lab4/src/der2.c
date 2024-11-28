#include <math.h>

#ifdef __cplusplus
extern "C"
#endif
float Derivative(float A, float deltaX)
{
    return (cosf(A+deltaX)-cosf(A-deltaX)) / (2*deltaX);
}