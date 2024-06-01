#include "src/TestingFramework.h"

int main(int argc, char* argv[]) {
    {
        Test::TestManager tManager;

        tManager.OnInit();
        while (tManager.WindowIsOpen())
            tManager.OnRun();
        tManager.OnDestroy();
    }
}
