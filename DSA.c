#include <string.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char M[64];
unsigned long H_VAL[5];
unsigned long ecg[2];
unsigned long P, Q, G, x_key, y_key, H;
unsigned long s_R, r_R;

// 消息扩展
void creat_w(unsigned char input[64], unsigned long w[80])
{
    int i, j;
    unsigned long temp, temp1;
    for (i = 0; i < 16; i++)
    {
        j = 4 * i;
        w[i] = ((long)input[j]) << 24 | ((long)input[1 + j]) << 16 | ((long)input[2 + j]) << 8 | ((long)input[3 + j]) << 0;
    }
    for (i = 16; i < 80; i++)
    {
        w[i] = w[i - 16] ^ w[i - 14] ^ w[i - 8] ^ w[i - 3];
        temp = w[i] << 1;
        temp1 = w[i] >> 31;
        w[i] = temp | temp1;
    }
}
// 消息填充-填充明文长度对应的二进制
void ms_len(long a, char intput[64])
{
    unsigned long temp3, p1;
    int i, j, k;
    temp3 = 0;
    p1 = ~(~temp3 << 8);
    for (i = 0; i < 64; i++)
    {
        printf("%d\t", intput[i]);
    }
    for (i = 0; i < 4; i++)
    {
        j = 8 * i;
        intput[63 - i] = (char)((a & (p1 << j)) >> j);
    }
    printf("\n");
    for (i = 0; i < 64; i++)
    {
        printf("%d\t", intput[i]);
    }
}
void hash()
{
    unsigned long H0 = 0x67452301, H1 = 0xefcdab89, H2 = 0x98badcfe, H3 = 0x10325476, H4 = 0xc3d2e1f0;
    unsigned long A, B, C, D, E, temp, temp1, temp2, temp3, k, f;
    int i, flag;
    unsigned long w[80];
    long x;
    int n = strlen(M);
    if (n < 57)
    {
        x = n * 8;
        ms_len(x, M);
        if (n == 56)
            for (i = n; i < 60; i++)
                M[i] = 0;
        else
        {
            M[n] = 128; // 明文末尾补1
            for (i = n + 1; i < 60; i++)
                M[i] = 0;
        }
    }
    printf("\n");
    for (i = 0; i < 64; i++)
    {
        printf("%d\t", M[i]);
    }
    creat_w(M, w);
    printf("\n");
    A = H0;
    B = H1;
    C = H2;
    D = H3;
    E = H4;
    for (i = 0; i < 80; i++)
    {
        flag = i / 20;
        switch (flag)
        {
        case 0:
            k = 0x5a827999;
            f = (B & C) | (~B & D);
            break;
        case 1:
            k = 0x6ed9eba1;
            f = B ^ C ^ D;
            break;
        case 2:
            k = 0x8f1bbcdc;
            f = (B & C) | (B & D) | (C & D);
            break;
        case 3:
            k = 0xca62c1d6;
            f = B ^ C ^ D;
            break;
        }

        temp1 = A << 5;
        temp2 = A >> 27;
        temp3 = temp1 | temp2;
        temp = temp3 + f + E + w[i] + k;
        E = D;
        D = C;

        temp1 = B << 30;
        temp2 = B >> 2;
        C = temp1 | temp2;
        B = A;
        A = temp;
    }
    H0 = H0 + A;
    H1 = H1 + B;
    H2 = H2 + C;
    H3 = H3 + D;
    H4 = H4 + E;
    printf("\n明文对应的杂凑码:\n");
    printf("%lx,%lx,%lx,%lx,%lx", H0, H1, H2, H3, H4);
    H_VAL[0] = H0;
    H_VAL[1] = H1;
    H_VAL[2] = H2;
    H_VAL[3] = H3;
    H_VAL[4] = H4;
}

int calculate(int g, int x, int p)
{
    int k = 1;
    for (int i = 0; i < x; i++)
    {
        k = (k * g) % p;
    }
    return k;
}
void exGcd(unsigned long a, int b)
{
    if (b == 0)
    {
        ecg[0] = 1;
        ecg[1] = 0;
    }
    else
    {
        exGcd(b, a % b);
        unsigned long x = ecg[0];
        ecg[0] = ecg[1];
        ecg[1] = x - (a / b) * ecg[1];
    }
}
unsigned long myPow(int a, unsigned long b, int m)
{
    unsigned long res = 1;
    a %= m;
    while (b != 0)
    {
        if ((b & 1) == 1)
            res = (res * a) % m;
        a = (a * a) % m;
        b >>= 1;
    }
    return res;
}
int sha_convert(unsigned long he)
{
    he >>= 8;
    int re = 0;
    int k = 16;
    int n = 1;

    while (he != 0)
    {
        re += (he % 10) * n;
        he /= 10;
        n *= k;
    }
    printf("\n用于签名的杂凑码：%d", re);
}
void DSA()
{
    printf("\n请输入P,Q,H\n");
    scanf("%d%d%d", &P, &Q, &H);
    int G_TMP = 1;
    for (int i = 0; i < (P - 1) / Q; i++)
    {
        G_TMP = (G_TMP * H) % P;
    }
    G = G_TMP;
    srand(time(NULL));
    x_key = rand() % Q;
    y_key = calculate(G, x_key, P);
    printf("P:%d,Q:%d,G:%d,x:%d,y:%d", P, Q, G_TMP, x_key, y_key);
}
void encrypt(unsigned long m) // 加密函数
{
    srand(time(NULL));
    int k = rand() % Q;

    r_R = calculate(G, k, P) % Q;
    exGcd(k, Q);
    k = ecg[0];

    // int m=13;
    s_R = (m + x_key * r_R) * k % Q; //	s_R=(m+x_key*r_R)*k%Q;

    printf("\n签名为(%d,%d)\n", r_R, s_R);
}
void decrypt(unsigned long m) // 加密函数
{
    printf("%lx", m);

    exGcd(s_R, Q);
    int w = ecg[0];
    if (w < 0)
        w += (Q);
    unsigned long u1 = (m * w) % Q;
    unsigned long u2 = r_R * w % Q;
    int v = myPow(G, u1, P) * myPow(y_key, u2, P) % P % Q;
    printf("(w,u1,u2,v)=(%d,%d,%d,%d)\n", w, u1, u2, v);
    if (v == r_R)
    {
        printf("签名有效");
    }
    else
    {
        printf("签名无效");
    }
}
int main()
{
    printf("输入明文：\n");
    gets(M);
    hash();
    DSA();
    int mm = sha_convert(H_VAL[0]);
    encrypt(mm);
    decrypt(mm);
    return 0;
}
