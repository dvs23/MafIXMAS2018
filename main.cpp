#include <iostream>
#include <limits>
#include <bitset>
#include <cmath>

unsigned int countSetBits(unsigned long long n) {
    unsigned int count = 0;

    while(n) {
        n &= (n - 1) ;
        count++;
    }

    return count;
}

int main(int argc, char const* argv[]) {
    std::cout << sizeof(unsigned long long) << std::endl;

    for(int n = 38; n <= 64; ++n) { //dimensions
        std::cout << n << std::endl;

        for(unsigned long long ia = std::pow(2, n - 1); ia < std::pow(2, n) && ia < std::numeric_limits<unsigned long long>::max(); ++ia) { //start at 0, otherwise 0 is result
            //std::bitset<64> a = ia;
            size_t absA = countSetBits(ia);

            for(unsigned long long ib = 1; ib < ia; ++ib) {
                //std::bitset<64> b = ib;

                size_t absB = countSetBits(ib);
                unsigned long long absProd = absA * absB;

                if(absProd < 1443)
                    continue;

                size_t scalarProd = countSetBits(ia & ib);
                unsigned long long res = absProd - scalarProd * scalarProd;

                if(res == 1443)
                    std::cout << ia << " " << ib << " - " << res << std::endl;
            }
        }
    }

    return 0;
}