// 2013.7
//********************************************
//**  Copyright  (C)  WCH  2002-2013    ******
//**  Web:  http://www.winchiphead.com  ******
//********************************************
//**  Driver for USB to serial adaptor CH34X**
//**  GCC                                   **
//********************************************

// Support linux kernel version 2.6.25 and later
//

#include <linux/version.h>
#ifndef KERNEL_VERSION
#define	KERNEL_VERSION(ver, rel, seq)	((ver << 16) | (rel << 8) | (seq))
#endif

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
//#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>

#define DRIVER_DESC		"WCH CH34x USB to serial adaptor driver"
#define DRIVER_AUTHOR	"<tech@wch.cn>"

#define CH34x_VENDOR_ID		0x1A86
#define CH340_PRODUCT_ID	0x7523
#define CH341_PRODUCT_ID	0x5523

#define CH34x_CLOSING_WAIT	(30 * HZ)

#define CH34x_BUF_SIZE		1024
#define CH34x_TMP_BUF_SIZE	1024

//Vendor define
#define VENDOR_WRITE_TYPE		0x40
#define VENDOR_READ_TYPE		0xC0

#define VENDOR_READ				0x95
#define VENDOR_WRITE			0x9A
#define VENDOR_SERIAL_INIT		0xA1
#define VENDOR_MODEM_OUT		0xA4
#define VENDOR_VERSION			0x5F

//For CMD 0xA4
#define UART_CTS		0x01
#define UART_DSR		0x02
#define UART_RING		0x04
#define UART_DCD		0x08
#define CONTROL_OUT		0x10
#define CONTROL_DTR		0x20
#define	CONTROL_RTS		0x40

//Uart state
#define UART_STATE			0x00
#define UART_OVERRUN_ERROR	0x01
#define UART_BREAK_ERROR	//no define
#define UART_PARITY_ERROR	0x02
#define UART_FRAME_ERROR	0x06
#define UART_RECV_ERROR		0x02
#define UART_STATE_TRANSIENT_MASK	0x07

//Port state
#define PORTA_STATE		0x01
#define PORTB_STATE		0x02
#define PORTC_STATE		0x03

//CH34x Baud Rate
#define CH34x_BAUDRATE_FACTOR	1532620800
#define CH34x_BAUDRATE_DIVMAX	3

//#define DEBUG_CH34x
#undef  DEBUG_CH34x

#ifdef DEBUG_CH34x
#define dbg_ch34x( format, arg... )		\
	printk( KERN_DEBUG "%d: " format "\n", __LINE__, ##arg )
#else
#define dbg_ch34x( format, arg... )		\
do{									\
	if(0)							\
		printk(KERN_DEBUG "%d: " format "\n", __LINE__, ##arg);	\
} while (0)
#endif

#ifdef DEBUG_CH34x
#define err_ch34x( format, arg... )		\
	printk(KERN_ERR KBUILD_MODNAME ": " format "\n", ##arg)
#else
#define err_ch34x( format, arg... )   \
do{								\
	if(0)						\
	printk( KERN_ERR KBUILD_MODNAME ": " format "\n", ##arg)\
}while(0)						
#endif

// For debug
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,1))
static int debug = 1;
#endif

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int wait_flag = 0;

struct ch34x_buf {
	unsigned int buf_size;
	char *buf_buf;
	char *buf_get;
	char *buf_put;
};

struct ch34x_private {
	spinlock_t	lock;	//access lock
	struct ch34x_buf	*buf;
	int	write_urb_in_use;
	unsigned baud_rate;
	wait_queue_head_t	delta_msr_wait;
	u8	line_control;
	u8	line_status;
	u8	termios_initialized;
};

static struct usb_device_id	id_table [] = {
	{ USB_DEVICE(CH34x_VENDOR_ID, CH340_PRODUCT_ID) },
	{ USB_DEVICE(CH34x_VENDOR_ID, CH341_PRODUCT_ID) },
	{ } //End
};
MODULE_DEVICE_TABLE( usb, id_table );

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,2))
static struct usb_driver ch34x_driver = {
	.name		   = "ch34x",
	.probe		   = usb_serial_probe,
	.disconnect	   = usb_serial_disconnect,
	.id_table	   = id_table,
	.suspend	   = usb_serial_suspend,
	.resume		   = usb_serial_resume,
	.no_dynamic_id = 1,
	.supports_autosuspend = 1,
};
#endif

// ch34x_buf_alloc
// Allocate a circular buffer and all associated memory
static struct ch34x_buf *ch34x_buf_alloc( unsigned int size )
{
	struct ch34x_buf *pb;

	if( size == 0 )
		return NULL;

	pb = kmalloc( sizeof(struct ch34x_buf), GFP_KERNEL );
	if( pb == NULL )
		return NULL;

	pb->buf_buf = kmalloc( size, GFP_KERNEL );
	if( pb->buf_buf == NULL ) {
		kfree(pb);
		return NULL;
	}

