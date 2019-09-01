#include <gtest/gtest.h>
#include <fstream>

using namespace std;

TEST(Fstream, normal)
{
    const char *fileName = "/tmp/1.txt";
    fstream fs{ fileName, fs.in | fs.binary | fs.out | fs.trunc};
    if(fs.is_open())
    {
        for (int i = 0; i < (1024*1024*500 / 20); i++)
        {
            fs << "current index is: " << i << endl;
            if(20*1024*1024 / 20 < i)
                fs.flush();
        }
        fs.flush();
        fs.close();
    }
    else 
    {
        FAIL();
    }
}