#include <c++/cstdint>
#include <debug.h>
#include <fileioc.h>
#include <graphx.h>
#include <keypadc.h>
#include <stdlib.h>
#include <sys/timers.h>
#include <sys/util.h>
#include <ti/screen.h>

/// @brief Check if `a` is in a certain number of `b`
/// @param a The `int` to check on `b`
/// @param b The `int` to check `a` with
/// @param deviation How far the `a` must be from `b` to return true
/// @return Whether or not `a` is within `deviation` of `b`
bool checkInRange(int a, int b, int deviation)
{
	if (a < b)
	{
		a = a + b;
		b = a - b;
		a = a - b;
	}

	return ((a - b) <= deviation || a == b);
}
uint8_t pointColorCheck(int points, int needed, uint8_t color1, uint8_t color2);

void gfxa_createText(const char *string, uint8_t color, int x, int y);
void gfxa_createText_Int(const char *string, uint8_t color, int x, int y, int addonInt, uint8_t addonColor, uint8_t length = 2);
void gfxa_createFillRectangle(int x, int y, int width, int height, uint8_t color);
void gfxa_createFillCircle(int x, int y, int radius, uint8_t color);

int main(void)
{
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
	int deathTimerMax = 60;
	int points = 0;
	int totalPoints = 0;

	bool dead = false;

	/* Begin loading save */
	uint8_t loadHandle;
	int loadData[] = {points, totalPoints, circleRadius, deathTimerMax};
	loadHandle = ti_Open("FakeSnake_save", "r");

	if (loadHandle != 0)
	{
		ti_Read(loadData, sizeof(int), sizeof(loadData) / sizeof(int), loadHandle);
		points = loadData[0];
		totalPoints = loadData[1];
		circleRadius = loadData[2];
		deathTimerMax = loadData[3];
	}

	ti_Close(loadHandle);
	/* End loading save */

	gfx_Begin();

	/* Draw first items to the screen */
	gfxa_createFillCircle(circleX, circleY, circleRadius, 0xE0);
	gfxa_createText_Int("Points: ", 0x00, 1, 1, points, 0x00);
	gfxa_createText_Int("Total Points: ", 0x00, GFX_LCD_WIDTH - gfx_GetStringWidth("Total Points: 0000"), 1, totalPoints, 0x00);

	/* Loop until 2nd is pressed */
	do
	{
		/* Update kb_Data */
		kb_Scan();

		/* Load group 7 registers */
		key = kb_Data[7];

		switch (key)
		{
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

		if (kb_Data[6] == kb_Enter)
		{
			/* Draw the shop to the screen */
			gfx_FillScreen(0xFF);

			gfxa_createText_Int("Points: ", 0x00, 1, 1, points, 0x07, 4);

			gfxa_createText_Int("1|More Apple Width - ", 0x00, 1, 15, circleRadius * 2, pointColorCheck(points, circleRadius * 2, 0x07, 0xE0), 4);
			gfxa_createText("Points", 0x00, gfx_GetStringWidth("1|More Apple Width - 0000 "), 15);

			gfxa_createText_Int("2|+1s Death Timer - ", 0x00, 1, 30, ((deathTimerMax + 10) / 5 + 5) * 3, pointColorCheck(points, ((deathTimerMax + 10) / 5 + 5) * 3, 0x07, 0xE0), 4);
			gfxa_createText("Points", 0x00, gfx_GetStringWidth("2|+1s Death Timer - 0000 "), 30);

			gfxa_createText("0|Exit", 0x00, 1, 45);

			bool menuDone = false;

			/* Wait for input */
			do
			{
				kb_Scan();
				key = kb_Data[3];

				switch (key)
				{
				case kb_1:
					if (points >= circleRadius * 2)
					{
						/* Purchase bigger circle */
						points -= circleRadius * 2;
						circleRadius++;
						menuDone = true;
						break;
					}
					else
					{
						break;
					}

				case kb_0:
					/* Exit shop */
					menuDone = true;
					break;
				}

				key = kb_Data[4];

				if (key == kb_2 && points >= ((deathTimerMax + 10) / 5 + 5) * 3)
				{
					/* Purchase more death time */
					points -= ((deathTimerMax + 10) / 5 + 5) * 3;
					deathTimerMax += 10;
					menuDone = true;
					break;
				}

			} while (!menuDone);
			/* Redraw game screen */
			gfx_FillScreen(0xFF);
			gfxa_createText_Int("Points: ", 0x00, 1, 1, points, 0x00);
			gfxa_createText_Int("Time until death: ", 0x00, 1, GFX_LCD_HEIGHT - 10, (deathTimerMax + 10 - deathTimer) / 10, 0x00);
			gfxa_createText_Int("Total Points: ", 0x00, GFX_LCD_WIDTH - gfx_GetStringWidth("Total Points: 0000"), 1, totalPoints, 0x00);
		}

		if (checkInRange(curX + 5, circleX, circleRadius) && checkInRange(curY + 5, circleY, circleRadius * 2))
		{
			/* Collect "apple" */
			deathTimer = 0;
			points++;
			totalPoints++;
			gfxa_createFillCircle(circleX, circleY, circleRadius, 0xFF);
			gfxa_createFillRectangle(1, 1, GFX_LCD_WIDTH, 10, 0xFF);
			gfxa_createText_Int("Points: ", 0x00, 1, 1, points, 0x00);
			gfxa_createText_Int("Total Points: ", 0x00, GFX_LCD_WIDTH - gfx_GetStringWidth("Total Points: 0000"), 1, totalPoints, 0x00);
			circleX = randInt(50, GFX_LCD_WIDTH - 50);
			circleY = randInt(50, GFX_LCD_HEIGHT - 50);
			// dbg_printf("close to circle\n");
		}
		else
		{
			deathTimer++;
			if (deathTimer % 10 == 1)
			{
				gfxa_createFillRectangle(1, GFX_LCD_HEIGHT - 10, GFX_LCD_WIDTH, 10, 0xFF);
			}
			/* Draw death timer to screen */
			gfxa_createText_Int("Time until death: ", 0x00, 1, GFX_LCD_HEIGHT - 10, (deathTimerMax + 10 - deathTimer) / 10, 0x60);
			// dbg_printf("timer++\n");
			dbg_printf("deathTimer %d\n", deathTimer);
			dbg_printf("deathTimerMax %d\n", deathTimerMax);
			// dbg_printf("curX: %d\n", curX);
			// dbg_printf("curY: %d\n", curY);
			// dbg_printf("circleX: %d\n", circleX);
			// dbg_printf("circleY: %d\n", circleY);
			if (deathTimer == deathTimerMax)
			{
				// dbg_printf("dead\n");
				dead = true;
			}
		}
		/* Step everything one tick forward */
		gfxa_createFillRectangle(curX, curY, 10, 10, 0xFF);

		curX += addX * 5;
		curY += addY * 5;
		gfxa_createFillRectangle(curX, curY, 10, 10, 0x00);

		gfxa_createFillCircle(circleX, circleY, circleRadius, 0xE0);

		msleep(100);

	} while (kb_Data[1] != kb_2nd && dead != true);

	gfx_End();
	if (!dead)
	{
		/* Begin saving */
		uint8_t handle;
		uint8_t res;
		int data[] = {points, totalPoints, circleRadius, deathTimerMax};
		handle = ti_Open("FakeSnake_save", "w");

		res = ti_Write(data, sizeof(int), sizeof(data) / sizeof(int), handle);

		if (res == sizeof(data) / sizeof(int))
		{
			ti_SetArchiveStatus(true, handle);
		}

		ti_Close(handle);
		/* End saving */
	}
	else
	{
		ti_Delete("FakeSnake_save");
	}

	return 0;
}

/* this one is dumb I just wanted to save some lines (for some reason) */
uint8_t pointColorCheck(int points, int needed, uint8_t color1, uint8_t color2)
{
	if (points >= needed)
	{
		return color1;
	}
	return color2;
}

/* Print colored text to the screen */
void gfxa_createText(const char *string, uint8_t color, int x, int y)
{
	gfx_SetTextFGColor(color);
	gfx_SetTextXY(x, y);
	gfx_PrintString(string);
}

/* Print colored text to the screen followed by a colored integer */
void gfxa_createText_Int(const char *string, uint8_t color, int x, int y, int addonInt, uint8_t addonColor, uint8_t length)
{
	gfx_SetTextFGColor(color);
	gfx_SetTextXY(x, y);
	gfx_PrintString(string);
	gfx_SetTextXY(gfx_GetStringWidth(string) + x - 1, y);
	gfx_SetTextFGColor(addonColor);
	gfx_PrintInt(addonInt, length);
}

/* Print a full colored rectangle to the screen */
void gfxa_createFillRectangle(int x, int y, int width, int height, uint8_t color)
{
	gfx_SetColor(color);
	gfx_FillRectangle(x, y, width, height);
}

/* Print a full colored circle to the screen */
void gfxa_createFillCircle(int x, int y, int radius, uint8_t color)
{
	gfx_SetColor(color);
	gfx_FillCircle(x, y, radius);
}

uint32_t srandomInt(int min, int max, int seed)
{
	srandom(seed);

	return (random() % ((max) - (min) + 1) + (min));
}