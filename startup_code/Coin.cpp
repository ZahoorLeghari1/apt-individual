#include "Coin.h"

// implement functions for managing coins; this may depend on your design.

enum Denomination get_denom_type(int denom) {
    for (unsigned i = 0; i < NUM_DENOMINATIONS; ++i) {
        if (DENOMINATIONS[i] == denom) {
            return (Denomination)i;
        }
    }
    return (Denomination)-1;
}
