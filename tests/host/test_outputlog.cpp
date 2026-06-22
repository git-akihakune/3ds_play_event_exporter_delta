#include "outputlog.h"

#include <cassert>
#include <string_view>

int main() {
    assert(OutputLogPath() == std::string_view{"/play_events.log"});
}
