#include <iostream>
#include <bitset>
#include <fstream>

using namespace std;
int main() {
    unsigned char H_matrix [8]={
            0b10011001,
            0b01111001,
            0b11010110,
            0b11000010,
            0b11000000,
            0b01010100,
            0b10100110,
            0b10101000};
    unsigned short HE_matrix[8];
    unsigned short mask = 0b10000000;
    for(int i=0;i<8;i++)
    {
        HE_matrix[i]=H_matrix[i];
        HE_matrix[i]<<=8u;
        HE_matrix[i]|=mask;
        mask>>=1u;
    }
    ofstream myfile ("example.txt");
    if (myfile.is_open())
    {
        myfile << "This is a line.\n";
        myfile << "This is another line.\n";
        myfile.close();
    }
    else cout << "Unable to open file";
    for(unsigned short i : HE_matrix)
    std::cout << std::bitset<16>(i) << std::endl;
    return 0;
}
