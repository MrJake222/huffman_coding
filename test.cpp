#include <iostream>
#include <gtest/gtest.h>

#include "libs/bitarray.h"

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    
    /*bitarr::BitArray<uint8_t> ba("1000 1000 0");
    cout << ba << endl;*/
}

