#!/bin/bash

# Get current date and time
now=$(date +"%Y-%m-%d_%H-%M-%S")

# Create a report file
report_file="system_health_report_$now.txt"
touch "$report_file"

# Collect system information
echo "System Health Report - $now" > "$report_file"
echo "-------------------------------------------------" >> "$report_file"

# CPU Usage
echo "CPU Usage:" >> "$report_file"
mpstat >> "$report_file"

# Memory Usage
echo "Memory Usage:" >> "$report_file"
free -h >> "$report_file"

# Disk Usage
echo "Disk Usage:" >> "$report_file"
df -h >> "$report_file"

# Network Activity
echo "Network Activity:" >> "$report_file"
netstat -s >> "$report_file"

# Output report path
echo "System health report has been generated: $report_file"
