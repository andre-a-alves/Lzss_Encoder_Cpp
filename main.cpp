#include "Lzss.h"

int main() {
    Lzss::encode("samComplete.txt", "encoded.txt");
    Lzss::decode("encoded.txt", "decoded.txt");
    return 0;
}
