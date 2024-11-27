#!/bin/bash

gcc gpu-fan-control.c \
    -fdiagnostics-color=always -g \
    -I/usr/local/cuda/include \
    -L/usr/lib/x86_64-linux-gnu \
    -lnvidia-ml \
    -o gpu-fan-control
