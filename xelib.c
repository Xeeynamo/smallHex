#include "xelib.h"

static const char STR_CONV_DIGITS[] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

int Itoa(int value, char *str, int base)
{
	int strLength = 0;
	if (base == 0)
		base = 10;
	if (base == 10)
	{
		if (value < 0)
		{
			if (str != 0)
				*str++ = '-';
			strLength++;
			value = -value;
		}
	}
	if (str != 0)
	{
		int i;
		char *tmp = str;
		do
		{
			*tmp++ = STR_CONV_DIGITS[value % base];
			value /= base;
			strLength++;
		} while (value != 0);
		for (i = strLength / 2 - 1; i >= 0; i--)
		{
			char c = str[i];
			str[i] = str[strLength - i - 1];
			str[strLength - i - 1] = c;
		}
	}
	else
	{
		do
		{
			value /= base;
			strLength++;
		} while (value != 0);
	}
	return strLength;
}
int Ltoa(long long value, char *str, int base)
{
	int strLength = 0;
	if (base == 0)
		base = 10;
	if (base == 10)
	{
		if (value < 0)
		{
			if (str != 0)
				*str++ = '-';
			strLength++;
			value = -value;
		}
	}
	if (str != 0)
	{
		int i;
		char *tmp = str;
		do
		{
			*tmp++ = STR_CONV_DIGITS[value % base];
			value /= base;
			strLength++;
		} while (value != 0);
		for (i = strLength / 2 - 1; i >= 0; i--)
		{
			char c = str[i];
			str[i] = str[strLength - i - 1];
			str[strLength - i - 1] = c;
		}
	}
	else
	{
		do
		{
			value /= base;
			strLength++;
		} while (value != 0);
	}
	return strLength;
}