/* KSR-10 Linux driver */

/* Libusb usage inspired by Jesper Thomschutz <jesperht@yahoo.com> */

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <usb.h>
#include <ncurses.h>
#include <errno.h>
#include <libusb-1.0/libusb.h>

#include "ksr10.h"

libusb_device_handle * findKSR(u_int16_t vendor, u_int16_t product)
{
    libusb_device **devs, *dev, *chosen = NULL;
    libusb_device_handle *handle;
    
    int i = 0;
    ssize_t cnt;

    cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0)
        return NULL;

    while ((dev = devs[i++]) != NULL)
    {
        struct libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev, &desc);

        if (r < 0)
        {
            fprintf(stderr, "Failed to get device descriptor!\n");
            libusb_free_device_list(devs, 1);
            return NULL;
        }
        else
        {
            printf("%04x:%04x (bus %d, device %d)\n", desc.idVendor, desc.idProduct, 
                                    libusb_get_bus_number(dev), libusb_get_device_address(dev));

            if (desc.idVendor == vendor && desc.idProduct == product)
            {
                printf("---> KSR-10 arm found!\n");
                chosen = dev;
            }
        }
    }

    if (chosen != NULL)
        libusb_open(chosen, &handle);

    libusb_free_device_list(devs, 1);
    
    return handle;
}

int send_command(libusb_device_handle * handle, unsigned char *data)
{
    return libusb_control_transfer(handle, bmRequestType, bRequest, wValue, wIndex, data, wLength, timeoutMillis);
}

