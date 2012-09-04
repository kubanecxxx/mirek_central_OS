/**
 * @file chsprintf.c
 * @author kubanec
 * @date 29.8.2012
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "ch.h"
#include <stdarg.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#define MAX_FILLER 11

static char *long_to_string_with_divisor(char *p, long num, unsigned radix,
		long divisor)
{
	int i;
	char *q;
	long l, ll;

	l = num;
	if (divisor == 0)
	{
		ll = num;
	}
	else
	{
		ll = divisor;
	}

	q = p + MAX_FILLER;
	do
	{
		i = (int) (l % radix);
		i += '0';
		if (i > '9')
			i += 'A' - '0' - 10;
		*--q = i;
		l /= radix;
	} while ((ll /= radix) != 0);

	i = (int) (p + MAX_FILLER - q);
	do
		*p++ = *q++;
	while (--i);

	return p;
}

static char *ltoa(char *p, long num, unsigned radix)
{

	return long_to_string_with_divisor(p, num, radix, 0);
}

void chsprintf(char * str, const char *fmt, ...)
{
	char * pico = str;
	va_list ap;
	char tmpbuf[MAX_FILLER + 1];
	char *p, *s, c, filler;
	int i, precision, width;
	bool_t is_long, left_align;
	long l;
#if CHPRINTF_USE_FLOAT
	float f;
#endif

	va_start(ap, fmt);
	while (TRUE)
	{
		c = *fmt++;
		if (c == 0)
		{
			va_end(ap);
			*str = 0;
			return;
		}
		if (c != '%')
		{
			*str++ = c;
			continue;
		}
		p = tmpbuf;
		s = tmpbuf;
		left_align = FALSE;
		if (*fmt == '-')
		{
			fmt++;
			left_align = TRUE;
		}
		filler = ' ';
		if (*fmt == '.')
		{
			fmt++;
			filler = '0';
		}
		width = 0;
		while (TRUE)
		{
			c = *fmt++;
			if (c >= '0' && c <= '9')
			c -= '0';
			else if (c == '*')
			c = va_arg(ap, int);
			else
			break;
			width = width * 10 + c;
		}
		precision = 0;
		if (c == '.')
		{
			while (TRUE)
			{
				c = *fmt++;
				if (c >= '0' && c <= '9')
				c -= '0';
				else if (c == '*')
				c = va_arg(ap, int);
				else
				break;
				precision *= 10;
				precision += c;
			}
		}
		/* Long modifier.*/
		if (c == 'l' || c == 'L')
		{
			is_long = TRUE;
			if (*fmt)
				c = *fmt++;
		}
		else
			is_long = (c >= 'A') && (c <= 'Z');

		/* Command decoding.*/
		switch (c)
		{
		case 'c':
			filler = ' ';
			*p++ = va_arg(ap, int);
			break;
		case 's':
			filler = ' ';
			if ((s = va_arg(ap, char *)) == 0)
				s = "(null)";
			if (precision == 0)
				precision = 32767;
			for (p = s; *p && (--precision >= 0); p++)
				;
			break;
		case 'D':
		case 'd':
			if (is_long)
			l = va_arg(ap, long);
			else
			l = va_arg(ap, int);
			if (l < 0)
			{
				*p++ = '-';
				l = -l;
			}
			p = ltoa(p, l, 10);
			break;
#if CHPRINTF_USE_FLOAT
			case 'f':
			f = (float) va_arg(ap, double);
			if (f < 0)
			{
				*p++ = '-';
				f = -f;
			}
			p = ftoa(p, f);
			break;
#endif
		case 'X':
		case 'x':
			c = 16;
			goto unsigned_common;
		case 'U':
		case 'u':
			c = 10;
			goto unsigned_common;
		case 'O':
		case 'o':
			c = 8;
			unsigned_common:
			if (is_long)
			l = va_arg(ap, long);
			else
			l = va_arg(ap, int);
			p = ltoa(p, l, c);
			break;
		default:
			*p++ = c;
			break;
		}
		i = (int) (p - s);
		if ((width -= i) < 0)
			width = 0;
		if (left_align == FALSE)
			width = -width;
		if (width < 0)
		{
			if (*s == '-' && filler == '0')
			{
				*str++ = *s++;
				i--;
			}
			do
				*str++ = filler;
			while (++width != 0);
		}
		while (--i >= 0)
			*str++ = *s++;

		while (width)
		{
			*str++ = filler;
			width--;
		}
	}
}
