#include <iostream>
#include <bitset>
#include <fstream>
#include <sstream>
using namespace std;
void encode(unsigned char tab[])
{
    ifstream is ("inputMessage.txt", ifstream::binary);
    ofstream os ("codedMessage.txt", ofstream::binary);
    if (is) {
        // get length of file:
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);
        char * buffer = new char [length];
        // read data as a block:
        is.read (buffer,length);
        is.close();
        char * textWithControlBits = new char [length*2];
        unsigned char controlBits;
        unsigned char mask;
        bool checkBit;
        for(int i=0;i<length*2;i=i+2)
        {

            textWithControlBits[i]=buffer[i/2];
            textWithControlBits[i+1]=0;

            for(int j=0;j<8;j++)
            {
                controlBits=textWithControlBits[i]&tab[j];
                mask = 0b10000000;
                checkBit=false;
                for(int k=0;k<8;k++)
                {
                    if((controlBits&mask)!=0) checkBit=!checkBit;
                    mask>>=1u;
                }
                textWithControlBits[i+1]<<=1u;
                if(checkBit)textWithControlBits[i+1]+=1;
            }
        }
        stringstream ss;
        for(int i=0;i<length*2;i=i+2)
        {
            cout<<std::bitset<8>(textWithControlBits[i])<<" "<<std::bitset<8>(textWithControlBits[i+1])<<endl;
        }
        for(int i=0;i<length*2;i++)
        {
            ss<<std::bitset<8>(textWithControlBits[i]);
        }
        os<<ss.str();
        os.close();
        delete[] buffer;

    }
}
void decode(const unsigned short tab[])
{
    ifstream is ("codedMessage.txt", ifstream::binary);
    ofstream os ("decodedMessage.txt", ifstream::binary);
    if (is) {
        // get length of file:
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);
        char * buffer = new char [length];
        // read data as a block:
        is.read (buffer,length);
//trzeba podzielic na 16 bit bloki i zapisac jako un int

        char * output = new char [length];
        is.close();
        unsigned int textWithControlBits[length];
        unsigned short errorVector=0u;
        unsigned short checkVector=0u;
        string textBinary="";
        for(int i=0;i<length;i++)
        {
            textBinary+=buffer[i];
            if((i+1)%16==0)
            {
                textWithControlBits[i/16]=std::bitset<16>(textBinary).to_ulong();
                textBinary="";
            }
        }
        length/=16;
        /*
         * jak dane wejsciowe nie byly binarnie, tylko 2 chary > 1 short
        for(int i=0;i<length;i++)
        {
            textWithControlBits[i]=buffer[i*2];
            textWithControlBits[i]<<=8u;
            // bez tego ucina o 1 bit (9), ktory byc moze odpowiadal za znak prawej czesci
            unsigned char bitsWithoutSign=buffer[i*2+1];
            textWithControlBits[i]+=bitsWithoutSign;
        }
*/
        cout<<endl;
        unsigned short mask,mask2;
        unsigned short findColumn=0u;
        bool isFoundColumn;
        bool checkBit;
        for(int i=0;i<length;i++)
        {
            errorVector=0u;
            isFoundColumn=true;
            for(int j=0;j<8;j++)
            {

                checkVector=textWithControlBits[i]&tab[j];
                mask = 0b1000000000000000;
                checkBit=false;
                for(int k=0;k<16;k++)
                {
                    if((checkVector&mask)!=0) checkBit=!checkBit;
                    mask>>=1u;
                }
                errorVector<<=1u;
                if(checkBit)errorVector+=1;
            }
            if(errorVector==0) {
                output[i]=(textWithControlBits[i]>>8u);
                cout<<"wiadomosc nr "<<i+1<<" ok"<<endl;
            }
            else{
                cout<<"wiadomosc nr "<<i+1<<" error "<<bitset<8>(errorVector)<<endl;
                isFoundColumn=false;
                mask = 0b1000000000000000;
                for(int j=0;j<16;j++ )
                {
                    findColumn=0u;
                    for(int k=0;k<8;k++)
                    {
                        findColumn<<=1u;
                        if((mask&tab[k])!=0)findColumn+=1;
                    }

                    if(findColumn==errorVector) {
                        output[i] = (textWithControlBits[i] ^ mask) >> 8u;
                        cout<<"w wiadomosci o nr "<<i+1<<" poprawiono blad na bicie "<<j+1<<endl;
                        j=16;
                        isFoundColumn=true;
                    }
                    else mask>>=1u;
                }
            }

            if(!isFoundColumn)
            {
                cout<<"2 bledy w wiadomosci nr "<<i+1<<"\n";
                for(int p=0;p<15;p++) {
                    mask  = 0b1000000000000000;
                    mask>>=p;
                    for (int j = p+1; j < 16; j++) {
                        mask2 = 0b1000000000000000;
                        mask2>>=j;
                        findColumn=0u;
                        for (int k = 0; k < 8; k++) {
                            findColumn <<= 1u;
                            if (((mask & tab[k])!=0 && (mask2 & tab[k])==0) ||
                                ((mask & tab[k])==0 && (mask2 & tab[k])!=0))findColumn += 1; //kolumna sumMod2
                        }

                        if(findColumn==errorVector)
                        {
                            cout<<"w wiadomosci o nr "<<i+1<<" poprawiono blad na bicie "<<p+1<<" i "<<j+1<<endl;
                            textWithControlBits[i] ^= mask;
                            textWithControlBits[i] ^= mask2;
                            output[i] = (textWithControlBits[i] >> 8u);
                            j=16;
                            p=16;
                        }
                    }
                }
            }
            cout<<endl;
        }

        os.write(output,length);
        os.close();
        delete [] buffer;
        delete[] output;
    }
}
int main() {
    char choice;
    unsigned char H_matrix [8]={
            0b10110010,
            0b11101001,
            0b01000111,
            0b00000010,
            0b00010001,
            0b00110111,
            0b01011111,
            0b10011111};
    unsigned short HE_matrix[8];
    unsigned short mask = 0b10000000;
    for(int i=0;i<8;i++)
    {
        HE_matrix[i]=H_matrix[i];
        HE_matrix[i]<<=8u;
        HE_matrix[i]|=mask;
        mask>>=1u;
    }
    cout<<"0 - wyjscie\n1 - koduj inputMessage.txt -> codedMessage.txt \n2 - dekoduj codedMessage.txt -> decodedMessage.txt\n";
    do{
        cin>>choice;
        switch(choice)
        {
            case '0': break;
            case '1':{
                encode(H_matrix);
                break;
            }
            case '2':{
                decode(HE_matrix);
                break;
            }
            default: cout<<"Wybierz jeszcze raz\n";
        }
    }while(choice!='0');
    return 0;
}