float diff_t(struct timespec start, struct timespec end)
{
    struct timespec diff;

    if ((end.tv_nsec - start.tv_nsec) < 0) {
        diff.tv_sec = end.tv_sec - start.tv_sec - 1;
        diff.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        diff.tv_sec = end.tv_sec - start.tv_sec;
        diff.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    return (float) (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main (int argc, char **argv)
{
    int r;	

    printf("Libusb init...\n");
    r = libusb_init(NULL);

    if (r == 0)
        printf("Libusb working\n");
    else
    {
        printf("Libusb not working\n");
        return r;
    }

    printf("Trying to locate the KSR arm...\n");

    // Call the findKSR function and store the returned results.
    // The function takes the vendor id and product id of the desired keyboard as arguments
    // so that it knows what to look for
    libusb_device_handle *handle = findKSR(VENDOR, PRODUCT);

    if (handle != NULL)
        printf("Found the KSR-10 arm.\n");
    else
        printf("Not found.\n");

    printf("Current configuration: ");
    int c;
    libusb_get_configuration(handle, &c);
    printf("%d\n", c); 

    r = libusb_set_configuration(handle, 0x1);
    if (r == 0)
    {
        libusb_get_configuration(handle, &c);
        fprintf(stderr, "Configuration set to %d\n", c);
    }
    else
    {
        fprintf(stderr, "Problems setting configuration\n");
        return 1;
    }

    r = libusb_claim_interface(handle, 0);

    if (r == 0)
        fprintf(stderr, "Interface claimed!\n");
    else
    {
        fprintf(stderr, "Problems claiming the interface\n");
        return 2;
    } 

    /* r = libusb_clear_halt(handle, 0x0);
    if (r == 0)
        printf("Interface cleared!\n");
    else
    {
        printf("Problems clearing the interface\n");
        return;
    } */

    int res;
    
    /* int tr;

    res = libusb_interrupt_transfer(handle, 0x0, 0x0, 1, &tr, 10);
    printf("Result = %d\n", res);  */

    printf("Execute [s]mall program, [r]everse small program, [v]ibrate, or [c]ustom? (s/r/v/c) ");
    int a = getchar();

    /* Small program */
    if (a == 's')
    {
        res = send_command(handle, CTRL_K);
        usleep(1000000);
        res = send_command(handle, RESET);

        res = send_command(handle, CTRL_J);
        usleep(400000);
        res = send_command(handle, RESET);

        res = send_command(handle, CTRL_S);
        usleep(300000);
        res = send_command(handle, RESET);

        res = send_command(handle, CTRL_F);
        usleep(900000);
        res = send_command(handle, RESET);

        res = send_command(handle, CTRL_E);
        usleep(600000);
        res = send_command(handle, RESET);
    }
    /* Reverse small program */
    else if (a == 'r')
    {
        res = send_command(handle, CTRL_I);
        usleep(1000000);
        res = send_command(handle, RESET);

        res = send_command(handle, CTRL_U);
        usleep(400000);
        res = send_command(handle, RESET);

        res = send_command(handle, CTRL_W);
        usleep(300000);
        res = send_command(handle, RESET);

        res = send_command(handle, CTRL_R);
        usleep(900000);
        res = send_command(handle, RESET);

        res = send_command(handle, CTRL_D);
        usleep(600000);
        res = send_command(handle, RESET);
    }
    /* High-speed vibration */
    else if (a == 'v')
    {
        const int loops = 10;
        const int pause_us = 100000;
        
        int loop = 0;
        
        while (loop++ < loops)
        {
            res = send_command(handle, CTRL_E);
            usleep(pause_us);
            res = send_command(handle, RESET);
            
            res = send_command(handle, CTRL_D);
            usleep(pause_us);
            res = send_command(handle, RESET);
        }
    }
    /* Custom commands in an ncurses screen */
    else if (a == 'c')
    {
        int action;
    
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
    
        printw("Move the robot arm using these keys:\n");
        printw("w, s: close/open gripper\n");
        printw("e, d: move first joint up/down\n");
        printw("r, f: move second joint up/down\n");
        printw("u, j: move third joint up/down\n");
        printw("i, k: rotate base to the right/left\n");
        printw("l: switch on/off the light (TODO: it switches off automatically)\n");
        printw("q: exit\n");
        printw("\n");

        float min, max, mov;
        min = max = mov = 0.0;
        int light = 0;
    
        while ((action = getch()) != 'q') 
        {
            struct timespec ts1, ts2;
            clock_gettime(CLOCK_REALTIME, &ts1);
    
            switch (action)
            {
                case 'w':
                res = send_command(handle, CTRL_W);
                break;
    
                case 's':
                res = send_command(handle, CTRL_S);
                break;
            
                case 'e':
                res = send_command(handle, CTRL_E);
                break;
    
                case 'd':
                res = send_command(handle, CTRL_D);
                break;
    
                case 'r':
                res = send_command(handle, CTRL_R);
                break;
    
                case 'f':
                res = send_command(handle, CTRL_F);
                break;
    
                case 'u':
                res = send_command(handle, CTRL_U);
                break;
    
                case 'j':
                res = send_command(handle, CTRL_J);
                break;
    
                case 'i':
                res = send_command(handle, CTRL_I);
                break;
    
                case 'k':
                res = send_command(handle, CTRL_K);
                break;
    
                case 'l':
                if (light == 0)
                {
                    res = send_command(handle, CTRL_L);
                    light = 1;
                }
                else
                {
                    res = send_command(handle, RESET);
                    light = 0;
                }
                break;

                case 'z':
                min = mov;
                printw("Time from min to max = %.9f\n", min);
                break;

                case 'x':
                max = mov;
                printw("Time from max to min = %.9f\n", max);
                break;

                case 'c':
                printw("Moving to half the excursion??? :\n");
                
                res = send_command(handle, CTRL_U);
                usleep((min + max) / 4 * 1000000);
                res = send_command(handle, RESET);
                break;
            }
        
            if (halfdelay(5) != ERR) 
                while (getch() == action)  
                    if (halfdelay(1) == ERR)
                        break;

            res = send_command(handle, RESET);

            if (light)
                res = send_command(handle, CTRL_L);
    
            clock_gettime(CLOCK_REALTIME, &ts2);

            mov = diff_t(ts1, ts2);
            printw("Time: %.9f\n", mov);
    
            cbreak(); 
        }
        
        endwin();
    
    }

    libusb_close(handle);
    libusb_exit(NULL);

    return 0;
}
