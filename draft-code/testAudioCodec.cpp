// g++ testAudioCodec.cpp -o testAudioCodec -lsndfile -I /usr/local/include/python3.8 -lpython3.8
#include "AudioCodec.cpp"
#include <sndfile.h>
#include <ctime>

int main(int argc, char *argv[])
{

    // Command line arguments processing
    if (argc != 2)
    {
        cout << "\033[1;31mError: Following argument is required <input filename>\033[0m" << endl;
        return 0;
    }
    char *inputfile = argv[1];

    AudioCodec comp(inputfile);

    int op1 = -1;
    while (!(op1 == 0 || op1 == 1))
    {
        cout << "Choose codec option (0 for lossless or 1 for lossy): ";
        cin >> op1;
    }
    int op2 = 0;
    while (op2 < 1 || op2 > 3)
    {
        cout << "Choose the predictor (1, 2 or 3): ";
        cin >> op2;
    }
    int bits = -1;
    if (op1 == 1)
    {
        while (bits < 0 || bits > 15)
        {
            cout << "Choose quantization step (number of bits to remove): ";
            cin >> bits;
        }
    }

    clock_t begin = clock();
    comp.compress("fcompress.bin", op2, op1, bits);
    clock_t end = clock();
    cout << "Duration: " << (double(end - begin) / CLOCKS_PER_SEC) << endl;
    printf("... done encoding\n\n");

    comp.decompress("fcompress.bin");
    printf("... done decompress\n\n");

    return 0;
}
