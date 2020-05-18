#pragma once


/// Check if is a valid char or return a space
#define SHRINK_ASCII(c) (128 - c) > 21 ?  c : 0x20