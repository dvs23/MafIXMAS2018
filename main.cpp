#include <iostream>
#include <limits>
#include <bitset.h>
#include <cmath>
#include <thread>

const unsigned int threadNum = 8;

void nextNum(Bitset& bnum, unsigned int ndim) {
    if(ndim > 1444)
        throw std::runtime_error("Dimension too high!");

    int lastZero = -1;
    int lastOne = -1;

    while(lastZero + 1 < ndim && !bnum.Get(lastZero + 1))
        ++lastZero;

    lastOne = lastZero;

    while(lastOne + 1 < ndim && bnum.Get(lastOne + 1))
        ++lastOne;

    if(lastOne == -1 || lastOne + 1 >= ndim)
        throw std::runtime_error("Overflow or no bits set!");

    int numOnesToShift = (lastZero == -1) ? lastOne : lastOne - lastZero - 1;

    for(int i = 0; i < lastOne; ++i) {
        if(i < numOnesToShift)
            bnum.Set(i, true);
        else
            bnum.Set(i, false);
    }

    bnum.Set(lastOne, false);
    bnum.Set(lastOne + 1, true);
}

void setOnes(unsigned int numOnes, Bitset& bs) {
    bs.Clear();

    for(unsigned int i = 0; i < numOnes; ++i)
        bs.Set(i);
}

void findSolution(unsigned int threadID) {
    for(unsigned int n = 38; n <= 1444; ++n) { //dimensions
        std::cout << n << std::endl;

        unsigned int best = 0;

        // all combinations of numbers of set bits in a and b which are multiplied >= 1443
        for(unsigned int onesInA = 38; onesInA <= n; ++onesInA) {//38 = notw. min. Dimension
            std::cout << "onesInA: " << onesInA << std::endl;
            Bitset a(n);
            setOnes(onesInA, a);//"smallest" vector with onesInA bits is with all bits set on the right

            try {
                for(unsigned int i = 0; i < threadID; ++i)//start with different bitsets -> after that every threadNum-th
                    nextNum(a, n);
            }
            catch(std::runtime_error& ex) {
                continue;
            }


            //we don't have to go through all possible vectors with onesInA ones if we can't find an appropriate onesInB
            bool foundPossibleOnesInB = false;

            while(true) {//go through all a vectors with onesInA ones
                //std::cout << a.to_string() << "\n";

                //make the "which are multiplied >= 1443" part sure with choosing an appropriate onesInB
                for(unsigned int onesInB = 1443 / onesInA + (1443 % onesInA == 0 ? 0 : 1); onesInB <= n; ++onesInB) {
                    if(onesInA * onesInB - (onesInA + onesInB - n) * (onesInA + onesInB - n) < 1443)
                        continue;

                    //std::cout << "onesInB: " << onesInB << std::endl;


                    foundPossibleOnesInB = true;//if we don't find an appropriate onesInB, this number of onesInA won't work in any combination -> break

                    Bitset b(n);
                    setOnes(onesInB, b);

                    try {
                        for(unsigned int i = 0; i < threadID; ++i)//start with different bitsets -> after that every threadNum-th
                            nextNum(b, n);
                    }
                    catch(std::runtime_error& ex) {
                        continue;
                    }


                    while(true) {
                        //std::cout << b.to_string() << "\n";
                        unsigned int absProd = onesInA * onesInB;

                        unsigned int scalarProd = a.CountAnd(b);
                        unsigned int res = absProd - scalarProd * scalarProd;

                        if(res < 1444)
                            best = std::max(best, res);


                        if(res == 1443) {
                            std::cout << "N: " << n << std::endl
                                      << "a: " << a.to_string() << std::endl//only show relevant part of bitset
                                      << "b: " << b.to_string() << std::endl;
                            //return;
                        }


                        try {
                            for(unsigned int i = 0; i < threadNum; ++i)//only calc every threadNum-th calculation
                                nextNum(b, n);
                        }
                        catch(std::runtime_error& ex) {
                            break;
                        }
                    }
                }

                if(!foundPossibleOnesInB)
                    break;

                try {
                    for(unsigned int i = 0; i < threadNum; ++i)
                        nextNum(a, n);
                }
                catch(std::runtime_error& ex) {
                    break;
                }
            }

            if(best > 0)
                std::cout << "Best in " << n << " with |a|=" << onesInA << ": " << best << std::endl;

            /*for(unsigned long long ib = 1; ib < ia; ++ib) {
                //std::bitset<64> b = ib;

                size_t absB = countSetBits(ib);
                unsigned long long absProd = absA * absB;

                if(absProd < 1443)
                    continue;

                size_t scalarProd = countSetBits(ia & ib);
                unsigned long long res = absProd - scalarProd * scalarProd;

                if(res == 1443)
                    std::cout << ia << " " << ib << " - " << res << std::endl;
            }*/
        }
    }
}

