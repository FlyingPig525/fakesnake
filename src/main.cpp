#include <graphx.h>
#include <keypadc.h>
#include <stdlib.h>
#include <sys/timers.h>
#include <sys/util.h>
#include <ti/screen.h>
#include <debug.h>
#include <cstdint>

bool checkInRange(int a, int b, int deviation) {
    if (a < b) {
        a = a + b;
        b = a - b;
        a = a - b;
    }

    return ((a - b) <= deviation || a == b);
}

void gfxa_createText(const char *string, uint8_t color, int x, int y);
void gfxa_createText_Int(const char *string, uint8_t color, int x, int y, int addonInt, uint8_t addonColor, uint8_t length = 2);

int main(void) {
    /* Key variable */
    kb_key_t key;

    int curX = 20;
    int curY = 20;

    int addX = 1;
    int addY = 0;

    int circleX = 50;
    int circleY = 75;
    int circleRadius = 5;

    int deathTimer = 0;
    int deathTimerMax = 50;
    int points = 0;

    bool dead = false;

    gfx_Begin();

    gfx_FillCircle(circleX, circleY, 5);
    gfx_SetTextXY(1, 1);
    gfx_PrintString("Points: ");
    gfx_SetTextXY(50, 1);
    gfx_PrintInt(points, 2);

    /* Loop until 2nd is pressed */
    do {
        /* Update kb_Data */
        kb_Scan();

        /* Load group 7 registers */
        key = kb_Data[7];

        switch (key) {
        case kb_Down:
            addY = 1;
            addX = 0;
            break;
        case kb_Right:
            addX = 1;
            addY = 0;
            break;
        case kb_Up:
            addY = -1;
            addX = 0;
            break;

        case kb_Left:
            addX = -1;
            addY = 0;
            break;

        default:
            break;
        }

        if (kb_Data[6] == kb_Enter) {
            gfx_FillScreen(0xFF);

            gfxa_createText_Int("Points: ", 0x00, 1, 1, points, 0x07, 4);

            gfxa_createText_Int("1|More Apple Width - ", 0x00, 1, 15, circleRadius * 2, 0x07, 4);
            gfxa_createText("Points", 0x00, gfx_GetStringWidth("1|More Apple Width - 0000 "), 15);

            gfxa_createText_Int("2|+100ms Death Timer - ", 0x00, 1, 30, deathTimerMax / 5 + 5, 0x07, 4);
            gfxa_createText("Points", 0xFF, gfx_GetStringWidth("2|+100ms Death Timer - 0000"), 30);

            gfxa_createText("0|Exit", 0x00, 1, 45);

            bool menuDone = false;

            do {
                kb_Scan();
                key = kb_Data[3];

                switch (key) {
                case kb_1:
                    if (points >= circleRadius * 2) {
                        points -= circleRadius * 2;
                        circleRadius++;
                        menuDone = true;
                        break;
                    }
                    else { break; }

                case kb_0:
                    menuDone = true;
                    break;
                }

                key = kb_Data[4];

                if (key == kb_2 && points >= deathTimerMax / 5 + 5) {
                    points -= deathTimerMax / 5 + 5;
                    deathTimerMax++;
                    menuDone = true;
                    break;
                }

            } while (!menuDone);
            gfx_FillScreen(0xFF);
        }

        gfx_SetColor(0xFF);

        if (checkInRange(curX + 5, circleX, circleRadius * 2) &&
            checkInRange(curY + 5, circleY, circleRadius * 2)) {
            deathTimer = 0;
            points++;
            gfx_FillCircle(circleX, circleY, circleRadius);
            gfx_FillRectangle(1, 1, GFX_LCD_WIDTH, 10);
            gfx_SetTextXY(1, 1);
            gfx_PrintString("Points: ");
            gfx_SetTextXY(gfx_GetStringWidth("Points: "), 1);
            gfx_PrintInt(points, 2);
            circleX = randInt(30, 200);
            circleY = randInt(30, 200);
            // dbg_printf("close to circle\n");
        }
        else {
            deathTimer++;
            if (deathTimer % 10 == 1) {
                gfx_FillRectangle(1, GFX_LCD_HEIGHT - 10, GFX_LCD_WIDTH, 10);
            }

            gfxa_createText_Int("Time until death (sec): ", 0x00, 1, GFX_LCD_HEIGHT - 10, (deathTimerMax + 10 - deathTimer) / 10, 0x00);
            // dbg_printf("timer++\n");
            dbg_printf("deathTimer %d\n", deathTimer);
            dbg_printf("deathTimerMax %d\n", deathTimerMax);
            // dbg_printf("curX: %d\n", curX);
            // dbg_printf("curY: %d\n", curY);
            // dbg_printf("circleX: %d\n", circleX);
            // dbg_printf("circleY: %d\n", circleY);
            if (deathTimer == deathTimerMax) {
                // dbg_printf("dead\n");
                dead = true;
            }
        }

        gfx_FillRectangle(curX, curY, 10, 10);

        curX += addX * 5;
        curY += addY * 5;
        gfx_SetColor(0x00);
        gfx_FillRectangle(curX, curY, 10, 10);
        gfx_SetColor(0x47);
        gfx_FillCircle(circleX, circleY, circleRadius);

        msleep(100);

    } while (kb_Data[1] != kb_2nd && dead != true);

    gfx_End();

    return 0;
}

void gfxa_createText(const char *string, uint8_t color, int x, int y) {
    gfx_SetTextFGColor(color);
    gfx_SetTextXY(x, y);
    gfx_PrintString(string);
}

void gfxa_createText_Int(const char *string, uint8_t color, int x, int y, int addonInt, uint8_t addonColor, uint8_t length) {
    gfx_SetTextFGColor(color);
    gfx_SetTextXY(x, y);
    gfx_PrintString(string);
    gfx_SetTextXY(gfx_GetStringWidth(string), y);
    gfx_SetTextFGColor(addonColor);
    gfx_PrintInt(addonInt, length);
}

