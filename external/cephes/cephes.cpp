#include "cephes.h"
#include <cassert>
#include <cmath>

namespace cephes
{
    // constants from const.h
    const double MACHEP = 1.11022302462515654042E-16; // 2**-53
    double MAXLOG = 7.09782712893383996732E2;         // log(MAXNUM)
    const double MINLOG = -7.451332191019412076235E2; // log(2**-1075)
#define MAXGAM 171.624376956302725

    double polevl(double x, const double coef[], int N)
    {
        double ans;
        int i;
        const double *p;

        p = coef;
        ans = *p++;
        i = N;

        do
            ans = ans * x + *p++;
        while (--i);

        return (ans);
    }

    /* p1evl()
     * Evaluate polynomial when coefficient of x^N  is 1.0.
     * Otherwise same as polevl.
     */

    double p1evl(double x, const double coef[], int N)
    {
        double ans;
        const double *p;
        int i;

        p = coef;
        ans = x + *p++;
        i = N - 1;

        do
            ans = ans * x + *p++;
        while (--i);

        return (ans);
    }

    double fdtri(int df1, int df2, double p)
    {
        assert(df1 > 0);
        assert(df2 > 0);
        assert((0 < p) && (p <= 1));

        double a = df1;
        double b = df2;
        double w, x;

        /* Compute probability for x = 0.5.  */
        w = incbet(0.5 * b, 0.5 * a, 0.5);
        /* If that is greater than y, then the solution w < .5.
           Otherwise, solve at 1-y to remove cancellation in (b - b*w).  */
        if (w > p || p < 0.001)
        {
            w = incbi(0.5 * b, 0.5 * a, p);
            x = (b - b * w) / (a * w);
        }
        else
        {
            w = incbi(0.5 * a, 0.5 * b, 1.0 - p);
            x = b * w / (a * (1.0 - w));
        }
        return (x);
    }

    // functions internally used by incbet
    const double big = 4.503599627370496e15;
    const double biginv = 2.22044604925031308085e-16;

    double incbcf(double a, double b, double x);
    double incbd(double a, double b, double x);
    double pseries(double a, double b, double x);

    double incbet(double aa, double bb, double xx)
    {
        assert(aa > 0);
        assert(bb > 0);
        assert((0 <= xx) && (xx <= 1));

        double a, b, t, x, xc, w, y;

        if (xx == 0.0)
            return (0.0);
        if (xx == 1.0)
            return (1.0);

        int flag = 0;
        if ((bb * xx) <= 1.0 && xx <= 0.95)
        {
            t = pseries(aa, bb, xx);
            goto done;
        }

        w = 1.0 - xx;

        /* Reverse a and b if x is greater than the mean. */
        if (xx > (aa / (aa + bb)))
        {
            flag = 1;
            a = bb;
            b = aa;
            xc = xx;
            x = w;
        }
        else
        {
            a = aa;
            b = bb;
            xc = w;
            x = xx;
        }

        if (flag == 1 && (b * x) <= 1.0 && x <= 0.95)
        {
            t = pseries(a, b, x);
            goto done;
        }

        /* Choose expansion for better convergence. */
        y = x * (a + b - 2.0) - (a - 1.0);
        if (y < 0.0)
            w = incbcf(a, b, x);
        else
            w = incbd(a, b, x) / xc;

        /* Multiply w by the factor
             a      b   _             _     _
            x  (1-x)   | (a+b) / ( a | (a) | (b) ) .   */

        y = a * log(x);
        t = b * log(xc);
        if ((a + b) < MAXGAM && fabs(y) < MAXLOG && fabs(t) < MAXLOG)
        {
            t = pow(xc, b);
            t *= pow(x, a);
            t /= a;
            t *= w;
            t *= tgamma(a + b) / (tgamma(a) * tgamma(b));
            goto done;
        }
        /* Resort to logarithms.  */
        y += t + lgamma(a + b) - lgamma(a) - lgamma(b);
        y += log(w / a);
        if (y < MINLOG)
            t = 0.0;
        else
            t = exp(y);

    done:

        if (flag == 1)
        {
            if (t <= MACHEP)
                t = 1.0 - MACHEP;
            else
                t = 1.0 - t;
        }
        return (t);
    }

