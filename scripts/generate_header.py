#!/usr/bin/env python3
"""
ESP-DashboardPlus - Generate dashboard_html.h for Arduino IDE

This script pre-generates the compressed HTML header file for
the Arduino IDE library distribution.

Usage:
    python generate_header.py

This will create/update src/dashboard_html.h from extras/dashboard.html
"""

import os
import sys

# Add scripts directory to path
script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, script_dir)

from html_to_header import generate_header, get_lib_root

def main():
    lib_root = os.path.dirname(script_dir)  # Parent of scripts/
    
    input_file = os.path.join(lib_root, 'extras', 'dashboard.html')
    output_file = os.path.join(lib_root, 'src', 'dashboard_html.h')
    
    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found")
        print("Make sure extras/dashboard.html exists")
        sys.exit(1)
    
    print("ESP-DashboardPlus - Arduino IDE Header Generator")
    print("=" * 50)
    print(f"Input:  {input_file}")
    print(f"Output: {output_file}")
    print()
    
    generate_header(input_file, output_file)
    print("\nDone! The library is now ready for Arduino IDE.")

if __name__ == '__main__':
    main()
