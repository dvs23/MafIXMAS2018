#include <iostream>
#include <limits>
#include <bitset.h>
#include <cmath>
#include <thread>
#include <atomic>

unsigned int threadNum = 8;
std::chrono::time_point<std::chrono::system_clock> start;

std::atomic<unsigned long long> total(0);

void nextNum(Bitset& bnum, unsigned int ndim) {
    if(ndim > 1444 //we don't need to go higher than 1444
            || bnum.Size() < ndim)//num of bits <= dims?
        throw std::runtime_error("Dimension too high!");

    int lastZero = -1;//-1 stays if no one/zero found
    int lastOne = -1;

    //if next position is still valid
    while(lastZero + 1 < ndim
            && !bnum.Get(lastZero + 1)) //and is a zero
        ++lastZero;

    //start searching for a one right after the last zero
    lastOne = lastZero;

    //protect from consecutive ones
    while(lastOne + 1 < ndim //if next pos still valid
            && bnum.Get(lastOne + 1)) //and is a one
        ++lastOne;

    if(lastOne == -1//if there is no one next to a zero
            || lastOne + 1 >= ndim)//or one is at the end
        throw std::runtime_error("Overflow!");

    //how many ones to shift to the end
    int numOnesToShift = lastOne;

    //if there were zeros before the found one
    if(lastZero != -1)//subtract them from num ones
        numOnesToShift =  lastOne - lastZero - 1;

    for(int i = 0; i < lastOne; ++i) {
        //if: set all ones at the beginning
        //else: all zeros between the last one
        //and the beginning 1es
        if(i < numOnesToShift)
            bnum.Set(i, true);
        else
            bnum.Set(i, false);
    }

    bnum.Set(lastOne, false);//change last one
    bnum.Set(lastOne + 1, true);//with zero next to it
}

/*
//ones = Anzahl Einsen
//ndim = Anzahl Dimensionen
for(int i = 0; i < ndim - ones; i++) {
    for(int j = i + 1; j < ndim - (ones - 1); j++) {
        ...

        for(int m = l + 1; m < ndim - 2; m++) {
            for(int n = m + 1; n < ndim - 1; n++) {
                //commands
            }
        }
    }
}
 */
void nextNum(std::vector<int>& ones, unsigned int ndim) {
    if(ndim == 0 || ones.size() == 0)
        throw std::runtime_error("Invalid parameters!");
    else if(ones[0] >= ndim - ones.size())
        throw std::runtime_error("Maximum reached!");

    //always start in most inner "loop"
    unsigned int loopPointer = ones.size() - 1;
    //reset of all inner variables required?
    bool triggerReset = false;

    //break if outest loop has reached the maximum
    while(loopPointer > 0
            || ones[0] <= ndim - ones.size()) {
        //default operation: increase index in curr loop
        ++ones[loopPointer];

        //if limit is not reached yet
        if(ones[loopPointer]
                <= ndim - (ones.size() - loopPointer))
            break;//nothing else to do, loop still valid
        else {//go up until you find a still valid loop
            //as soon as we found one, we need to
            //set all inner loops to their start value
            triggerReset = true;

            //only decrease if not already in outest loop
            if(loopPointer > 0)
                --loopPointer;
        }
    }

    //if reset triggered and the max value has not
    //been reached set all following bits back ascending
    if(triggerReset && ones[0] <= ndim - ones.size()) {
        //invalid values should be impossible
        //due to the limits of each loop
        for(unsigned int tLoop = loopPointer;
                tLoop < ones.size() - 1;
                ++tLoop)
            ones[tLoop + 1] = ones[tLoop] + 1;
    }
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
            /*std::vector<int> aVec(onesInA);

            for(int i = 0; i < aVec.size(); i++) //{0,1,2,...
                aVec[i] = i;*/


            try {
                for(unsigned int i = 0; i < threadID; ++i) //start with different bitsets -> after that every threadNum-th
                    nextNum(a, n);
            }
            catch(std::runtime_error& ex) {
                continue;
            }

            //a.Clear();
            //a.Set(aVec);


            //we don't have to go through all possible vectors with onesInA ones if we can't find an appropriate onesInB
            bool foundPossibleOnesInB = false;

            while(true) {//go through all a vectors with onesInA ones

                unsigned int roundUp = 1443 % onesInA == 0 ? 0 : 1;

                //make the "which are multiplied >= 1443" part sure with choosing an appropriate onesInB
                for(unsigned int onesInB = 1443 / onesInA + roundUp;
                        onesInB <= n;
                        ++onesInB) {
                    if(onesInA * onesInB - (onesInA + onesInB - n) * (onesInA + onesInB - n) < 1443)
                        continue;

                    //std::cout << "onesInB: " << onesInB << std::endl;


                    foundPossibleOnesInB = true;//if we don't find an appropriate onesInB, this number of onesInA won't work in any combination -> break

                    Bitset b(n);
                    setOnes(onesInB, b);
                    /*std::vector<int> bVec(onesInA);

                    for(int i = 0; i < bVec.size(); i++) //{0,1,2,...
                        bVec[i] = i;

                    b.Clear();
                    b.Set(bVec);*/

                    while(true) {
                        //std::cout << b.to_string() << "\n";
                        unsigned int absProd = onesInA * onesInB;

                        unsigned int scalarProd = a.CountAnd(b);
                        unsigned int res = absProd - scalarProd * scalarProd;

                        unsigned long long t = total.fetch_add(1);

                        if(t % 1000000000 == 0) {
                            std::cout << "processed: " << t << std::endl
                                      << "secs: " << std::chrono::duration<double>(std::chrono::system_clock::now() - start).count() << std::endl
                                      << "res: " << res << std::endl
                                      << "a: " << a.to_string() << std::endl//only show relevant part of bitset
                                      << "b: " << b.to_string() << std::endl;
                        }

                        if(res < 1444)
                            best = std::max(best, res);


                        if(res == 1443) {
                            std::cout << "N: " << n << std::endl
                                      << "a: " << a.to_string() << std::endl//only show relevant part of bitset
                                      << "b: " << b.to_string() << std::endl;
                            //return;
                        }


                        try {
                            nextNum(b, n);
                            //b.Set(bVec);
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

                    //a.Clear();
                    //a.Set(aVec);

                }
                catch(std::runtime_error& ex) {
                    break;
                }
            }

            if(best > 0)
                std::cout << "Best in " << n << " with |a|=" << onesInA << ": " << best << std::endl;
        }
    }
}

