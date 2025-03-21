nPM2100 bare metal drivers
==========================

This driver collection supports bare metal development with the nPM2100.
For development using NCS or Zephyr, use the native drivers instead.

To adapt this to your own project, copy the src and hal folders to your project.
The hal/i2c.h file contains the function declarations that must be defined in your project.
