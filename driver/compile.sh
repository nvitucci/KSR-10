#!/bin/bash

gcc -lusb-1.0 -lncurses -pedantic -W -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -O2 ksr10.c -o ksr10
# gcc -lusb-1.0 -lncurses ksr10.c -o ksr10
