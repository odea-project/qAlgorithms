# Analytical Derivation of the Expectation Value for a Piecewise Gaussian-Like Distribution

This document provides a step-by-step analytical derivation of the expectation value (mean) for a piecewise-defined Gaussian-like distribution. The distribution is defined with different quadratic exponents on either side of the apex (mode), allowing for asymmetry. We will derive expressions for both the normalization integral and the expectation value.

## Table of Contents

1. [Definition of the Distribution](#definition-of-the-distribution)
2. [Normalization Integral Z](#normalization-integral-z)
   - [Integral for x < 0](#integral-for-x--0)
   - [Integral for  x >= 0](#integral-for-x--0)
   - [Combined Normalization Integral](#combined-normalization-integral)
3. [Expectation Value mu](#expectation-value-mu)
   - [Relationship Between mu and Z](#relationship-between-mu-and-z)
   - [Derivative of Z with Respect to b_1](#derivative-of-z-with-respect-to-b1)
   - [Final Expression for mu](#final-expression-for-mu)
4. [Summary](#summary)
5. [References](#references)

---

## Definition of the Distribution

We consider a piecewise-defined function $` f(x) `$ that resembles a Gaussian distribution but allows for asymmetry by using different quadratic coefficients on either side of the apex. The function is defined as:

```math
f(x) =
\begin{cases}
\exp(b_0 + b_1 x + b_2 x^2), & x < 0, \\
\exp(b_0 + b_1 x + b_3 x^2), & x \geq 0,
\end{cases}
```

where:
- $` b_0 `$ is a constant ensuring scale.
- $` b_1 `$ controls the location shift.
- $` b_2 `$ and $` b_3 `$ are quadratic coefficients that determine the spread (variance) on the left and right sides, respectively.

**Assumptions:**
- $` b_2 < 0 `$ and $` b_3 < 0 `$ to ensure the function decays to zero as $` x \to \pm\infty `$, resembling a Gaussian distribution.

---

## Normalization Integral $` Z `$

To ensure $` f(x) `$ is a valid probability density function (PDF), it must satisfy the normalization condition:

```math
Z = \int_{-\infty}^{\infty} f(x) \, dx = 1.
```

However, we will first compute $` Z `$ without assuming it equals 1 and later normalize the expectation value accordingly.

### Integral for $` x < 0 `$

For $` x < 0 `$, the integral $` Z_1 `$ is:

```math
Z_1 = \int_{-\infty}^{0} \exp(b_0 + b_1 x + b_2 x^2) \, dx = e^{b_0} \int_{-\infty}^{0} \exp(b_1 x + b_2 x^2) \, dx.
```

**Completing the Square:**

```math
b_1 x + b_2 x^2 = b_2 \left( x^2 + \frac{b_1}{b_2} x \right) = b_2 \left[ \left( x + \frac{b_1}{2b_2} \right)^2 - \frac{b_1^2}{4b_2^2} \right] = b_2 \left( x + \frac{b_1}{2b_2} \right)^2 - \frac{b_1^2}{4b_2}.
```

Substituting back into $` Z_1 `$:

```math
Z_1 = e^{b_0 - \frac{b_1^2}{4b_2}} \int_{-\infty}^{0} \exp\left( b_2 \left( x + \frac{b_1}{2b_2} \right)^2 \right) \, dx.
```

**Change of Variable:**

Let:

```math
u = x + \frac{b_1}{2b_2} \quad \Rightarrow \quad x = u - \frac{b_1}{2b_2}, \quad dx = du.
```

**Updated Limits:**

```math
x \to -\infty \Rightarrow u \to -\infty + \frac{b_1}{2b_2}, \quad x = 0 \Rightarrow u = \frac{b_1}{2b_2}.
```

**Expressing $` Z_1 `$ in Terms of $` u `$:**

```math
Z_1 = e^{b_0 - \frac{b_1^2}{4b_2}} \int_{-\infty + \frac{b_1}{2b_2}}^{\frac{b_1}{2b_2}} \exp(b_2 u^2) \, du.
```

**Using the Error Function ($` \text{erf} `$):**

Since $` b_2 < 0 `$, we can express the integral in terms of the error function:

```math
\int \exp(b_2 u^2) \, du = \sqrt{\frac{\pi}{-b_2}} \cdot \frac{1}{2} \left[ \text{erf}\left( \sqrt{-b_2} \, u \right) \right] + C.
```

Applying the limits:

```math
Z_1 = e^{b_0 - \frac{b_1^2}{4b_2}} \cdot \sqrt{\frac{\pi}{-b_2}} \cdot \frac{1}{2} \left[ \text{erf}\left( \frac{b_1}{2\sqrt{-b_2}} \right) - (-1) \right] = e^{b_0 - \frac{b_1^2}{4b_2}} \cdot \sqrt{\frac{\pi}{-b_2}} \cdot \frac{1}{2} \left[ 1 + \text{erf}\left( \frac{b_1}{2\sqrt{-b_2}} \right) \right].
```

### Integral for $` x \geq 0 `$

For $` x \geq 0 `$, the integral $` Z_2 `$ is:

```math
Z_2 = \int_{0}^{\infty} \exp(b_0 + b_1 x + b_3 x^2) \, dx = e^{b_0} \int_{0}^{\infty} \exp(b_1 x + b_3 x^2) \, dx.
```

**Completing the Square:**

```math
b_1 x + b_3 x^2 = b_3 \left( x^2 + \frac{b_1}{b_3} x \right) = b_3 \left[ \left( x + \frac{b_1}{2b_3} \right)^2 - \frac{b_1^2}{4b_3^2} \right] = b_3 \left( x + \frac{b_1}{2b_3} \right)^2 - \frac{b_1^2}{4b_3}.
```

Substituting back into $` Z_2 `$:

```math
Z_2 = e^{b_0 - \frac{b_1^2}{4b_3}} \int_{0}^{\infty} \exp\left( b_3 \left( x + \frac{b_1}{2b_3} \right)^2 \right) \, dx.
```

**Change of Variable:**

Let:

```math
u = x + \frac{b_1}{2b_3} \quad \Rightarrow \quad x = u - \frac{b_1}{2b_3}, \quad dx = du.
```

**Updated Limits:**

```math
x = 0 \Rightarrow u = \frac{b_1}{2b_3}, \quad x \to \infty \Rightarrow u \to \infty + \frac{b_1}{2b_3}.
```

**Expressing $` Z_2 `$ in Terms of $` u `$:**

```math
Z_2 = e^{b_0 - \frac{b_1^2}{4b_3}} \int_{\frac{b_1}{2b_3}}^{\infty + \frac{b_1}{2b_3}} \exp(b_3 u^2) \, du.
```

**Using the Error Function ($` \text{erf} `$):**

Since $` b_3 < 0 `$, we can express the integral in terms of the error function:

```math
\int \exp(b_3 u^2) \, du = \sqrt{\frac{\pi}{-b_3}} \cdot \frac{1}{2} \left[ \text{erf}\left( \sqrt{-b_3} \, u \right) \right] + C.
```

Applying the limits:

```math
Z_2 = e^{b_0 - \frac{b_1^2}{4b_3}} \cdot \sqrt{\frac{\pi}{-b_3}} \cdot \frac{1}{2} \left[ 1 - \text{erf}\left( \frac{b_1}{2\sqrt{-b_3}} \right) \right].
```

### Combined Normalization Integral

Combining $` Z_1 `$ and $` Z_2 `$, the total normalization integral $` Z `$ is:

```math
Z = Z_1 + Z_2 = e^{b_0 - \frac{b_1^2}{4b_2}} \cdot \sqrt{\frac{\pi}{-b_2}} \cdot \frac{1}{2} \left[ 1 + \text{erf}\left( \frac{b_1}{2\sqrt{-b_2}} \right) \right] + e^{b_0 - \frac{b_1^2}{4b_3}} \cdot \sqrt{\frac{\pi}{-b_3}} \cdot \frac{1}{2} \left[ 1 - \text{erf}\left( \frac{b_1}{2\sqrt{-b_3}} \right) \right].
```

---

## Expectation Value $` \mu `$

The expectation value (mean) $` \mu `$ of the distribution is given by:

```math
\mu = \frac{\int_{-\infty}^{\infty} x f(x) \, dx}{\int_{-\infty}^{\infty} f(x) \, dx} = \frac{\int_{-\infty}^{0} x f(x) \, dx + \int_{0}^{\infty} x f(x) \, dx}{Z}.
```

### Relationship Between $` \mu `$ and $` Z `$

Notice that $` Z `$ depends on $` b_1 `$, and the numerator $` \int x f(x) \, dx `$ can be related to the derivative of $` Z `$ with respect to $` b_1 `$:

```math
\frac{dZ}{db_1} = \int_{-\infty}^{\infty} \frac{\partial f(x)}{\partial b_1} \, dx = \int_{-\infty}^{\infty} x f(x) \, dx.
```

Therefore, the expectation value $` \mu `$ can be expressed as:

```math
\mu = \frac{1}{Z} \frac{dZ}{db_1}.
```

This relationship allows us to compute $` \mu `$ by differentiating the normalization integral $` Z `$ with respect to $` b_1 `$.

### Derivative of $` Z `$ with Respect to $` b_1 `$

Compute $` \frac{dZ}{db_1} `$ by differentiating both $` Z_1 `$ and $` Z_2 `$ with respect to $` b_1 `$:

```math
\frac{dZ}{db_1} = \frac{dZ_1}{db_1} + \frac{dZ_2}{db_1}.
```

#### Derivative of $` Z_1 `$

```math
Z_1 = e^{b_0 - \frac{b_1^2}{4b_2}} \cdot \sqrt{\frac{\pi}{-b_2}} \cdot \frac{1}{2} \left[ 1 + \text{erf}\left( \frac{b_1}{2\sqrt{-b_2}} \right) \right].
```

Differentiating $` Z_1 `$ with respect to $` b_1 `$:

```math
\frac{dZ_1}{db_1} = Z_1 \cdot \left( -\frac{b_1}{2b_2} \right) + e^{b_0 - \frac{b_1^2}{4b_2}} \cdot \sqrt{\frac{\pi}{-b_2}} \cdot \frac{1}{2} \cdot \frac{2}{\sqrt{\pi}} e^{-\left( \frac{b_1}{2\sqrt{-b_2}} \right)^2} \cdot \frac{1}{2\sqrt{-b_2}}.
```

Simplifying:

```math
\frac{dZ_1}{db_1} = -\frac{b_1}{2b_2} Z_1 + \frac{1}{4b_2} e^{b_0 - \frac{b_1^2}{2b_2}}.
```

#### Derivative of $` Z_2 `$

```math
Z_2 = e^{b_0 - \frac{b_1^2}{4b_3}} \cdot \sqrt{\frac{\pi}{-b_3}} \cdot \frac{1}{2} \left[ 1 - \text{erf}\left( \frac{b_1}{2\sqrt{-b_3}} \right) \right].
```

Differentiating $` Z_2 `$ with respect to $` b_1 `$:

```math
\frac{dZ_2}{db_1} = Z_2 \cdot \left( -\frac{b_1}{2b_3} \right) - e^{b_0 - \frac{b_1^2}{4b_3}} \cdot \sqrt{\frac{\pi}{-b_3}} \cdot \frac{1}{2} \cdot \frac{2}{\sqrt{\pi}} e^{-\left( \frac{b_1}{2\sqrt{-b_3}} \right)^2} \cdot \frac{1}{2\sqrt{-b_3}}.
```

Simplifying:

```math
\frac{dZ_2}{db_1} = -\frac{b_1}{2b_3} Z_2 - \frac{1}{4b_3} e^{b_0 - \frac{b_1^2}{2b_3}}.
```

### Final Expression for $` \mu `$

Combining the derivatives:

```math
\frac{dZ}{db_1} = \frac{dZ_1}{db_1} + \frac{dZ_2}{db_1} = -\frac{b_1}{2b_2} Z_1 - \frac{b_1}{2b_3} Z_2 + \frac{1}{4b_2} e^{b_0 - \frac{b_1^2}{2b_2}} - \frac{1}{4b_3} e^{b_0 - \frac{b_1^2}{2b_3}}.
```

Therefore, the expectation value $` \mu `$ is:

```math
\mu = \frac{1}{Z} \left( -\frac{b_1}{2b_2} Z_1 - \frac{b_1}{2b_3} Z_2 + \frac{1}{4b_2} e^{b_0 - \frac{b_1^2}{2b_2}} - \frac{1}{4b_3} e^{b_0 - \frac{b_1^2}{2b_3}} \right).
```

**Simplifying Further:**

Factor out common terms where possible:

```math
\mu = -\frac{b_1}{2} \left( \frac{Z_1}{b_2} + \frac{Z_2}{b_3} \right) + \frac{1}{4} \left( \frac{e^{b_0 - \frac{b_1^2}{2b_2}}}{b_2} - \frac{e^{b_0 - \frac{b_1^2}{2b_3}}}{b_3} \right).
```

This expression shows that $` \mu `$ is directly related to both the normalization integral $` Z `$ and its derivative with respect to $` b_1 `$.

---

## Summary

- **Normalization Integral ($` Z `$)**: Computed by splitting the integral into two regions ($` x < 0 `$ and $` x \geq 0 `$), completing the square, and expressing the result in terms of the error function ($` \text{erf} `$).

- **Expectation Value ($` \mu `$)**: Determined by differentiating the normalization integral $` Z `$ with respect to $` b_1 `$ and normalizing by $` Z `$. The final expression incorporates both $` Z `$ and its derivative, reflecting the influence of the distribution's parameters on the mean.

- **Asymmetry Handling**: The piecewise definition with different quadratic coefficients allows modeling asymmetry, which is reflected in the expectation value through the separate contributions from each side of the distribution.

---

## References

- [Error Function ($` \text{erf} `$)](https://en.wikipedia.org/wiki/Error_function)
- [Completing the Square](https://en.wikipedia.org/wiki/Completing_the_square)
- [Two-Piece Normal Distribution](https://en.wikipedia.org/wiki/Split_normal_distribution)

