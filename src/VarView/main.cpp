#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <sys/timers.h>
#include <cstdint>

int main() {
    uint8_t loadHandle;
    int loadData[] = { 0, 0, 5, 60 };
    loadHandle = ti_Open("FakeSnake_save", "r");

    if (loadHandle != 0) {
        ti_Read(loadData, sizeof(int), sizeof(loadData) / sizeof(int), loadHandle);
    }

    ti_Close(loadHandle);
    kb_key_t key;
	gfx_Begin();

    for (uint8_t i = 0; i < (sizeof(loadData) / sizeof(int)); i++) {
        gfx_SetTextXY(1, 10 * i);
        gfx_PrintInt(i, 1);
        gfx_SetTextXY(20, 10 * i);
        gfx_PrintInt(loadData[i], 1);
    }

    bool exit = false;
    int dataElement = 0;

    do {
        kb_Scan();

        key = kb_Data[3];

        if (key == kb_0) {
            exit = true;
        }
        else if (key == kb_1) {
            exit = true;
            dataElement = 1;
        }

        key = kb_Data[4];

        if (key == kb_2) {
            exit = true;
            dataElement = 2;
        }

        key = kb_Data[5];

        if (key == kb_3) {
            exit = true;
            dataElement = 3;
        }

    } while (!exit);

    exit = false;

    gfx_FillScreen(0xFF);

    gfx_SetTextXY(1, 1);
    gfx_PrintInt(loadData[dataElement], 1);

    gfx_PrintStringXY("Y=:+1", 1, 50);
    gfx_PrintStringXY("Window:+5", 1, 60);
    gfx_PrintStringXY("Zoom:+10", 1, 70);
    gfx_PrintStringXY("Trace:+50", 1, 80);
    gfx_PrintStringXY("Graph:+100", 1, 90);
    gfx_PrintStringXY("2nd:Exit", 1, GFX_LCD_HEIGHT - 10);

    

    do {

        kb_Scan();

        key = kb_Data[1];

        switch (key)
        {
        case kb_Yequ:
            loadData[dataElement] -= 10;
            gfx_FillRectangle(1, 1, 10, 20);
            break;

        case kb_Window:
            loadData[dataElement]--;
            break;

        case kb_Zoom:
            loadData[dataElement]++;
            break;

        case kb_Trace:
            loadData[dataElement] += 10;
            break;

        case kb_Graph:
            loadData[dataElement] += 50;
            break;

        case kb_2nd:
            exit = true;
            break;

        default:
            break;
        }

            gfx_SetColor(0xFF);
            gfx_FillRectangle(1, 1, GFX_LCD_WIDTH, 10);

            gfx_SetTextXY(1, 1);
            gfx_PrintInt(loadData[dataElement], 1);

            msleep(100);

    } while (!exit);

    gfx_End();
    
    uint8_t handle;
    handle = ti_Open("FakeSnake_save", "w");

    ti_Write(loadData, sizeof(int), sizeof(loadData) / sizeof(int), handle);

    ti_Close(handle);


	return 0;
}