	pb->buf_size = size;
	pb->buf_get = pb->buf_put = pb->buf_buf;

	return pb;
}

// ch34x_buf_free
// Free the buffer and all associated memory
static void ch34x_buf_free( struct ch34x_buf *pb )
{
	if( pb ) {
		kfree( pb->buf_buf );
		kfree( pb );
	}
}

// ch34x_buf_clear
// Clear out all data in the circular buffer
static void ch34x_buf_clear( struct ch34x_buf *pb )
{
	if( pb != NULL ) 
		pb->buf_get = pb->buf_put;
	// equivalent to a get of all data available
}

// ch34x_buf_data_avail
// Return the number of bytes of data available in he circular buffer
static unsigned int ch34x_buf_data_avail( struct ch34x_buf *pb )
{
	if( pb == NULL )
		return 0;

	return ((pb->buf_size + pb->buf_put - pb->buf_get) % pb->buf_size );
}

// ch34x_buf_space_avail
// Return the number of bytes of space available in the circular
static unsigned int ch34x_buf_space_avail( struct ch34x_buf *pb )
{
	if( pb == NULL )
		return 0;

	return ((pb->buf_size + pb->buf_get - pb->buf_put - 1) % pb->buf_size );
}

// ch34x_buf_put
// Copy data from a user buffer and put it into the circular buffer.
// Restrict to the amount of space available
// Return the number of bytes copied
static unsigned int ch34x_buf_put( struct ch34x_buf *pb,
		const char *buf, unsigned int count )
{
	unsigned int len;

	if( pb == NULL )
		return 0;

	len = ch34x_buf_space_avail(pb);
	if( count > len )
		count = len;
	else if( count == 0 )
		return 0;

	len = pb->buf_buf + pb->buf_size - pb->buf_put;
	if( count > len ) {
		memcpy( pb->buf_put, buf, len );
		memcpy( pb->buf_buf, buf+len, count - len );
		pb->buf_put = pb->buf_buf + count - len;
	}
	else {
		memcpy( pb->buf_put, buf, count );
		if( count < len )
			pb->buf_put += count;
		else if( count == len )
			pb->buf_put = pb->buf_buf;
	}

	return count;
}

static unsigned int ch34x_buf_get( struct ch34x_buf *pb,
		char *buf, unsigned int count )
{
	unsigned int len;

	if( pb == NULL )
		return 0;

	len = ch34x_buf_data_avail(pb);
	if( count > len )
		count = len;
	else if( count == 0 )
		return 0;

	len = pb->buf_buf + pb->buf_size - pb->buf_get;
	if( count > len ) {
		memcpy( buf, pb->buf_get, len );
		memcpy( buf+len, pb->buf_buf, count - len );
		pb->buf_get = pb->buf_buf + count - len;
	}
	else {
		memcpy( buf, pb->buf_get, count );
		if( count < len )
			pb->buf_get += count;
		else if( count == len )
			pb->buf_get = pb->buf_buf;
	}

	return count;
}

static int ch34x_vendor_read( __u8 request,
		__u16 value,
		__u16 index,
		struct usb_serial *serial,
		unsigned char *buf,
		__u16 len )
{
	int retval;

	retval = usb_control_msg( serial->dev, usb_rcvctrlpipe(serial->dev, 0), 
			request, VENDOR_READ_TYPE, value, index, buf, len, 1000 );
	dbg_ch34x("0x%x:0x%x:0x%x:0x%x %d - %d", 
			VENDOR_READ_TYPE, request, value, index, retval, len );

	return retval;
}

static int ch34x_vendor_write( __u8 request,
		__u16 value,
		__u16 index,
		struct usb_serial *serial,
		unsigned char *buf,
		__u16 len )
{
	int retval;

	retval = usb_control_msg( serial->dev, 
			usb_sndctrlpipe(serial->dev, 0),
			request,
			VENDOR_WRITE_TYPE,
			value, index, buf, len, 1000 );

	return retval;
}

static int set_control_lines( struct usb_serial *serial,
		u8 value )
{
	int retval;

	retval = ch34x_vendor_write( VENDOR_MODEM_OUT, (unsigned short)value, 
			0x0000, serial, NULL, 0x00 );
	dbg_ch34x("%s - value=%d, retval=%d", __func__, value, retval );

	return retval;
}

