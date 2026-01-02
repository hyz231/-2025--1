#include "MainUI.h"
#ifdef _WIN32
#include <windows.h>
#endif

namespace {
void configureConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}
} // namespace

int main() {
    configureConsole();
    User user("user001", "记账达人");
    MainUI ui(user);
    ui.run();
    return 0;
}
