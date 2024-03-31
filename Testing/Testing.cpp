#include "GraphicsDevice.h"
#include "src/TestingFramework.h"

int main(int argc, char* argv[]) {
    Maths::mat4x4 persp = Maths::mat4x4::perspective_fov(90.0f, 4.0f / 3.0f, 0.01f, 100.0f);
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
