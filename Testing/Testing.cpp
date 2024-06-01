#include "src/TestingFramework.h"

int main(int argc, char* argv[]) {
    Debug::Info("Execution Directory: {}", argv[0]);
    Debug::Info("Project Directory: {}", Test::PROJECT_DIRECTORY);
    {
        Test::TestManager tManager;

        tManager.OnInit();
        while (tManager.WindowIsOpen())
            tManager.OnRun();
        tManager.OnDestroy();
    }
}
