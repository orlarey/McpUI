#include "McpUI.h"

int main()
{
    McpUI      myUI;
    FAUSTFLOAT level = 0.5f;
    FAUSTFLOAT play  = 0.0f;

    myUI.openVerticalBox("Mixer");
    myUI.openVerticalBox("Channel 3");
    myUI.addHorizontalSlider("Level", &level, 0.0f, 0.0f, 1.0f, 0.01f);
    myUI.addButton("play", &play);
    myUI.closeBox();
    myUI.closeBox();

    myUI.run();

    return 0;
}
