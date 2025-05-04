#include <corex/corex.h>

void main() {
    corex_init(COREX_MEMX|COREX_DSX|COREX_STRX);

    String string;
    corex->strx.create(5, &string);
    printf("my string: %s (len)%d\n", string.buffer, corex->strx.len(&string));
    corex->strx.grow(3, "Hi!", &string);
    printf("my string: %s (len)%d\n", string.buffer, corex->strx.len(&string));
    
    String string2;
    corex->strx.create(5, &string2);
    corex->strx.copy(&string, &string2);
    printf("my string 2: %s (len)%d\n", string2.buffer, corex->strx.len(&string2));

    String string3;
    corex->strx.create(2, &string3);
    corex->strx.ncopy(2, &string2, &string3);
    printf("my string 3: %s (len)%d\n", string3.buffer, corex->strx.len(&string3));

    corex->strx.destroy(&string3);
    corex->strx.destroy(&string2);
    corex->strx.destroy(&string);

    if (corex_cleanup()) printf("Strx Test Ran!\n");
}
