struct query { uint32_t attr; uint32_t value; };
struct triple { share* permit; share* deny; share* na; };
enum CombinationRule { NOT, WEA, SMAX, SMIN, WMAX, WMIN, PO, DO, FA };
