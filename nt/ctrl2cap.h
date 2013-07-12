//======================================================================
// 
// ctrl2cap.h
//
//  This module contains the PRIVATE (driver-only) definitions for the
//  code that implements the mono device driver.
//
//======================================================================


// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.
#define FILE_DEVICE_CTRL2CAP	0x00008300

//
// Define scan codes of interest here. For scan code tables, see 
// "The Undocumented PC", by Frank Van Gullwe, Addison-Wesley 1994.
//

#define KEY_UP         1
#define KEY_DOWN       0

#define ESC		((USHORT)1)

#define F1		((USHORT)59)
#define F2		((USHORT)60)
#define F3		((USHORT)61)
#define F4		((USHORT)62)
#define F5		((USHORT)63)
#define F6		((USHORT)64)
#define F7		((USHORT)65)
#define F8		((USHORT)66)
#define F9		((USHORT)67)
#define F10		((USHORT)68)
#define F11		((USHORT)87)
#define F12		((USHORT)89)

#define PRSCR		((USHORT)1)
#define PGUP		((USHORT)73)
        
#define BCKSLSH		((USHORT)43)
#define ALTL		((USHORT)56)
#define ALTR		((USHORT)56)
#define CTRLL		((USHORT)29)
#define CTRLR		((USHORT)29)
        
#define TILDA		((USHORT)41)
        
#define SCRLCK		((USHORT)70)
#define PAUSE		((USHORT)69)
#define UP		((USHORT)72)
#define DEL		((USHORT)83)
#define END		((USHORT)79)
#define INS		((USHORT)82)

#define RIGHT		((USHORT)77)
#define PGDN		((USHORT)81)
#define HOME		((USHORT)71)

#define CPSLCK		((USHORT)58)
#define DOWN		((USHORT)80)
#define LEFT		((USHORT)75)
#define NUMLCK		((USHORT)69)
#define SLASH		((USHORT)53)
#define STAR		((USHORT)55)
#define MINUS		((USHORT)74)
#define PLUS		((USHORT)78)
#define ENTER		((USHORT)28)


static unsigned char IBM_keys[128] = {
  0, NUMLCK, 0, 0, 0, 0, 0, 0,				/* 0x00-0x07 */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x08-0x0f */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x10-0x17 */
  0, 0, 0, 0, 0, CPSLCK, 0, 0,				/* 0x18-0x1f */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x20-0x27 */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x28-0x2f */

  0, 0, 0, 0, 0, 0, 0, PLUS,				/* 0x30-0x37 */
  CTRLL, 0, CTRLR, 0, 0, 0, 0, 0,			/* 0x38-0x3f */
  0, 0, 0, 0, 0, SLASH, STAR, 0,			/* 0x40-0x47 */
  0, 0, 0, 0, 0, 0, ENTER, 0,				/* 0x48-0x4f */
  0, 0, 0, 0, MINUS, DOWN, LEFT, 0,			/* 0x50-0x57 */
  F1, F2, F3, F4, F5, F6, F7, F8,			/* 0x58-0x5f */

  F9, F10, F11, F12, ESC, 0, 0, 0,			/* 0x60-0x67 */
  0, 0, 0, 0, 0, 0, PRSCR, PGUP,			/* 0x68-0x6f */
  BCKSLSH, ALTL, ALTR, 0, TILDA, 0, SCRLCK, PAUSE,	/* 0x70-0x77 */
  UP, DEL, END, INS, 0, RIGHT, PGDN, HOME		/* 0x78-0x7f */
};

static unsigned int IBM_e0_keys[128] = {
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x00-0x07 */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x08-0x0f */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x10-0x17 */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x18-0x1f */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x20-0x27 */
  0, 0, 0, 0, 0, 0, 0, 0,				/* 0x28-0x2f */

  0, 0, 0, 0, 0, 0, 0, PLUS,				/* 0x30-0x37 */
  CTRLL, 0, CTRLR, 0, 0, 0, 0, 0,			/* 0x38-0x3f */
  0, 0, 0, 0, 0, SLASH, STAR, 0,			/* 0x40-0x47 */
  0, 0, 0, 0, 0, 0, ENTER, 0,				/* 0x48-0x4f */
  0, 0, 0, 0, MINUS, DOWN, LEFT, 0,			/* 0x50-0x57 */
  F1, F2, F3, F4, F5, F6, F7, F8,			/* 0x58-0x5f */

  F9, F10, F11, F12, ESC, 0, 0, 0,			/* 0x60-0x67 */
  0, 0, 0, 0, 0, 0, PRSCR, PGUP,			/* 0x68-0x6f */
  BCKSLSH, ALTL, ALTR, 0, TILDA, 0, SCRLCK, PAUSE,	/* 0x70-0x77 */
  UP, DEL, END, INS, 0, RIGHT, PGDN, HOME		/* 0x78-0x7f */
};
