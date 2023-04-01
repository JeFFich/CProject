#include <stdio.h>
#include <math.h>
/* Начальный момент порядка k */
double beg_moment(double * x, double * prob, int n, double k)
{
    int i;
    double res = 0;
    for (i = 0; i < n; i++)
    {
        res = res + pow(x[i], k) * prob[i];
    }
    return res;
}
/* Иатематическое ожидание */
double math_exp(double * x, double * prob, int n)
{
    return beg_moment(x, prob, n, 1);
}
/* Центральный начальный момент порядка k */
double central_beg_moment(double * x, double * prob, int n, double k)
{
    int i;
    double exp = math_exp(x, prob, n);
    double res = 0;
    for (i = 0; i < n; i++)
    {
        res = res + pow(x[i] - exp, k) * prob[i];
    }
    return res;
}
/* Дисперсия*/
double dispersion(double * x, double * prob, int n)
{
    return central_beg_moment(x, prob, n, 2);
}
/* Среднеквадратичное отклонение */
double mean_square_deviation(double * x, double * prob, int n)
{
    return pow(dispersion(x, prob, n), 0.5);
}
/* Асимметрия */
double asymmetry(double * x, double * prob, int n)
{
    return (central_beg_moment(x, prob, n, 3) / pow(mean_square_deviation(x, prob, n), 3));
}
/* Эксцесс */
double excess(double * x, double * prob, int n)
{
    return (central_beg_moment(x, prob, n, 4) / pow(dispersion(x, prob, n), 2) - 3);
}
/* Мода */
double mode(double * x, double * prob, int n)
{
    int res = 0;
    int i;
    for (i = 1; i < n; i++)
        if (prob[res] < prob[i])
            res = i;
    return x[res];
}
/* На потестить */
int main()
{
    double x[] = {1., 2., 3.};
    double prob[] = {0.45, 0.3, 0.35};
    printf("%.1f\n", beg_moment(x, prob, 3, 1));
    printf("%.1f\n", math_exp(x, prob, 3));
    printf("%.1f\n", central_beg_moment(x, prob, 3, 2));
    printf("%.1f\n", dispersion(x, prob, 3));
    printf("%.1f\n", mean_square_deviation(x, prob, 3));
    printf("%.1f\n", asymmetry(x, prob, 3));
    printf("%.1f\n", excess(x, prob, 3));
    printf("%.1f\n", mode(x, prob, 3));
    return 0;
}