void findFirstMax() {
    for(int n = 1; n <= 1444; ++n) {//dimensions
        //starting with 38 would work, too
        // -> others break because >n
        for(unsigned int onesInA = 1;
                onesInA <= n;
                ++onesInA) {
            //1 if we need to round up
            //because there is a remaining
            unsigned int roundUp = 1;

            //true if there is no remaining
            if(1443 % onesInA == 0)
                roundUp = 1;

            unsigned int newStart = 1443
                                    / onesInA
                                    + roundUp;

            for(unsigned int onesInB = newStart;
                    onesInB <= n;
                    ++onesInB) {
                unsigned int scalMin = onesInA
                                       + onesInB
                                       - n;
                unsigned int max = onesInA * onesInB
                                   - scalMin * scalMin;

                if(max >= 1443) {
                    std::cout << "N: " << n
                              << " A: " << onesInA
                              << " B: " << onesInB
                              << " - " << max
                              << std::endl;
                    return;
                }
            }
        }
    }
}

void nextNumBenchmarkA(unsigned int ndim, unsigned int ones) {
    Bitset a(ndim);
    std::vector<int> aVec(ones);

    for(int i = 0; i < aVec.size(); i++) //{0,1,2,...
        aVec[i] = i;


    auto st = std::chrono::system_clock::now();

    try {
        while(true) {
            nextNum(aVec, ndim);
            a.Clear();//ggf. zum Testen ausschalten
            a.Set(aVec);//ggf. zum Testen ausschalten
        }
    }
    catch(std::runtime_error& ex) {
        std::cout << "A took secs: " << std::chrono::duration<double>(std::chrono::system_clock::now() - st).count() << std::endl;
        return;
    }
}

void nextNumBenchmarkB(unsigned int ndim, unsigned int ones) {
    Bitset b(ndim);
    setOnes(ones, b);

    auto st = std::chrono::system_clock::now();

    try {
        while(true)
            nextNum(b, ndim);
    }
    catch(std::runtime_error& ex) {
        std::cout << "B took secs: " << std::chrono::duration<double>(std::chrono::system_clock::now() - st).count() << std::endl;
        return;
    }
}

void nextNumTest() {
    std::cout << "6.2: " << std::endl;
    unsigned int num = 0;
    std::vector<int> v = {0, 1, 2};
    Bitset test(5);
    test.Clear();
    test.Set(v);
    std::cout << num++ << ": " << test.to_string() << std::endl;

    while(true) {
        try {
            nextNum(v, 5);
            test.Clear();
            test.Set(v);
            std::cout << num++ << ": " << test.to_string() << std::endl;
        }
        catch(std::runtime_error& ex) {
            std::cout << ex.what() << std::endl;
            break;
        }
    }

    std::cout << "6.1:" << std::endl;

    num = 0;
    std::vector<int> v2 = {0, 1, 2};
    test.Clear();
    test.Set(v2);
    std::cout << num++ << ": " << test.to_string() << std::endl;

    while(true) {
        try {
            nextNum(test, 5);
            std::cout << num++ << ": " << test.to_string() << std::endl;
        }
        catch(std::runtime_error& ex) {
            std::cout << ex.what() << std::endl;
            break;
        }
    }
}

int main(int argc, char const* argv[]) {
    start = std::chrono::system_clock::now();

    if(argc > 1)
        threadNum = std::atoi(argv[1]);

    std::cout << "First possibly working max found at:" << std::endl;
    findFirstMax();

    std::vector<std::thread> thrs;

    for(unsigned int i = 0; i < threadNum; ++i) {
        thrs.push_back(std::thread([ = ]() {
            findSolution(i);
        }));
    }

    for(unsigned int i = 0; i < threadNum; ++i)
        thrs[i].join();

    return 0;
}