static int ch34x_get_baud_rate( unsigned int baud_rate,
		unsigned char *factor, unsigned char *divisor)
{
	unsigned char a;
	unsigned char b;
	unsigned long c;

	switch ( baud_rate ) {
	case 921600: 
		a = 0xf3; 
		b = 7; 
		break; 
	case 307200:
		a = 0xd9; 
		b = 7; 
		break; 
	default: 
		if ( baud_rate > 6000000/255 ) { 
			b = 3;
			c = 6000000;
		} else if ( baud_rate > 750000/255 ) {  
			b = 2;
			c = 750000;
		} else if (baud_rate > 93750/255) { 
			b = 1;
			c = 93750;
		} else {
			b = 0;
			c = 11719;
		}
		a = (unsigned char)(c / baud_rate);
		if (a == 0 || a == 0xFF) return -EINVAL;
		if ((c / a - baud_rate) > (baud_rate - c / (a + 1))) 
			a ++;
		a = 256 - a;
		break;
	}
	*factor = a;
	*divisor = b;
	return 0;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
static void ch34x_set_termios( struct tty_struct *tty,
		struct usb_serial_port *port, struct ktermios *old_termios )
{
#else
static void ch34x_set_termios( struct usb_serial_port *port,
		struct ktermios *old_termios )
{
	struct tty_struct *tty = port->tty;
#endif
	struct usb_serial *serial = port->serial;	
	struct ch34x_private *priv = usb_get_serial_port_data(port);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,1)) //sure
	struct ktermios *termios = &tty->termios;
#else
	struct ktermios *termios = tty->termios;
#endif
	unsigned int baud_rate;
	unsigned int cflag;
	unsigned long flags;
	u8 control;

	unsigned char divisor = 0;
	unsigned char reg_count = 0;
	unsigned char factor = 0;
	unsigned char reg_value = 0;
	unsigned short value = 0;
	unsigned short index = 0;
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number);
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number);
#endif
	
	spin_lock_irqsave( &priv->lock, flags );
	if( !priv->termios_initialized ) {
		*(termios) = tty_std_termios;
		termios->c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
		termios->c_ispeed = 9600;
		termios->c_ospeed = 9600;
		priv->termios_initialized = 1;
	}
	spin_unlock_irqrestore( &priv->lock, flags );

	/*
	 * The ch34x is reported to lose bytes if you change serial setting
	 * even to the same vaules as before. Thus we actually need to filter 
	 * in this specific case.
	 */
	if( !tty_termios_hw_change(termios, old_termios) )
		return;

	cflag = termios->c_cflag;
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s (%d) cflag=0x%x\n", __func__, port->number, cflag);
#else
	dbg_ch34x("%s (%d) cflag=0x%x\n", __func__, port->port_number, cflag);
#endif
	// Get the byte size
	switch( cflag & CSIZE )
	{
		case CS5:
			reg_value |= 0x00;
			break;
		case CS6:
			reg_value |= 0x01;
			break;
		case CS7:
			reg_value |= 0x02;
			break;
		case CS8:
			reg_value |= 0x03;
			break;
		default:
			reg_value |= 0x03;
			break;
	}
	dbg_ch34x("%s - data bits = %d", __func__, reg_value + 0x05 );

	// Figure out the stop bits 
	if( cflag & CSTOPB ) {
		reg_value |= 0x04;
		dbg_ch34x("%s - stop bits = 2", __func__);
	}
	else 
		dbg_ch34x("%s - stop bits = 1", __func__);

	// Determine the parity
	if (cflag & PARENB) {
		if (cflag & CMSPAR) {
			if (cflag & PARODD) {
    			reg_value |= (0x28 | 0x00);
    			dbg_ch34x("%s - parity = mark", __func__);
			} else {
    			reg_value |= (0x38 | 0x10);
    			dbg_ch34x("%s - parity = space", __func__);
			}
		} else {
			if (cflag & PARODD) {
    			reg_value |= (0x08 | 0x00);
    			dbg_ch34x("%s - parity = odd", __func__);
			} else {
    			reg_value |= (0x18 | 0x10);
    			dbg_ch34x("%s - parity = even", __func__);
			}
		}
	}
	else
		dbg_ch34x("%s - parity = none", __func__);

	// Determine the baud rate
	baud_rate = tty_get_baud_rate( tty );
	dbg_ch34x("%s = baud_rate = %d", __func__, baud_rate);	
	ch34x_get_baud_rate( baud_rate, &factor, &divisor );	
	dbg_ch34x("----->>>> baud_rate = %d, factor:0x%x, divisor:0x%x",
				baud_rate, factor, divisor );	

	//enable SFR_UART RX and TX
	reg_value |= 0xc0;
	//enable SFR_UART Control register and timer
	reg_count |= 0x9c;

	value |= reg_count;
	value |= (unsigned short)reg_value << 8;
	index |= 0x80 | divisor;
	index |= (unsigned short)factor << 8;
	ch34x_vendor_write( VENDOR_SERIAL_INIT, value, index, serial, NULL, 0 );

	// change control lines if we are switching to or from B0
	spin_lock_irqsave( &priv->lock, flags ); 
	control = priv->line_control;
	if( (cflag & CBAUD) == B0 )
		priv->line_control &= ~(CONTROL_DTR | CONTROL_RTS);
	else
		priv->line_control |= (CONTROL_DTR | CONTROL_RTS);

	if( control != priv->line_control ) {
		control = priv->line_control;
		spin_unlock_irqrestore( &priv->lock, flags );
		set_control_lines( serial, control );
	}
	else
		spin_unlock_irqrestore( &priv->lock, flags );

	if( cflag & CRTSCTS )
		ch34x_vendor_write( VENDOR_WRITE, 0x2727, 0x0101, serial, NULL, 0);

	// FIXME: Need to read back resulting baud rate
	if( baud_rate )
		tty_encode_baud_rate(tty, baud_rate, baud_rate);

}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,3)) 
static int ch34x_tiocmget( struct tty_struct *tty )
{
	struct usb_serial_port *port = tty->driver_data;
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
static int ch34x_tiocmget( struct tty_struct *tty,
		struct file *filp )
{
	struct usb_serial_port *port = tty->driver_data;
#else
static int ch34x_tiocmget( struct usb_serial_port *port,
		struct file *filp )
{
#endif
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int mcr;
	/*unsigned int msr;*/
	unsigned int retval;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number);
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number);
#endif
	if( !usb_get_intfdata( port->serial->interface) ) 
		return -ENODEV;

	spin_lock_irqsave( &priv->lock, flags );
	mcr = priv->line_control;
	spin_unlock_irqrestore( &priv->lock, flags );

	retval = ((mcr & CONTROL_DTR) ? TIOCM_DTR : 0) |
			 ((mcr & CONTROL_RTS) ? TIOCM_RTS : 0) |
			 ((mcr & UART_CTS) ? TIOCM_CTS : 0) |
			 ((mcr & UART_DSR) ? TIOCM_DSR : 0) |
			 ((mcr & UART_RING) ? TIOCM_RI : 0) |
			 ((mcr & UART_DCD) ? TIOCM_CD : 0);

	dbg_ch34x("%s - retval=0x%x", __func__, retval);

	return retval;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) && \
		LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )
