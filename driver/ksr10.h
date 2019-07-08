#define VENDOR  0x1267
#define PRODUCT 0x0000

#define QUIT 'q'

#define RESET (unsigned char []) {'\x00', '\x00', '\x00'}
#define CTRL_W (unsigned char []) {'\x01', '\x00', '\x00'}
#define CTRL_S (unsigned char []) {'\x02', '\x00', '\x00'}
#define CTRL_E (unsigned char []) {'\x04', '\x00', '\x00'}
#define CTRL_D (unsigned char []) {'\x08', '\x00', '\x00'}
#define CTRL_R (unsigned char []) {'\x10', '\x00', '\x00'}
#define CTRL_F (unsigned char []) {'\x20', '\x00', '\x00'}
#define CTRL_U (unsigned char []) {'\x40', '\x00', '\x00'}
#define CTRL_J (unsigned char []) {'\x80', '\x00', '\x00'}
#define CTRL_I (unsigned char []) {'\x00', '\x01', '\x00'}
#define CTRL_K (unsigned char []) {'\x00', '\x02', '\x00'}
#define CTRL_L (unsigned char []) {'\x00', '\x00', '\xff'}

const uint8_t bmRequestType = 0x40;
const uint8_t bRequest = 0x6;
const uint16_t wValue = 0x0100;
const uint16_t wIndex = 0x0;
const uint16_t wLength = 3;
const unsigned int timeoutMillis = 10;

libusb_device_handle * findKSR(u_int16_t vendor, u_int16_t product);
float diff_t(struct timespec start, struct timespec end);
int send_command(libusb_device_handle * handle, unsigned char *data);