    /* Continued fraction expansion #1
     * for incomplete beta integral
     */

    double incbcf(double a, double b, double x)
    {
        double xk, pk, pkm1, pkm2, qk, qkm1, qkm2;
        double k1, k2, k3, k4, k5, k6, k7, k8;
        double r, t, ans, thresh;
        int n;

        k1 = a;
        k2 = a + b;
        k3 = a;
        k4 = a + 1.0;
        k5 = 1.0;
        k6 = b - 1.0;
        k7 = k4;
        k8 = a + 2.0;

        pkm2 = 0.0;
        qkm2 = 1.0;
        pkm1 = 1.0;
        qkm1 = 1.0;
        ans = 1.0;
        r = 1.0;
        n = 0;
        thresh = 3.0 * MACHEP;
        do
        {

            xk = -(x * k1 * k2) / (k3 * k4);
            pk = pkm1 + pkm2 * xk;
            qk = qkm1 + qkm2 * xk;
            pkm2 = pkm1;
            pkm1 = pk;
            qkm2 = qkm1;
            qkm1 = qk;

            xk = (x * k5 * k6) / (k7 * k8);
            pk = pkm1 + pkm2 * xk;
            qk = qkm1 + qkm2 * xk;
            pkm2 = pkm1;
            pkm1 = pk;
            qkm2 = qkm1;
            qkm1 = qk;

            if (qk != 0)
                r = pk / qk;
            if (r != 0)
            {
                t = fabs((ans - r) / r);
                ans = r;
            }
            else
                t = 1.0;

            if (t < thresh)
                goto cdone;

            k1 += 1.0;
            k2 += 1.0;
            k3 += 2.0;
            k4 += 2.0;
            k5 += 1.0;
            k6 -= 1.0;
            k7 += 2.0;
            k8 += 2.0;

            if ((fabs(qk) + fabs(pk)) > big)
            {
                pkm2 *= biginv;
                pkm1 *= biginv;
                qkm2 *= biginv;
                qkm1 *= biginv;
            }
            if ((fabs(qk) < biginv) || (fabs(pk) < biginv))
            {
                pkm2 *= big;
                pkm1 *= big;
                qkm2 *= big;
                qkm1 *= big;
            }
        } while (++n < 300);

    cdone:
        return (ans);
    }

    /* Continued fraction expansion #2
     * for incomplete beta integral
     */

    double incbd(double a, double b, double x)
    {
        double xk, pk, pkm1, pkm2, qk, qkm1, qkm2;
        double k1, k2, k3, k4, k5, k6, k7, k8;
        double r, t, ans, z, thresh;
        int n;

        k1 = a;
        k2 = b - 1.0;
        k3 = a;
        k4 = a + 1.0;
        k5 = 1.0;
        k6 = a + b;
        k7 = a + 1.0;
        ;
        k8 = a + 2.0;

        pkm2 = 0.0;
        qkm2 = 1.0;
        pkm1 = 1.0;
        qkm1 = 1.0;
        z = x / (1.0 - x);
        ans = 1.0;
        r = 1.0;
        n = 0;
        thresh = 3.0 * MACHEP;
        do
        {

            xk = -(z * k1 * k2) / (k3 * k4);
            pk = pkm1 + pkm2 * xk;
            qk = qkm1 + qkm2 * xk;
            pkm2 = pkm1;
            pkm1 = pk;
            qkm2 = qkm1;
            qkm1 = qk;

            xk = (z * k5 * k6) / (k7 * k8);
            pk = pkm1 + pkm2 * xk;
            qk = qkm1 + qkm2 * xk;
            pkm2 = pkm1;
            pkm1 = pk;
            qkm2 = qkm1;
            qkm1 = qk;

            if (qk != 0)
                r = pk / qk;
            if (r != 0)
            {
                t = fabs((ans - r) / r);
                ans = r;
            }
            else
                t = 1.0;

            if (t < thresh)
                goto cdone;

            k1 += 1.0;
            k2 -= 1.0;
            k3 += 2.0;
            k4 += 2.0;
            k5 += 1.0;
            k6 += 1.0;
            k7 += 2.0;
            k8 += 2.0;

            if ((fabs(qk) + fabs(pk)) > big)
            {
                pkm2 *= biginv;
                pkm1 *= biginv;
                qkm2 *= biginv;
                qkm1 *= biginv;
            }
            if ((fabs(qk) < biginv) || (fabs(pk) < biginv))
            {
                pkm2 *= big;
                pkm1 *= big;
                qkm2 *= big;
                qkm1 *= big;
            }
        } while (++n < 300);
    cdone:
        return (ans);
    }