int main(int argc, char const* argv[]) {
    std::vector<std::thread> thrs;

    for(unsigned int i = 0; i < threadNum; ++i) {
        thrs.push_back(std::thread([ = ]() {
            findSolution(i);
        }));
    }

    for(unsigned int i = 0; i < threadNum; ++i)
        thrs[i].join();

    /*std::cout << std::bitset<8>(1) << " -> " << std::bitset<8>(nextNum(1, 8)) << std::endl;
    std::cout << std::bitset<8>(3) << " -> " << std::bitset<8>(nextNum(3, 8)) << std::endl;
    std::cout << std::bitset<8>(6) << " -> " << std::bitset<8>(nextNum(6, 8)) << std::endl;
    std::cout << std::bitset<8>(11) << " -> " << std::bitset<8>(nextNum(11, 8)) << std::endl;
    std::cout << std::bitset<8>(23) << " -> " << std::bitset<8>(nextNum(23, 8)) << std::endl;
    std::cout << std::bitset<8>(24) << " -> " << std::bitset<8>(nextNum(24, 8)) << std::endl;
    std::cout << std::bitset<8>(44) << " -> " << std::bitset<8>(nextNum(44, 8)) << std::endl;
    std::cout << std::bitset<8>(46) << " -> " << std::bitset<8>(nextNum(46, 8)) << std::endl;

    try {
        std::cout << std::bitset<8>(0) << " -> " << std::bitset<8>(nextNum(0, 8)) << std::endl;
    }
    catch(std::runtime_error& ex) {
        std::cout << ex.what() << std::endl;
    }

    try {
        std::cout << std::bitset<8>(255) << " -> " << std::bitset<8>(nextNum(255, 8)) << std::endl;
    }
    catch(std::runtime_error& ex) {
        std::cout << ex.what() << std::endl;
    }*/

    /*unsigned long long last = std::pow(2, 38) - 1;
    unsigned long long n = 0;

    while(true) {
        try {
            if((n++) % 10000000 == 0)
                std::cout << std::bitset<64>(last) << " -> ";

            last = nextNum(last, 64);

            if((n - 1) % 10000000 == 0)
                std::cout << std::bitset<64>(last) << std::endl;
        }
        catch(std::runtime_error& ex) {
            std::cout << ex.what() << std::endl;
            break;
        }
    }*/
    /*for(int n = 1; n <= 1444; ++n) {
        std::cout << n << std::endl;

        for(unsigned long long onesInA = 38; onesInA <= n; ++onesInA) {
            for(unsigned long long onesInB = 1443 / onesInA + (1443 % onesInA == 0 ? 0 : 1); onesInB <= n; ++onesInB) {
                unsigned int max = onesInA * onesInB - (onesInA + onesInB - n) * (onesInA + onesInB - n);

                if(max >= 1443) {
                    std::cout << "A: " << onesInA << " B: " << onesInB << " - " << max << std::endl;
                    return 0;
                }
            }
        }
    }*/

    /*Bitset test(66);
    std::vector<int> v;
    std::string bad = "000000000000000000000001111111111111111111111111111111111111111010";

    for(size_t i = 0; i < 66; i++) {
        if(bad[65 - i] == '1')
            v.push_back(i);
    }

    test.Set(v);

    nextNum(test, 66);

    std::cout<<test.to_string()<<std::endl;*/

    /*std::vector<int> aVec;

    for(int i = 0; i < 66; ++i) {
        if(i >= 25)
            aVec.push_back(i);
    }

    std::vector<int> bVec;

    for(int i = 0; i < 66; ++i) {
        if(i < 25 || i >= 47)
            bVec.push_back(i);
    }*/



    return 0;
}