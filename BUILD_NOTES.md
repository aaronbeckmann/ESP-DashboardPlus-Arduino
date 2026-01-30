# ESP-DashboardPlus Arduino Library - Build Notes

## Generating the Compressed HTML Header

The Arduino IDE version of this library requires a pre-generated `dashboard_html.h` 
file containing the gzip-compressed dashboard HTML.

### For Users Modifying the Dashboard

If you modify `extras/dashboard.html`, regenerate the header:

```bash
python scripts/html_to_header.py extras/dashboard.html src/dashboard_html.h
```

This will:
1. Read `extras/dashboard.html`
2. Compress it with gzip (level 9)
3. Generate `src/dashboard_html.h` with the compressed data

### Requirements

- Python 3.6+
- No additional packages required (uses only standard library)

### File Structure

```
esp32-dashboard-arduino/
├── library.properties      # Arduino IDE manifest
├── keywords.txt            # Syntax highlighting
├── README.md               # User documentation
├── LICENSE                 # MIT License
├── src/
│   ├── ESPDashboardPlus.h  # Main library header
│   └── dashboard_html.h    # Pre-compressed HTML (generated)
├── extras/
│   └── dashboard.html      # Source HTML file
├── scripts/
│   ├── html_to_header.py   # Compression script
│   └── generate_header.py  # Convenience wrapper
└── examples/
    └── Basic/
        └── Basic.ino       # Example sketch
```

### Difference from PlatformIO Version

The PlatformIO version uses a pre-build script that automatically generates 
`dashboard_html.h` during compilation. The Arduino IDE version requires 
pre-generation because Arduino IDE doesn't support custom build scripts.
