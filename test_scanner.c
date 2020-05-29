#include "scanner.h"

int main() {
    struct scanner *con = load_scanner_from_str(L"defffff");

    struct scanner *scanner = load_scanner_from_file("../fa.c", UTF8);
    while (!scanner_is_end(scanner)) {
        char_t t = scanner_next(scanner);
        if (t == 'a') {
            scanner_merge(scanner, con);
        }
        wprintf(L"%lc", t);
    }
}