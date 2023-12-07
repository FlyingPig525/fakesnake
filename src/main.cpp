#include <graphx.h>
#include <keypadc.h>
#include <stdlib.h>
#include <sys/timers.h>
#include <sys/util.h>
#include <ti/screen.h>
#include <debug.h>

bool checkInRange(int a, int b, int deviation) {
    if (a < b) {
        a = a + b;
        b = a - b;
        a = a - b;
    }

    return ((a - b) <= deviation || a == b);
}

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

            gfx_SetTextXY(1, 1);
            gfx_PrintString("Points: ");
            gfx_SetTextXY(gfx_GetStringWidth("Points: "), 1);
            gfx_PrintInt(points, 4);

            gfx_SetTextXY(1, 15);
            gfx_PrintString("1|More Apple Width - ");
            gfx_SetTextXY(gfx_GetStringWidth("1|More Apple Width - "), 15);
            gfx_PrintInt(circleRadius * 2, 4);
            gfx_SetTextXY(gfx_GetStringWidth("1|More Apple Width - 0000 "), 15);
            gfx_PrintString("Points");

            gfx_SetTextXY(1, 30);
            gfx_PrintString("2|+100ms Death Timer - ");
            gfx_SetTextXY(gfx_GetStringWidth("2|+100ms Death Timer - "), 30);
            gfx_PrintInt(deathTimerMax / 5 + 5, 4);
            gfx_SetTextXY(gfx_GetStringWidth("2|+100ms Death Timer - 0000"), 30);
            gfx_PrintString("Points");

            gfx_SetTextXY(1, 45);
            gfx_PrintString("0|Exit");

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
            gfx_SetTextXY(1, GFX_LCD_HEIGHT - 10);
            gfx_PrintString("Time until death (sec): ");
            gfx_SetTextXY(gfx_GetStringWidth("Time until death (sec): "),
                GFX_LCD_HEIGHT - 10);
            gfx_PrintInt((deathTimerMax + 10 - deathTimer) / 10, 2);
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
