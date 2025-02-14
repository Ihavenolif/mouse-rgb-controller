#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_rgb(libusb_device_handle *handle, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t data[520] = "\004\021\000{\000\000\000\000d\b\003\027\200\001\002\003\004\005\a\r\000\000\000\000\000\000\000\000\000\377\000\000\000\000\377\000\377\000\377F\000\000\377\377\377\000\377\377\377\377\000\000\000\0022\000@\253\315\357\002\a\377\000\000\000\377\000\000\000\377\000\377\377\377\377\000\377\000\377\377\377\377BB@\377\000\000\000\000\377\000\377\000\200\000\377\000\000\000\000\000\000\000\000\000\000\000\000B\377\000\000\000\000\377BB\002\200\000\377\000\377\000\000\000\000\377\377\000\377\377\377\000\000\377\377\372\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000";

    data[57] = red;
    data[58] = green;
    data[59] = blue;

    int transferred;
    int res = libusb_control_transfer(handle,
                                      0x21,
                                      0x09,
                                      0x0304,
                                      1,
                                      data, sizeof(data),
                                      0);

    if (res < 0)
    {
        fprintf(stderr, "Error in control transfer: %s\n", libusb_error_name(res));
    }
}
int main(int argc, char *argv[])
{
    if (argc <= 1 || strlen(argv[0]) == 0 || argv[1][0] != '#')
    {
        printf("Invalid format. Example:\n\nmouse-rgb \"#ff0000\"");
    }

    int red, green, blue;

    sscanf(argv[1], "#%2x%2x%2x", &red, &green, &blue);

    libusb_context *ctx = NULL;
    libusb_device_handle *handle = NULL;

    if (libusb_init(&ctx) < 0)
    {
        fprintf(stderr, "libusb initialization failed\n");
        return EXIT_FAILURE;
    }

    handle = libusb_open_device_with_vid_pid(ctx, 0x258a, 0x1007); // Replace with your mouse's VID and PID
    if (handle == NULL)
    {
        fprintf(stderr, "Cannot open device\n");
        libusb_exit(ctx);
        return EXIT_FAILURE;
    }

    // Detach the kernel driver if necessary
    if (libusb_kernel_driver_active(handle, 1) == 1)
    {
        int res = libusb_detach_kernel_driver(handle, 1);
        if (res < 0)
        {
            fprintf(stderr, "Failed to detach kernel driver: %s\n", libusb_error_name(res));
            libusb_close(handle);
            libusb_exit(ctx);
            return EXIT_FAILURE;
        }
    }

    int x = libusb_claim_interface(handle, 1);

    set_rgb(handle, red, green, blue); // Set color to red
    libusb_release_interface(handle, 1);

    x = libusb_attach_kernel_driver(handle, 1);

    libusb_close(handle);
    libusb_exit(ctx);
    return EXIT_SUCCESS;
}