static void ch34x_close( struct tty_struct *tty,
		struct usb_serial_port *port, struct file *filp )
{
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32) )
static void ch34x_close( struct usb_serial_port *port )
{
	struct tty_struct *tty = port->port.tty;
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
static void ch34x_close( struct usb_serial_port *port, 
		struct file *filp )
{

	struct tty_struct *tty = port->tty;
#endif
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int c_cflag;
	int bps;
	long timeout;
	wait_queue_t wait;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number);
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number);
#endif
//	dbg_ch34x("Addr of priv: 0x%x", priv);
//	dbg_ch34x("Addr of tty:  0x%x", tty);
	// wait for data do drain from the buffer
/*
	spin_lock_irqsave( &priv->lock, flags );
	timeout = CH34x_CLOSING_WAIT;
	init_waitqueue_entry( &wait, current );
	add_wait_queue( &tty->write_wait, &wait );
	for(;;) {
		set_current_state( TASK_INTERRUPTIBLE );
		if( ch34x_buf_data_avail(priv->buf) == 0 || timeout == 0 || 
				signal_pending(current) || port->serial->disconnected )
			break;
		spin_unlock_irqrestore( &priv->lock, flags );
		timeout = schedule_timeout( timeout );
		spin_lock_irqsave( &priv->lock, flags );
	}
	set_current_state( TASK_RUNNING );
	remove_wait_queue( &tty->write_wait, &wait );
*/
	spin_lock_irqsave( &priv->lock, flags );
	// clear out any remaining data in the buffer
	ch34x_buf_clear( priv->buf );
	spin_unlock_irqrestore( &priv->lock, flags );

/*
	bps = tty_get_baud_rate( tty );
	if( bps > 1200 ) 
		timeout = max( (HZ * 2560) / bps, HZ / 10 );
	else
		timeout = 2 * HZ;
	schedule_timeout_interruptible(timeout);
*/
	// shutdown our urbs
	usb_kill_urb(port->interrupt_in_urb);
	usb_kill_urb(port->read_urb);
	usb_kill_urb(port->write_urb);
	//usb_serial_generic_close(port, filp);

	if( tty ) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,1)) //sure
		c_cflag = tty->termios.c_cflag;
#else
		c_cflag = tty->termios->c_cflag;
#endif
		if( c_cflag & HUPCL ) {
			// drop DTR and RTS
			spin_lock_irqsave( &priv->lock, flags );
			priv->line_control = 0;
			spin_unlock_irqrestore( &priv->lock, flags );
			set_control_lines( port->serial, 0 );
		}
	}

//	usb_serial_generic_close(port);
//	usb_kill_urb(port->interrupt_in_urb);

}

// kernel version 
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) \
		&& LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
static int ch34x_open( struct tty_struct *tty,
		struct usb_serial_port *port, struct file *filp )
{
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
static int ch34x_open( struct tty_struct *tty,
		struct usb_serial_port *port )
{
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
static int ch34x_open( struct usb_serial_port *port,
		struct file *filp )
{
	struct tty_struct *tty = port->tty;
#endif
	struct ktermios tmp_termios;
	struct usb_serial *serial = port->serial;
	int retval;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number );
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number );
#endif
	usb_clear_halt( serial->dev, port->write_urb->pipe );
	usb_clear_halt( serial->dev, port->read_urb->pipe );