    /* Power series for incomplete beta integral.
       Use when b*x is small and x not too close to 1.  */

    double pseries(double a, double b, double x)
    {
        double s, t, u, v, n, t1, z, ai;

        ai = 1.0 / a;
        u = (1.0 - b) * x;
        v = u / (a + 1.0);
        t1 = v;
        t = u;
        n = 2.0;
        s = 0.0;
        z = MACHEP * ai;
        while (fabs(v) > z)
        {
            u = (n - b) * x / n;
            t *= u;
            v = t / (a + n);
            s += v;
            n += 1.0;
        }
        s += t1;
        s += ai;

        u = a * log(x);
        if ((a + b) < MAXGAM && fabs(u) < MAXLOG)
        {
            t = tgamma(a + b) / (tgamma(a) * tgamma(b));
            s = s * t * pow(x, a);
        }
        else
        {
            t = lgamma(a + b) - lgamma(a) - lgamma(b) + u + log(s);
            if (t < MINLOG)
                s = 0.0;
            else
                s = exp(t);
        }
        return (s);
    }

    double incbi(double aa, double bb, double yy0)
    {
        double a, b, y0, d, y, x, x0, x1, lgm, yp, di, dithresh, yl, yh, xt;
        int i, rflg, dir, nflg;

        i = 0;
        if (yy0 <= 0)
            return (0.0);
        if (yy0 >= 1.0)
            return (1.0);
        x0 = 0.0;
        yl = 0.0;
        x1 = 1.0;
        yh = 1.0;
        nflg = 0;

        if (aa <= 1.0 || bb <= 1.0)
        {
            dithresh = 1.0e-6;
            rflg = 0;
            a = aa;
            b = bb;
            y0 = yy0;
            x = a / (a + b);
            y = incbet(a, b, x);
            goto ihalve;
        }
        else
        {
            dithresh = 1.0e-4;
        }
        /* approximation to inverse function */

        yp = -ndtri(yy0);

        if (yy0 > 0.5)
        {
            rflg = 1;
            a = bb;
            b = aa;
            y0 = 1.0 - yy0;
            yp = -yp;
        }
        else
        {
            rflg = 0;
            a = aa;
            b = bb;
            y0 = yy0;
        }

        lgm = (yp * yp - 3.0) / 6.0;
        x = 2.0 / (1.0 / (2.0 * a - 1.0) + 1.0 / (2.0 * b - 1.0));
        d = yp * sqrt(x + lgm) / x - (1.0 / (2.0 * b - 1.0) - 1.0 / (2.0 * a - 1.0)) * (lgm + 5.0 / 6.0 - 2.0 / (3.0 * x));
        d = 2.0 * d;
        if (d < MINLOG)
        {
            x = 1.0;
            goto under;
        }
        x = a / (a + b * exp(d));
        y = incbet(a, b, x);
        yp = (y - y0) / y0;
        if (fabs(yp) < 0.2)
            goto newt;

    /* Resort to interval halving if not close enough. */
    ihalve:

        dir = 0;
        di = 0.5;
        for (i = 0; i < 100; i++)
        {
            if (i != 0)
            {
                x = x0 + di * (x1 - x0);
                if (x == 1.0)
                    x = 1.0 - MACHEP;
                if (x == 0.0)
                {
                    di = 0.5;
                    x = x0 + di * (x1 - x0);
                    if (x == 0.0)
                        goto under;
                }
                y = incbet(a, b, x);
                yp = (x1 - x0) / (x1 + x0);
                if (fabs(yp) < dithresh)
                    goto newt;
                yp = (y - y0) / y0;
                if (fabs(yp) < dithresh)
                    goto newt;
            }
            if (y < y0)
            {
                x0 = x;
                yl = y;
                if (dir < 0)
                {
                    dir = 0;
                    di = 0.5;
                }
                else if (dir > 3)
                    di = 1.0 - (1.0 - di) * (1.0 - di);
                else if (dir > 1)
                    di = 0.5 * di + 0.5;
                else
                    di = (y0 - y) / (yh - yl);
                dir += 1;
                if (x0 > 0.75)
                {
                    if (rflg == 1)
                    {
                        rflg = 0;
                        a = aa;
                        b = bb;
                        y0 = yy0;
                    }
                    else
                    {
                        rflg = 1;
                        a = bb;
                        b = aa;
                        y0 = 1.0 - yy0;
                    }
                    x = 1.0 - x;
                    y = incbet(a, b, x);
                    x0 = 0.0;
                    yl = 0.0;
                    x1 = 1.0;
                    yh = 1.0;
                    goto ihalve;
                }
            }
            else
            {
                x1 = x;
                if (rflg == 1 && x1 < MACHEP)
                {
                    x = 0.0;
                    goto done;
                }
                yh = y;
                if (dir > 0)
                {
                    dir = 0;
                    di = 0.5;
                }
                else if (dir < -3)
                    di = di * di;
                else if (dir < -1)
                    di = 0.5 * di;
                else
                    di = (y - y0) / (yh - yl);
                dir -= 1;
            }
        }
        // mtherr("incbi", PLOSS);
        if (x0 >= 1.0)
        {
            x = 1.0 - MACHEP;
            goto done;
        }
        if (x <= 0.0)
        {
        under:
            // mtherr("incbi", UNDERFLOW);
            x = 0.0;
            goto done;
        }

    newt:

        if (nflg)
            goto done;
        nflg = 1;
        lgm = lgamma(a + b) - lgamma(a) - lgamma(b);

        for (i = 0; i < 8; i++)
        {
            /* Compute the function at this point. */
            if (i != 0)
                y = incbet(a, b, x);
            if (y < yl)
            {
                x = x0;
                y = yl;
            }
            else if (y > yh)
            {
                x = x1;
                y = yh;
            }
            else if (y < y0)
            {
                x0 = x;
                yl = y;
            }
            else
            {
                x1 = x;
                yh = y;
            }
            if (x == 1.0 || x == 0.0)
                break;
            /* Compute the derivative of the function at this point. */
            d = (a - 1.0) * log(x) + (b - 1.0) * log(1.0 - x) + lgm;
            if (d < MINLOG)
                goto done;
            if (d > MAXLOG)
                break;
            d = exp(d);
            /* Compute the step to the next approximation of x. */
            d = (y - y0) / d;
            xt = x - d;
            if (xt <= x0)
            {
                y = (x - x0) / (x1 - x0);
                xt = x0 + 0.5 * y * (x - x0);
                if (xt <= 0.0)
                    break;
            }
            if (xt >= x1)
            {
                y = (x1 - x) / (x1 - x0);
                xt = x1 - 0.5 * y * (x1 - x);
                if (xt >= 1.0)
                    break;
            }
            x = xt;
            if (fabs(d / x) < 128.0 * MACHEP)
                goto done;
        }
        /* Did not converge.  */
        dithresh = 256.0 * MACHEP;
        goto ihalve;

    done:

        if (rflg)
        {
            if (x <= MACHEP)
                x = 1.0 - MACHEP;
            else
                x = 1.0 - x;
        }
        return (x);
    }

