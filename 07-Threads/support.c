#include <stdint.h>
#include "support.h"

unsigned int strlen(const char *str)
{
	unsigned int i = 0;
	for(i=0; str[i]!='\0'; i++);
	return i;
}

int strcmp(const char *str1, const char *str2)
{
	while (*str1++ == *str2++) {
		if (*str1 == '\0' || *str2 == '\0') {
			if (*str1 == *str2)
				return 1;
			return 0;
		}
	}
	return 0;
}

void strcpy(char *src, char *dst)
{
	int i = 0;
	while (dst[i] != '\0') {
		src[i] = dst[i];
		i++;
	}
}

void reverse(char *str)
{
	unsigned int i = 0;
	unsigned int length = strlen(str) - 1;
	char c;
	
	for (i = 0; i < length; i++, length--) {
		c = str[i];
		str[i] = str[length];
		str[length] = c;
	}
}

void itoa(int n, char *str)
{
	int i = 0, sign;
	if ((sign = n) < 0)
		n = -n;
	do {
		str[i++] = n % 10 + '0';
	} while((n /= 10) > 0);
	if (sign < 0)
		str[i++] = '-';
	str[i] = '\0';
	reverse(str);
}

int atoi(const char *str)
{
	int i = 0, num = 0, sign = 0;
	if (str[i] == '-') {
		i++;
		sign = 1;
	}
	while (str[i] >= '0' && str[i] <= '9') {
		num = num * 10 + str[i] - '0';
		i++;
	}
	if (sign)
		num = -num;
	return num;
}