	if( tty ) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
		ch34x_set_termios( tty, port, &tmp_termios );
#else
		ch34x_set_termios( port, &tmp_termios );
#endif
	}

	dbg_ch34x("%s - submit read urb", __func__); 
	port->read_urb->dev = serial->dev; 
	retval = usb_submit_urb( port->read_urb, GFP_KERNEL );
	if(retval) {
		dev_err( &port->dev, "%s - failed submit read urb,error %d\n", 
				__func__, retval );
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) && \
		LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )
		ch34x_close(tty, port, NULL);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
		ch34x_close(port);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
		ch34x_close(port, filp);
#endif
		goto err_out;
	}

	dbg_ch34x("%s - submit interrupt urb", __func__ );
	port->interrupt_in_urb->dev = serial->dev; 
	retval = usb_submit_urb( port->interrupt_in_urb, GFP_KERNEL );
	if(retval) {
		dev_err( &port->dev, "%s - failed submit interrupt urb,error %d\n", 
				__func__, retval );
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) && \
		LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )
		ch34x_close(tty, port, NULL);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
		ch34x_close(port);
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
		ch34x_close(port, filp);
#endif
		goto err_out;
	}

err_out:
	return retval;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,3))
static int ch34x_tiocmset( struct tty_struct *tty,
		unsigned int set, unsigned int clear )
{
	struct usb_serial_port *port = tty->driver_data;
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
static int ch34x_tiocmset( struct tty_struct *tty,
		struct file *filp, unsigned int set, unsigned int clear )
{
	struct usb_serial_port *port = tty->driver_data;
#else
static int ch34x_tiocmset( struct usb_serial_port *port,
		struct file *filp, unsigned int set, unsigned int clear )
{
#endif
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	/*unsigned int mcr = priv->line_control;*/
	u8 control;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number);
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number);
#endif

	if( !usb_get_intfdata(port->serial->interface) ) 
		return -ENODEV;

	spin_lock_irqsave( &priv->lock, flags );
	if( set & TIOCM_RTS )
		priv->line_control |= CONTROL_RTS;
	if( set & TIOCM_DTR )
		priv->line_control |= CONTROL_DTR;
	if( clear & TIOCM_RTS )
		priv->line_control &= ~CONTROL_RTS;
	if( clear & TIOCM_DTR )
		priv->line_control &= ~CONTROL_DTR;
	control = priv->line_control;
	spin_unlock_irqrestore( &priv->lock, flags );

	return set_control_lines( port->serial, control );
}

static int wait_modem_info( struct usb_serial_port *port,
		unsigned int arg )
{
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int prevstatus;
	unsigned int status;
	unsigned int changed;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s -port:%d", __func__, port->number);
#else
	dbg_ch34x("%s -port:%d", __func__, port->port_number);
#endif
	spin_lock_irqsave( &priv->lock, flags );
	prevstatus = priv->line_status;
	spin_unlock_irqrestore( &priv->lock, flags );

	while(1) {
		wait_event_interruptible( wq, wait_flag != 0 );
		wait_flag = 0;
		// see if a signal did it
		if( signal_pending(current) ) 
			return -ERESTARTSYS;

		spin_lock_irqsave( &priv->lock, flags );
		status = priv->line_status;
		spin_unlock_irqrestore( &priv->lock, flags );

		changed = prevstatus ^ status;

		if( ((arg & TIOCM_RNG) && (changed & UART_RING)) ||
			((arg & TIOCM_DSR) && (changed & UART_DSR))  ||
			((arg & TIOCM_CD)  && (changed & UART_DCD))  ||
			((arg & TIOCM_CTS) && (changed & UART_CTS)) )
			return 0;

		prevstatus = status;
	}

	// Not reatched
	return 0;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,3,3)) 
