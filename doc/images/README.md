# Images Directory

This directory contains images for the SOEM GUI documentation.

## Current Images

- `legacy_iomap.png` - Legacy I/O mapping diagram
- `memory_layout.png` - Memory layout diagram  
- `overlapping_iomap.png` - Overlapping I/O mapping diagram

## Adding GUI Screenshot

To add a GUI screenshot for the README:

1. **Take a screenshot** of the SOEM GUI application running
2. **Save it as** `gui_screenshot.png` in this directory
3. **Recommended size:** 1200x800 pixels or similar aspect ratio
4. **Format:** PNG (preferred) or JPG
5. **Content:** Show the main interface with:
   - Network settings panel
   - Motion control section
   - Data monitoring graphs
   - Status indicators

## Screenshot Guidelines

- **Clear and readable:** Ensure text and controls are visible
- **Representative:** Show typical usage scenario
- **Professional:** Clean desktop background, no sensitive information
- **Optimized:** Compress image for web viewing (under 500KB)

## Example Command

```bash
# On Linux, you can use:
gnome-screenshot -w -f doc/images/gui_screenshot.png

# Or use any screenshot tool and save to this directory
```

The README.md will automatically display the screenshot once you add the file. 