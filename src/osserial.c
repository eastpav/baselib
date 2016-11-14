/* serial.c - Serial routines */

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stropts.h>
#include <stdio.h>
#include <rawtypes.h>
#include <osserial.h>

/* supported baundrates */
static int 
baudRate[] = { 
	B1200,   B1800,   B2400,   B4800,   
	B9600,   B19200,  B38400,  B57600, 
	B115200, B230400, B460800, B500000, 
	B576000, B921600, 
	-1
};

static int 
baud[] = { 
	1200,   1800,   2400,   4800,   
	9600,   19200,  38400,  57600, 
	115200, 230400, 460800, 500000, 
	576000, 921600, 
	-1
};

/* local declarations */

/* convert INT to Bxxxxx */
static int 
setBaudRate(struct termios *opts, int br)
{
	int i = 0;

	while (baud[i] != -1)
	{
		if (baud[i] == br) 
			break;
		i++;
	}
	if(baud[i] == -1) 
		return -1;

	cfsetispeed(opts, baudRate[i]);
	cfsetospeed(opts, baudRate[i]);

	return 0;
}

/* set data size  */
static int 
setDataSize(struct termios *opts, int ds)
{
	int dsize = 0;

	switch (ds)	{
		case 5:
			dsize = CS5;
			break;
		case 6:
			dsize = CS6;
			break;
		case 7:
			dsize = CS7;
			break;
		case 8:
			dsize = CS8;
			break;
		default:
			return -1;
	}

	opts->c_cflag &= ~CSIZE;
	opts->c_cflag |= dsize;

	return 0;
}

/* set data size  */
static void 
setParity(struct termios *opts, int p)
{
	switch (p){
		case 0: /* none */
			opts->c_cflag &= ~PARENB;
			opts->c_iflag &= ~INPCK; 
			break;
		case 1: /* odd */
			opts->c_cflag |= PARENB;
			opts->c_cflag |= PARODD;

			/* check parity and skip it */
			opts->c_iflag |= (INPCK | ISTRIP);
			break;
		case 2: /* even */
			opts->c_cflag |= PARENB;
			opts->c_cflag &= ~PARODD;

			/* check parity and skip it */
			opts->c_iflag |= (INPCK | ISTRIP); 
			break;
		default:
			break;
	}
}

/* set data size  */
static void 
setStopBit(struct termios *opts, int s)
{
	if (s)
		opts->c_cflag |= CSTOPB;
	else
		opts->c_cflag &= ~CSTOPB;
}

/* init a serial port */
int 
serial_init( char* name, int baudrate, int dataSize, int parity, int stopBit )
{
	int    fd;
	struct termios opts;

	if ( name == NULL ) 
		return ERROR;
	
	fd = open( name, O_RDWR | O_NOCTTY | O_NDELAY );
	if (fd < 0) return ERROR;
	
	if (tcgetattr(fd, &opts) < 0) 
		return ERROR;

	/* choose raw input */
	opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	opts.c_iflag &= ~(IXON | IXOFF | IXANY);

	/* choose raw output */
	opts.c_oflag &= ~OPOST;

	/* Enable the receiver and set local mode... */
	opts.c_cflag |= (CLOCAL | CREAD);
	
	/* set baudrate */
	if(setBaudRate(&opts, baudrate) 
		== -1) {
		close(fd);
		return -1;
	}
	/* set data size */
	if(setDataSize(&opts, dataSize) 
		== -1) {
		close(fd);
		return -1;
	}

	/* set parity and stop bit */
	setParity ( &opts, parity );
	setStopBit( &opts, stopBit);

	/* change attribute now */
	if(tcsetattr(fd, TCSANOW, &opts) < 0){
		close(fd);
		return ERROR;
	}
	
	fcntl(fd, F_SETFL, 0);
	
	return fd;	
}

// Set baudrate of serial port.
int  serial_setBaudrate( int hserial, int baudrate )
{
	struct termios opts;

	if (tcgetattr(hserial, &opts) < 0) 
		return ERROR;

	return setBaudRate(&opts, baudrate);
}

// Set baudrate of serial port.
int  serial_setDataSize( int hserial, int dataSize )
{
	struct termios opts;

	if (tcgetattr(hserial, &opts) < 0) 
		return ERROR;

	return setDataSize(&opts, dataSize);
}

// Write to serial
int serial_write( int hserial, char* buf, int len )
{
	return write(hserial, buf, len);
}

// Read from serial
int serial_read( int hserial, char* buf, int len )
{
	return read(hserial, buf, len);
}

// Close the serial
int  serial_close( int hserial )
{
	return close(hserial);
}
