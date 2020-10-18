#include "../include/helpers.h"

#include <math.h> /* logf */

#define SWAP(_first, _second) do{       \
    register char _temp = *(_second);   \
    *(_second) = *(_first);             \
    *(_first) = _temp;                  \
}while(0)

/* Transform macro */
#define REVERSE(_str, _length) do{          \
    register int start = 0;                 \
    register int end = (_length) - 1;       \
                                            \
    while (start < end) {                   \
        SWAP(&(_str[start]), &(_str[end])); \
        ++start;                            \
        --end;                              \
    }                                       \
}while(0)


void itoa_d(const int _num_param, char* _str_param) {
    register int num = _num_param;

    /* Process individual digits */
    register int i = 0;
    while (num != 0) {
        register int rem = num % 10;

        if (rem > 9)
            _str_param[i] = (char)((rem - 10) + 'a');
        else
            _str_param[i] = (char)(rem + '0');

        num /= 10;

        ++i;
    }

    _str_param[i] = '\0'; /* Append string terminator */

    /* Reverse the string */
    REVERSE(_str_param, i);
}

inline unsigned long count_numbers(const int _num) {
    return (unsigned long)logf((float)_num);
}
