#define GPIO_D_BASE  0x40011400
#define GPIO_D_CFGLR ((volatile uint32_t *)0x40011400)
#define GPIO_D_CFGHR ((volatile uint32_t *)0x40011404)
#define GPIO_D_INDR  ((volatile uint32_t *)0x40011408)
#define GPIO_D_OUTDR ((volatile uint32_t *)0x4001140C)
#define GPIO_D_BSHR  ((volatile uint32_t *)0x40011410)
#define GPIO_D_BCR   ((volatile uint32_t *)0x40011414)

int get_keyboard_button(void);

void init_keyboard();