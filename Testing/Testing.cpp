#include "GraphicsDevice.h"
#include "src/TestingFramework.h"

int main(int argc, char* argv[]) {
    {
        Graphics::GraphicsDevice GraphicsDevice = Graphics::GraphicsDevice::Initialize({ 1200, 900 });
        Test::TestManager testm;

        testm.OnInit(GraphicsDevice);
        while (GraphicsDevice.WindowIsOpen()) {
            testm.OnUpdate(GraphicsDevice);

            GraphicsDevice.BeginRender();

            testm.OnRender(GraphicsDevice);
            testm.OnImGuiRender(GraphicsDevice);

            GraphicsDevice.EndRender();
        }
        testm.OnDestroy(GraphicsDevice);
    }
}