    const double LS2PI = 0.91893853320467274178; // log( sqrt( 2*pi ) )
    const double LOGPI = 1.14472988584940017414;

#define MAXLGM 2.556348e305

    // inverse of normal distribution

    /* approximation for 0 <= |y - 0.5| <= 3/8 */
    const double P0[5] = {
        -5.99633501014107895267E1,
        9.80010754185999661536E1,
        -5.66762857469070293439E1,
        1.39312609387279679503E1,
        -1.23916583867381258016E0,
    };
    const double Q0[8] = {
        /* 1.00000000000000000000E0,*/
        1.95448858338141759834E0,
        4.67627912898881538453E0,
        8.63602421390890590575E1,
        -2.25462687854119370527E2,
        2.00260212380060660359E2,
        -8.20372256168333339912E1,
        1.59056225126211695515E1,
        -1.18331621121330003142E0,
    };

    /* Approximation for interval z = sqrt(-2 log y ) between 2 and 8
     * i.e., y between exp(-2) = .135 and exp(-32) = 1.27e-14.
     */
    const double P1[9] = {
        4.05544892305962419923E0,
        3.15251094599893866154E1,
        5.71628192246421288162E1,
        4.40805073893200834700E1,
        1.46849561928858024014E1,
        2.18663306850790267539E0,
        -1.40256079171354495875E-1,
        -3.50424626827848203418E-2,
        -8.57456785154685413611E-4,
    };
    const double Q1[8] = {
        /*  1.00000000000000000000E0,*/
        1.57799883256466749731E1,
        4.53907635128879210584E1,
        4.13172038254672030440E1,
        1.50425385692907503408E1,
        2.50464946208309415979E0,
        -1.42182922854787788574E-1,
        -3.80806407691578277194E-2,
        -9.33259480895457427372E-4,
    };

