#include "titlenames.h"

#include <cassert>

int main() {
    TitleNameResolver resolver;
    resolver.byId[0x000400000BAFE000ull] = "Nintendo 3DS Sound";

    assert(ResolveTitleName(resolver, 0x000400000BAFE000ull) == "Nintendo 3DS Sound");
    assert(ResolveTitleName(resolver, 0x0004000000000000ull) == "0004000000000000");
}