static int ch34x_ioctl( struct tty_struct *tty,
		unsigned int cmd, unsigned long arg )
{
	struct usb_serial_port *port = tty->driver_data;
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
static int ch34x_ioctl( struct tty_struct *tty, 
		struct file *filp, unsigned int cmd, unsigned long arg )
{
	struct usb_serial_port *port = tty->driver_data;
#else
static int ch34x_ioctl( struct usb_serial_port *port,
		struct file *filp, unsigned int cmd, unsigned long arg )
{
	//struct usb_serial_port *port = tty->driver_data;
#endif
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d, cmd=0x%04x", __func__, port->number, cmd);
#else
	dbg_ch34x("%s - port:%d, cmd=0x%04x", __func__, port->port_number, cmd);
#endif
	switch(cmd)
	{
		// Note here 
		case TIOCMIWAIT:
#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
			dbg_ch34x("%s - port:%d TIOCMIWAIT", __func__, port->number);
#else
			dbg_ch34x("%s - port:%d TIOCMIWAIT", __func__, port->port_number);
#endif

			return wait_modem_info(port, arg);
		default:
			dbg_ch34x("%s not supported=0x%04x", __func__, cmd);
			break;
	}

	return -ENOIOCTLCMD;
}

static void ch34x_send( struct usb_serial_port *port )
{
	int count;
	int retval;
	struct ch34x_private *priv = usb_get_serial_port_data( port );
	unsigned long flags;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number);
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number);
#endif
	spin_lock_irqsave( &priv->lock, flags );
	if( priv->write_urb_in_use ) {
		spin_unlock_irqrestore( &priv->lock, flags );
		return;
	}

	count = ch34x_buf_get( priv->buf, port->write_urb->transfer_buffer,
			port->bulk_out_size );
	if( count == 0 ) {
		spin_unlock_irqrestore( &priv->lock, flags );
		return;
	}

	priv->write_urb_in_use = 1;
	spin_unlock_irqrestore( &priv->lock, flags );

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,1))
	usb_serial_debug_data( &port->dev, __func__, count,
			port->write_urb->transfer_buffer );
#else
	usb_serial_debug_data( debug, &port->dev, __func__, count,
			port->write_urb->transfer_buffer );
#endif

	port->write_urb->transfer_buffer_length = count;
	port->write_urb->dev = port->serial->dev;
	retval = usb_submit_urb( port->write_urb, GFP_ATOMIC );
	if( retval ) {
		dev_err( &port->dev, "%s - failed submitting write urb,error %d\n"
				, __func__, retval );
		priv->write_urb_in_use = 0;
		// reschedule ch34x_send
	}

	usb_serial_port_softint( port ); 
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
static int ch34x_write( struct tty_struct *tty, 
		struct usb_serial_port *port, const unsigned char *buf, int count )
#else
static int ch34x_write( struct usb_serial_port *port,
		const unsigned char *buf, int count )
#endif
{
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d, %d bytes", __func__, port->number, count);
#else
	dbg_ch34x("%s - port:%d, %d bytes", __func__, port->port_number, count);
#endif

	if( !count )
		return count;

	spin_lock_irqsave( &priv->lock, flags );
	count = ch34x_buf_put( priv->buf, buf, count ); 
	spin_unlock_irqrestore( &priv->lock, flags );

	ch34x_send(port);

	return count;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
static int ch34x_write_room( struct tty_struct *tty )
{
	struct usb_serial_port *port = tty->driver_data;
#else
static int ch34x_write_room( struct usb_serial_port *port )
{
#endif
	struct ch34x_private *priv = usb_get_serial_port_data( port );
	int room = 0;
	unsigned long flags;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number);
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number);
#endif

	spin_lock_irqsave( &priv->lock, flags );
	room = ch34x_buf_space_avail( priv->buf );
	spin_unlock_irqrestore( &priv->lock, flags );

	dbg_ch34x("%s - room:%d", __func__, room );
	return room;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
static int ch34x_chars_in_buffer( struct tty_struct *tty )
{
	struct usb_serial_port *port = tty->driver_data;
#else
static int ch34x_chars_in_buffer( struct usb_serial_port *port )
{
#endif
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	int chars = 0;
	unsigned long flags;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number);
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number);
#endif

	spin_lock_irqsave( &priv->lock, flags );
	chars = ch34x_buf_data_avail( priv->buf );
	spin_unlock_irqrestore( &priv->lock, flags );

	dbg_ch34x("%s - chars:%d", __func__, chars );

	return chars;
}

static int ch34x_attach( struct usb_serial *serial )
{
	struct ch34x_private *priv; 
	int i;
	char buf[8];

	dbg_ch34x("%s", __func__);

	for( i = 0; i < serial->num_ports; ++i ) {
		priv = kzalloc( sizeof(struct ch34x_private), GFP_KERNEL );
		if( !priv )
			goto cleanup;
		spin_lock_init( &priv->lock );
		priv->buf = ch34x_buf_alloc( CH34x_BUF_SIZE );
		if( priv->buf == NULL ) {
			kfree( priv );
			goto cleanup;
		}
		init_waitqueue_head( &priv->delta_msr_wait );
		usb_set_serial_port_data( serial->port[i], priv );
	}

	ch34x_vendor_read( VENDOR_VERSION, 0x0000, 0x0000,
			serial, buf, 0x02 );
	ch34x_vendor_write( VENDOR_SERIAL_INIT, 0x0000, 0x0000,
			serial, NULL, 0x00 );
	ch34x_vendor_write( VENDOR_WRITE, 0x1312, 0xD982, 
			serial, NULL, 0x00 );
	ch34x_vendor_write( VENDOR_WRITE, 0x0F2C, 0x0004,
			serial, NULL, 0x00 );
	ch34x_vendor_read( VENDOR_READ, 0x2518, 0x0000,
			serial, buf, 0x02 );
	ch34x_vendor_write( VENDOR_WRITE, 0x2727, 0x0000,
			serial, NULL, 0x00 );
	ch34x_vendor_write( VENDOR_MODEM_OUT, 0x009F, 0x0000,
			serial, NULL, 0x00 );

	return 0;

cleanup:
	for( --i; i >= 0; --i ) {
		priv = usb_get_serial_port_data( serial->port[i] );
		ch34x_buf_free( priv->buf );
		kfree( priv );
		usb_set_serial_port_data( serial->port[i], NULL );
	}

	return -ENOMEM;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32))
