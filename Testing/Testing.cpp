#include "src/TestingFramework.h"

// static Quasi::usize allocSize = 0;
// void* operator new(Quasi::usize size) {
//     allocSize += size;
//     return malloc(size);
// }
//
// void operator delete(void* dat, Quasi::usize size) {
//     allocSize -= size;
//     return free(dat);
// }

int main(int argc, char* argv[]) {
    Quasi::Debug::Info("Execution Directory: {}", argv[0]);
    Quasi::Debug::Info("Project Directory: {}", Test::PROJECT_DIRECTORY);

    Test::TestManager tManager;
    tManager.OnInit();
    while (tManager.WindowIsOpen())
        tManager.OnRun();
    tManager.OnDestroy();
}
