Section 3.1: 1: Total samples are = 11 (10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20)
                Favorable events = 6 (10, 12, 14, 16, 18, 20)
                Probability that x is even is : = 6/11

            2: {f(x)} = 1, 5(cx) = 1, (c * 1 + c * 2 + ... +c * 5) = 1, c(1 + 2 + 3 + 4 + 5) = 1, c * 15= 1, c = 1/15

            5: P(x) = (Red balls x)(Blue balls 5 - x) / (Total balls Red balls)
                = (7 x)(3 5 - x) / (x 5)

            6: 15C0 * 0.5^x * 0.5^(15 - 0) + 15C1 * 0.5^x * 0.5^(15 - 1) + 15C2 * 0.5^x * 0.5^(15 - 2) + 
            15C3 * 0.5^x * 0.5^(15 - 3) + 15C4 * 0.5^x * 0.5^(15 - 4) + 15C5 * 0.5^x * 0.5^(15 - 5)

            7: Pr(X ≥ 5) = Pr(Y ≤ 3) = Pr(Y = 0) + Pr(Y = 1) + Pr(Y = 2) + Pr(Y = 3)
            = P(k;p,n) = (n k)p^k(1 - p)^(n - k)

            8: P[n = 0] = (20 0)(0.1)^0(0.9)^20
            P[n = 1] = (20 1)(0.1)^1(0.9)^19
            P[n = 2] = (20 2)(0.1)^2(0.9)^18
            P[n = 3] = (20 3)(0.1)^3(0.9)^17
            P[n > 3] = 1 - P[n = 0] - P[n = 1] - P[n = 2] - P[n = 3]

Section 5.2: 8: P(X ≥ 2 | X ≥ 1) = P(X ≥ 2) / P(X ≥ 1)
                = [1 - P(X < 2)] / [1 - P(X < 1)]
                = [1 - 0.3758] / [1 - 0.1074]
                = 0.6242 / 0.8926
                = 0.6993

Section 5.3: 2: P(X ≥ 3) = 1 - P(X < 3) = 1 - P(X = 0) - P(X = 1) - P(X = 2) = 1 - (5C0 * 10C7 / 15C7) - (5C1 * 10C6 / 15C7) - (5C2 * 10C5 / 15C7)
                = 1 - 0.0186 - 0.1632 - 0.3916 = 0.4266

Section 5.4: 3: The number of defects: 0.4 * 5 = 2
            Σ(e^-2 * 2^x / x!, x, 0, 5) = 0.983436
            P = 1 - 0.983436 = 0.016564