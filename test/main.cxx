#include "tests.h"

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: test_corsika path_to_test_data\n");
        return 1;
    }
    const char* dir = argv[1];
    
    test_low_high(dir);
    test_file(dir);
    test_rawstream(dir);
    test_index();
    printf("All Tests Were Successfull!\n");
}
