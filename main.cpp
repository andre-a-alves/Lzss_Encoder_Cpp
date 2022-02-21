#include "Lzss.h"

int main() {
    Lzss::encode("sam2.txt", "encoded.txt");
    Lzss::decode("encoded.txt", "decoded.txt");
    return 0;
}