static void ch34x_shutdown( struct usb_serial *serial )
{
	struct ch34x_private *priv;
	int i;

	dbg_ch34x("%s", __func__);

	for( i = 0; i < serial->num_ports; ++i ) {
		priv = usb_get_serial_port_data( serial->port[i] );
		if( priv ) {
			ch34x_buf_free( priv->buf );
			kfree( priv );
			usb_set_serial_port_data( serial->port[i], NULL );
		}
	}
}
#endif

static void ch34x_update_line_status( struct usb_serial_port *port,
		unsigned char *data, unsigned int actual_length )
{
	struct ch34x_private *priv = usb_get_serial_port_data( port );
	unsigned long flags;
	u8 length = UART_STATE + 0x04;

	if( actual_length < length )
		return;

	// Save off the uart status for others to look at
	spin_lock_irqsave( &priv->lock, flags );
	priv->line_status = data[UART_STATE];
	priv->line_control = data[PORTB_STATE];
	spin_unlock_irqrestore( &priv->lock, flags );
	wait_flag = 1;
	wake_up_interruptible( &priv->delta_msr_wait );
}

static void ch34x_read_int_callback( struct urb *urb )
{
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context; 
	unsigned char *data = urb->transfer_buffer;
	unsigned int actual_length = urb->actual_length;
	int status = urb->status;
	int retval;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s port:%d", __func__, port->number );
#else
	dbg_ch34x("%s port:%d", __func__, port->port_number );
#endif
	switch( status ) {
		case 0: //success
			break;
		case -ECONNRESET:
		case -ENOENT:
		case -ESHUTDOWN: //this urb is terminated, clean up
			dbg_ch34x("%s - urb shutting down with status:%d", __func__, status );
			return;
		default:
			dbg_ch34x("%s - nonzero urb status received:%d", __func__, status );
			goto exit;
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,1))
	usb_serial_debug_data( &port->dev, __func__,
			urb->actual_length, urb->transfer_buffer );
#else
	usb_serial_debug_data( debug, &port->dev, 
			__func__, urb->actual_length, urb->transfer_buffer );
#endif

	ch34x_update_line_status( port, data, actual_length );

exit:
	retval = usb_submit_urb( urb, GFP_ATOMIC ); 
	if( retval ) 
		dev_err( &urb->dev->dev, "%s - usb_submit_urb failed with result %d\n", 
				__func__, retval );
}

static void ch34x_read_bulk_callback( struct urb *urb )
{
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	struct ch34x_private *priv = usb_get_serial_port_data( port );
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	unsigned long flags;
	int i;
	int retval;
	int status = urb->status;
	u8 line_status;
	char tty_flag;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number );
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number);
#endif
	if( status ) {
		dbg_ch34x("%s - urb status=%d", __func__, status );
		if( status == -EPROTO ) {
			// CH34x mysteriously fails with -EPROTO reschedule the read
			dbg_ch34x("%s - caught -EPROTO, resubmitting the urb", __func__);
			urb->dev = port->serial->dev;
			retval = usb_submit_urb( urb, GFP_ATOMIC );
			if( retval ) {
				dev_err( &urb->dev->dev, 
						"%s - failed resubmitting read urb, error %d\n",
						__func__, retval );
				return;
			}
		}

		dbg_ch34x("%s - unable to handle the error, exiting.", __func__);
		return;
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,1))
	usb_serial_debug_data( &port->dev, __func__,
			urb->actual_length, data );
#else
	usb_serial_debug_data( debug, &port->dev, 
			__func__, urb->actual_length, data );
#endif

	// get tty_flag from status
	tty_flag = TTY_NORMAL;

	spin_lock_irqsave( &priv->lock, flags );
	line_status = priv->line_status;
	priv->line_status &= ~UART_STATE_TRANSIENT_MASK;
	spin_unlock_irqrestore( &priv->lock, flags );
	wait_flag = 1;  
	wake_up_interruptible( &priv->delta_msr_wait );

	// break takes precedence over parity, 
	// which takes precedence over framing errors
	if( line_status & UART_PARITY_ERROR )
		tty_flag = TTY_PARITY;
	else if( line_status & UART_OVERRUN_ERROR )
		tty_flag = TTY_OVERRUN;
	else if( line_status & UART_FRAME_ERROR )
		tty_flag = TTY_FRAME;
	dbg_ch34x("%s - tty_flag=%d", __func__, tty_flag);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
	tty = port->port.tty; 
