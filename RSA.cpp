#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
using namespace std;

long long gcd(long long a, long long b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

long long power(long long x, long long y, long long p) {
    long long res = 1;
    x = x % p;
    while (y > 0) {
        if (y & 1)
            res = (res * x) % p;
        y = y >> 1;
        x = (x * x) % p;
    }
    return res;
}

bool isPrime(long long n) {
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
    if (n % 2 == 0 || n % 3 == 0)
        return false;
    for (long long i = 5; i * i <= n; i = i + 6)
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    return true;
}

long long generatePrime(int bits) {
    long long min = pow(2, bits - 1);
    long long max = pow(2, bits) - 1;
    long long prime;
    while (true) {
        prime = rand() % max + min;
        if (isPrime(prime))
            break;
    }
    return prime;
}

void generateKeys(long long& n, long long& e, long long& d, int bits) {
    srand(time(NULL));
    long long p = generatePrime(bits);
    long long q = generatePrime(bits);
    n = p * q;
    long long phi = (p - 1) * (q - 1);
    do {
        e = rand() % phi + 1;
    } while (gcd(e, phi) != 1);
    for (d = 1; d <= phi; d++) {
        if ((d * e) % phi == 1)
            break;
    }
}

long long encrypt(long long m, long long e, long long n) {
    return power(m, e, n);
}

long long decrypt(long long c, long long d, long long n) {
    return power(c, d, n);
}

int main() {
    long long n, e, d;
    int bits = 16;
    generateKeys(n, e, d, bits);
    cout << "Public key: (" << n << ", " << e << ")" << endl;
    cout << "Private key: (" << n << ", " << d << ")" << endl;
    long long m = 31531;
    long long c = encrypt(m, e, n);
    cout << "Plaintext: " << m << endl;
    cout << "Ciphertext: " << c << endl;
    long long m1 = decrypt(c, d, n);
    cout << "Decrypted plaintext: " << m1 << endl;
    return 0;
}