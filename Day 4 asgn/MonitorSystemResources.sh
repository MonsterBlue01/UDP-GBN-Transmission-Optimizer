#!/bin/bash

CPU_THRESHOLD=80
MEM_THRESHOLD=70

while true
do
    CPU_USAGE=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}' | cut -d'%' -f1)
    MEM_USAGE=$(free | grep Mem | awk '{print $3/$2 * 100.0}')

    if (( $(echo "$CPU_USAGE > $CPU_THRESHOLD" | bc -l) )); then
        echo "Warning: CPU usage exceeds threshold, current usage is $CPU_USAGE%" | logger -t SystemMonitor
    fi

    if (( $(echo "$MEM_USAGE > $MEM_THRESHOLD" | bc -l) )); then
        echo "Warning: Memory usage exceeds threshold, current usage is $MEM_USAGE%" | logger -t SystemMonitor
    fi

    sleep 300
done