#else
	tty = port->tty;
#endif
	if( tty && urb->actual_length ) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,1))
		tty_buffer_request_room( tty->port, urb->actual_length + 1);
#else
		tty_buffer_request_room( tty, urb->actual_length + 1 );
#endif
		// overrun is special, not associated with a char
		if( line_status & UART_OVERRUN_ERROR )
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,1))
			tty_insert_flip_char( tty->port, 0, TTY_OVERRUN );
#else 
			tty_insert_flip_char( tty, 0, TTY_OVERRUN ); 
#endif
		for( i = 0; i < urb->actual_length; ++i )
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,1))
			tty_insert_flip_char( tty->port, data[i], tty_flag );
#else
			tty_insert_flip_char( tty, data[i], tty_flag );
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,1))
		tty_flip_buffer_push( tty->port );
#else
		tty_flip_buffer_push( tty );
#endif
	}

	//Schedule the next read _if_ we are still open
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
	if( port->open_count ) 
#endif
	{
		urb->dev = port->serial->dev;
		retval = usb_submit_urb( urb, GFP_ATOMIC );
		if( retval ) 
			dev_err( &urb->dev->dev, 
					"%s - fialed resubmitting read urb, error %d\n", 
					__func__, retval );
	}

	return;
}

static void ch34x_write_bulk_callback( struct urb *urb )
{
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	struct ch34x_private *priv = usb_get_serial_port_data(port);
	int retval;
	int status = urb->status;

#if(LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
	dbg_ch34x("%s - port:%d", __func__, port->number );
#else
	dbg_ch34x("%s - port:%d", __func__, port->port_number );
#endif
	switch( status ) {
		case 0: //success
			break;
		case -ECONNRESET:
		case -ENOENT:
		case -ESHUTDOWN:
			// this urb is terminated, clean up
			dbg_ch34x("%s - urb shutting down with status:%d", __func__, status);
			priv->write_urb_in_use = 0;
			return;
		default:
			// error in the urb, so we have to resubmit it
			dbg_ch34x("%s - Overflow in write", __func__);
			dbg_ch34x("%s - nonzero write bulk status received:%d", __func__, status);
			port->write_urb->transfer_buffer_length = 1;
			port->write_urb->dev = port->serial->dev;
			retval = usb_submit_urb(port->write_urb, GFP_ATOMIC);
			if( retval )
				dev_err( &urb->dev->dev, 
						"%s - failed resubmitting write urv, error:%d\n",
						__func__, retval );
			else 
				return;
	}

	priv->write_urb_in_use = 0;

	// send any buffered data
	ch34x_send(port);
}

static struct usb_serial_driver	ch34x_device = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "ch34x",
	},
	.id_table	= id_table,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,1))
	.usb_driver	= &ch34x_driver,
#endif
	.num_ports		= 1,
	.open			= ch34x_open,
	.close			= ch34x_close,
	.write			= ch34x_write,
	.ioctl			= ch34x_ioctl,
	.set_termios	= ch34x_set_termios,
	.tiocmget		= ch34x_tiocmget,
	.tiocmset		= ch34x_tiocmset,
	.read_bulk_callback  = ch34x_read_bulk_callback,
	.read_int_callback   = ch34x_read_int_callback,
	.write_bulk_callback = ch34x_write_bulk_callback,
	.write_room     = ch34x_write_room,
	.chars_in_buffer = ch34x_chars_in_buffer,
	.attach			= ch34x_attach,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) )
	.shutdown		= ch34x_shutdown,
#endif
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,5)) 
static struct usb_serial_driver *const serial_driver [] = {
	&ch34x_device, NULL
};
#endif


static int __init ch34x_init(void)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,5)) 
	int retval = 0;

	retval = usb_serial_register( &ch34x_device );
	if( retval ) {
		goto err_usb_serial_register;
	}
	retval = usb_register( &ch34x_driver );
	if( retval ) {
		goto err_usb_register;
	}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32))
	info( DRIVER_DESC );
#endif
	return 0;

err_usb_register:
	usb_deregister( &ch34x_driver );
err_usb_serial_register:
	usb_serial_deregister( &ch34x_device );
	return retval;
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,1))
	return usb_serial_register_drivers( serial_driver, 
			KBUILD_MODNAME, id_table );
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,5) && \
		LINUX_VERSION_CODE < KERNEL_VERSION(3,5,1))
	return usb_serial_register_drivers(&ch34x_driver, serial_driver);
#endif
}

static void __exit ch34x_exit(void)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,5))
	usb_deregister( &ch34x_driver );
	usb_serial_deregister( &ch34x_device );
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,5,1))
	usb_serial_deregister_drivers( serial_driver );
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,5) && \
		LINUX_VERSION_CODE < KERNEL_VERSION(3,5,1))
	usb_serial_deregister_drivers(&ch34x_driver, serial_driver);
#endif
}

module_init( ch34x_init );
module_exit( ch34x_exit );


MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL");
