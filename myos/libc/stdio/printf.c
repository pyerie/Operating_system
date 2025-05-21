#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(str, len))
				return -1;
			written += len;
		} else if (*format == 'x'){
			format++;
			uint32_t str = va_arg(parameters, uint32_t);
			int i = 0;
			char* buffer;
			if (str == 0){
				buffer[i++] = '0';
			} else {
				while (str != 0){
					uint8_t rem = str % 16;
					buffer[i++] = (rem < 10) ? (rem + '0') : (rem - 10 + 'A');
					str /= 16;

				}

			}
			buffer[i] = '\0';
			int start = 0;
			int end = i - 1;
			while (start < end){
				char temp = buffer[start];
				buffer[start] = buffer[end];
				buffer[end] = temp;
				start++;
				end--;

			}
			size_t len = strlen(buffer);
			if (!print(buffer, len))
                                return -1;
                        written += len;


		} else if (*format == 'd'){
			int i = 0;
			char* buffer;
			format++;
			uint32_t input = va_arg(parameters, uint32_t);
			if (input == 0){
				buffer[i++] = '0';
				buffer[i] = '\0';
			}
			while (input != 0){
				buffer[i++] = (input % 10) + '0';
				input /= 10;
			}
			buffer[i] = '\0';
			int start = 0;
			int end = i - 1;
			while (start < end){
				char temp = buffer[start];
				buffer[start] = buffer[end];
				buffer[end] = temp;
				start++;
				end--;
			}
			size_t len = strlen(buffer);
                        if (!print(buffer, len))
                                return -1;
                        written += len;

		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}
