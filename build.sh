#!/bin/bash
gcc gpu-fan-control.c -fdiagnostics-color=always -g \
    -o gpu-fan-control \
    -L/usr/lib/x86_64-linux-gnu -lnvidia-ml
