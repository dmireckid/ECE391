#ifndef TESTS_H
#define TESTS_H

#define DIREC_0_BITS		0x23
#define DIREC_1_BITS		0xe3
#define TABLE_23_BITS		0x63

#define KERNEL_ADDR_S		0x400000
#define KERNEL_ADDR_E		0x7fffff
#define KERNEL_ADDR_M		0x4abcde
#define VIDEO_ADDR_S		0xB8000
#define VIDEO_ADDR_E		0xB8fff
#define VIDEO_ADDR_M		0xb8abc
#define FIVE				5

// test launcher
void launch_tests();

#endif /* TESTS_H */