    /* Approximation for interval z = sqrt(-2 log y ) between 8 and 64
     * i.e., y between exp(-32) = 1.27e-14 and exp(-2048) = 3.67e-890.
     */
    const double P2[9] = {
        3.23774891776946035970E0,
        6.91522889068984211695E0,
        3.93881025292474443415E0,
        1.33303460815807542389E0,
        2.01485389549179081538E-1,
        1.23716634817820021358E-2,
        3.01581553508235416007E-4,
        2.65806974686737550832E-6,
        6.23974539184983293730E-9,
    };
    const double Q2[8] = {
        /*  1.00000000000000000000E0,*/
        6.02427039364742014255E0,
        3.67983563856160859403E0,
        1.37702099489081330271E0,
        2.16236993594496635890E-1,
        1.34204006088543189037E-2,
        3.28014464682127739104E-4,
        2.89247864745380683936E-6,
        6.79019408009981274425E-9,
    };
    const double s2pi = 2.50662827463100050242E0;

    double ndtri(double y0)
    {
        assert((0 <= y0) && (y0 <= 1));

        double x, y, z, y2, x0, x1;
        int code = 1;

        y = y0;
        if (y > (1.0 - 0.13533528323661269189)) /* 0.135... = exp(-2) */
        {
            y = 1.0 - y;
            code = 0;
        }

        if (y > 0.13533528323661269189)
        {
            y = y - 0.5;
            y2 = y * y;
            x = y + y * (y2 * polevl(y2, P0, 4) / p1evl(y2, Q0, 8));
            x = x * s2pi;
            return (x);
        }

        x = sqrt(-2.0 * log(y));
        x0 = x - log(x) / x;

        z = 1.0 / x;
        if (x < 8.0) /* y > exp(-32) = 1.2664165549e-14 */
            x1 = z * polevl(z, P1, 8) / p1evl(z, Q1, 8);
        else
            x1 = z * polevl(z, P2, 8) / p1evl(z, Q2, 8);
        x = x0 - x1;
        if (code != 0)
            x = -x;
        return (x);
    }
}