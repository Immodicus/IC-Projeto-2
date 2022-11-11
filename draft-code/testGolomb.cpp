// g++ testGolomb.cpp -o testGolomb

#include "Golomb.cpp"
#include "util.cpp"

void printAsBinary(char byte[]);
void printAsChar(char byte[]);

int main(int argc, char *argv[])
{
     // Get m and n
     int m, n;
     cout << "Insert m: ";
     cin >> m;
     cout << "Insert n: ";
     cin >> n;

     cout << endl
          << "Testing Enconding..." << endl;
     // Initialize Golomb object for enconding a number
     Golomb gb("t.bin", 'e', m);
     // Encode number
     int size = gb.encode(n);
     int size2 = gb.encode(-12);
     int size3 = gb.encode(8);
     int size4 = gb.encode(84);
     // Close stream
     gb.close();

     // BitStream to read result
     BitStream bs("t.bin", 'r');
     char byte[size + 1];
     bs.readNbits(byte, size + 1);
     // Print in Binary
     printAsBinaryMsbtoLsb(byte, size);
     bs.readNbits(byte, size2 + 1);
     printAsBinaryMsbtoLsb(byte, size2);
     bs.readNbits(byte, size3 + 1);
     printAsBinaryMsbtoLsb(byte, size3);
     bs.readNbits(byte, size4 + 1);
     printAsBinaryMsbtoLsb(byte, size4);
     // Close BitStream
     bs.close();

     cout << endl
          << "Testing Decoding..." << endl;
     // Initialize Golomb object for decoding value
     Golomb g("t.bin", 'd', m);
     // Decode number
     cout << "Decoder Value: " << g.decode() << endl;
     cout << "Decoder Value: " << g.decode() << endl;
     cout << "Decoded Value: " << g.decode() << endl;
     cout << "Decoded Value: " << g.decode() << endl;
     // Close stream
     g.close();

     return 0;
}
