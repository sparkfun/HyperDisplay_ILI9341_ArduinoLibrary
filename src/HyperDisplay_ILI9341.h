/*

A library to control ILI9341 drivers, derived from the SparkFun HyperDisplay library

Created: August 2020
Authors: Alex Wende

*/

#ifndef HPYERDISPLAY_ILI9341_H
#define HPYERDISPLAY_ILI9341_H


////////////////////////////////////////////////////////////
//							Includes    				  //
////////////////////////////////////////////////////////////
#include "hyperdisplay.h"		// Inherit drawing functions from this library
#include "fast_hsv2rgb.h"		// Used to work with HSV color space		
#include <SPI.h>				// Arduino SPI support

////////////////////////////////////////////////////////////
//							Defines     				  //
////////////////////////////////////////////////////////////
#define ILI9341_MAX_X 240
#define ILI9341_MAX_Y 320
#define ILI9341_START_COL 0
#define ILI9341_STOP_COL 239
#define ILI9341_START_ROW 0
#define ILI9341_STOP_ROW 319
#define ILI9341_MAX_BPP 4




////////////////////////////////////////////////////////////
//							Typedefs    				  //
////////////////////////////////////////////////////////////
typedef enum{
	ILI9341_STAT_Nominal = 0x00,
	ILI9341_STAT_Error
}ILI9341_STAT_t;

typedef enum{
	ILI9341_CMD_NOP = 0x00,
	ILI9341_CMD_SWRST,
	//
	ILI9341_CMD_RDIDI = 0x04,
	//
	ILI9341_CMD_RDSTS = 0x09,
	ILI9341_CMD_RDPWR,
	ILI9341_CMD_RDMADCTL,
	ILI9341_CMD_RDPXFMT,
	ILI9341_CMD_RDIM,
	ILI9341_CMD_RDSM,
	ILI9341_CMD_RDSM2,
	ILI9341_CMD_SLPIN,
	ILI9341_CMD_SLPOUT,
	ILI9341_CMD_PTLON,
	ILI9341_CMD_NMLON,
	// 
	ILI9341_CMD_INVOFF = 0x20,
	ILI9341_CMD_INVON,
	//
	ILI9341_CMD_GAMST = 0x26,
	//
	ILI9341_CMD_OFF = 0x28,
	ILI9341_CMD_ON,
	ILI9341_CMD_CASET,
	ILI9341_CMD_RASET,
	ILI9341_CMD_WRRAM,
	ILI9341_CMD_WRCS,
	// ILI9341_CMD_RDRAM // Not implemented.....?
	ILI9341_CMD_PTLAREA = 0x30,
	//
	ILI9341_CMD_WRVSCRL = 0x33,
	ILI9341_CMD_TELOFF,
	ILI9341_CMD_TELON,
	ILI9341_CMD_WRMADCTL,
	ILI9341_CMD_WRVSSA,
	ILI9341_CMD_IDLOFF,
	ILI9341_CMD_IDLON,
	ILI9341_CMD_WRPXFMT,
	//
	ILI9341_CMD_WRNMLFRCTL = 0xB1,
	ILI9341_CMD_WRIDLFRCTL,
	ILI9341_CMD_WRPTLFRCTL,
	ILI9341_CMD_WRDICTL,
	ILI9341_CMD_WRIBPS,
	ILI9341_CMD_WRDF,
	ILI9341_CMD_WRSDRVDIR,
	ILI9341_CMD_WRGDRVDIR,
	//
	ILI9341_CMD_WRPWCTL1 = 0xC0,
	ILI9341_CMD_WRPWCTL2,
	ILI9341_CMD_WRPWCTL3,
	ILI9341_CMD_WRPWCTL4,
	ILI9341_CMD_WRPWCTL5,
	ILI9341_CMD_WRVCOMCTL1,
	ILI9341_CMD_WRVCOMCTL2,
	ILI9341_CMD_WRVCMOFSTCTL,
	//
	ILI9341_CMD_WRID4 = 0xD3,
	//
	ILI9341_CMD_WRNVMFCTL1 = 0xD5,
	ILI9341_CMD_WRNVMFCTL2,
	ILI9341_CMD_WRNVMFCTL3,
	//
	ILI9341_CMD_RDID1 = 0xDA,
	ILI9341_CMD_RDID2,
	ILI9341_CMD_RDID3,
	//
	ILI9341_CMD_WRPGCS = 0xE0,
	ILI9341_CMD_WRNGCS,
	//
	ILI9341_CMD_WRGAMRS = 0xF2
}ILI9341_CMD_t;

typedef enum{
	ILI9341_INTFC_4WSPI = 0x00,
	ILI9341_INTFC_3WSPI,
	ILI9341_INTFC_8080
}ILI9341_INTFC_t;

typedef enum{
	ILI9341_PXLFMT_16 = 0x05,
	ILI9341_PXLFMT_18 = 0x06
}ILI9341_PXLFMT_t;

typedef struct ILI9341_color_18{
	uint8_t r;
	uint8_t g;
	uint8_t b;
}ILI9341_color_18_t;

typedef struct ILI9341_color_16{
	uint8_t rgh;	// Red, green high
	uint8_t glb;	// Green low, blue
}ILI9341_color_16_t;

typedef struct ILI9341_color_12{
	uint8_t b0;	// Red, green high
	uint8_t b1;	// Green low, blue
}ILI9341_color_12_t;


////////////////////////////////////////////////////////////
//					 Class Definition   				  //
////////////////////////////////////////////////////////////
class ILI9341 : virtual public hyperdisplay{
private:
protected:
	public: // Temporary

	ILI9341_INTFC_t _intfc;
	ILI9341_PXLFMT_t _pxlfmt;

	// Pure virtual functions from HyperDisplay Implemented:
	color_t getOffsetColor(color_t base, uint32_t numPixels);
	void 	hwpixel(hd_hw_extent_t x0, hd_hw_extent_t y0, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0);
    // // Note: only the hwpixel function is implemented at this level because it can be optimized using the writePacket API. Further optimizations are made in further derived classes like 4WSPI (below)
    // virtual void    hwxline(uint16_t x0, uint16_t y0, uint16_t len, color_t data = NULL, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, bool goLeft = false);
    // virtual void    hwyline(uint16_t x0, uint16_t y0, uint16_t len, color_t data = NULL, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, bool goUp = false);
    // virtual void 	hwrectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, bool filled = false, color_t data = NULL, uint16_t colorCycleLength = 1, uint16_t startColorOffset = 0, bool reverseGradient = false, bool gradientVertical = false); 
    // virtual void 	hwfillFromArray(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t numPixels, color_t data);

	void swpixel( hd_extent_t x0, hd_extent_t y0, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0);

public:

	ILI9341(uint8_t xSize, uint8_t ySize, ILI9341_INTFC_t intfc );	// Constructor

	static ILI9341_color_18_t hsvTo18b( uint16_t h, uint8_t s, uint8_t v );
	static ILI9341_color_16_t hsvTo16b( uint16_t h, uint8_t s, uint8_t v );
	static ILI9341_color_12_t hsvTo12b( uint16_t h, uint8_t s, uint8_t v, uint8_t odd);

	static ILI9341_color_18_t rgbTo18b( uint8_t r, uint8_t g, uint8_t b );
	static ILI9341_color_16_t rgbTo16b( uint8_t r, uint8_t g, uint8_t b );
	static ILI9341_color_12_t rgbTo12b( uint8_t r, uint8_t g, uint8_t b, uint8_t odd);

	// Low-level interface functions to be defined in derived classes:
	virtual ILI9341_STAT_t writePacket(ILI9341_CMD_t* pcmd = NULL, uint8_t* pdata = NULL, uint16_t dlen = 0) = 0;		// This function sends any combination of one command and or dlen data byes
	// virtual ILI9341_STAT_t readPacket(ILI9341_CMD_t* pcmd = NULL, uint8_t* pdata = NULL, uint8_t dlen = 0) = 0;

	// Some Utility Functions
	uint8_t getBytesPerPixel( void );


	// Basic Control Functions
	ILI9341_STAT_t swReset( void );
	ILI9341_STAT_t sleepIn( void );
	ILI9341_STAT_t sleepOut( void );
	ILI9341_STAT_t partialModeOn( void );
	ILI9341_STAT_t normalDisplayModeOn( void );
	ILI9341_STAT_t setInversion( bool on );
	ILI9341_STAT_t setPower( bool on );
	ILI9341_STAT_t setColumnAddress( uint16_t start, uint16_t end );
	ILI9341_STAT_t setRowAddress( uint16_t start, uint16_t end );
	ILI9341_STAT_t writeToRAM( uint8_t* pdata, uint16_t numBytes );
	
	
	// Functions to configure the display fully
	ILI9341_STAT_t setMemoryAccessControl( bool mx, bool my, bool mv, bool ml, bool bgr, bool mh );
	ILI9341_STAT_t selectGammaCurve( uint8_t bmNumber );
	ILI9341_STAT_t setPartialArea(uint16_t start, uint16_t end );
	ILI9341_STAT_t setVerticalScrolling( uint16_t tfa, uint16_t vsa, uint16_t bfa );
	ILI9341_STAT_t setVerticalScrollingStartAddress( uint16_t ssa );
	ILI9341_STAT_t setIdleMode( bool on );
	ILI9341_STAT_t setInterfacePixelFormat( uint8_t CTRLintfc );
	ILI9341_STAT_t setTearingEffectLine( bool on );
	ILI9341_STAT_t setNormalFramerate( uint8_t diva, uint8_t vpa );
	ILI9341_STAT_t setIdleFramerate( uint8_t divb, uint8_t vpb );
	ILI9341_STAT_t setPartialFramerate( uint8_t divc, uint8_t vpc );
	ILI9341_STAT_t setPowerControl1( uint8_t vrh, uint8_t vc );
	ILI9341_STAT_t setPowerControl2( uint8_t bt );
	ILI9341_STAT_t setPowerControl3( uint8_t apa );
	ILI9341_STAT_t setPowerControl4( uint8_t apb );
	ILI9341_STAT_t setPowerControl5( uint8_t apc );
	ILI9341_STAT_t setVCOMControl1( uint8_t vmh, uint8_t vml );
	ILI9341_STAT_t setVCOMControl2( uint8_t vma );
	ILI9341_STAT_t setVCOMOffsetControl( bool nVM, uint8_t vmf );
	ILI9341_STAT_t setSrcDriverDir( bool crl );
	ILI9341_STAT_t setGateDriverDir( bool ctb );
	ILI9341_STAT_t setGamRSel( bool gamrsel );
	ILI9341_STAT_t setPositiveGamCorr( uint8_t* gam16byte );
	ILI9341_STAT_t setNegativeGamCorr( uint8_t* gam16byte );
	
	// Functions not yet implemented...
	
	// ILI9341_CMD_WRDICTL
	// ILI9341_CMD_WRIBPS
	// ILI9341_CMD_WRDF

	// ILI9341_CMD_WRID4
	// ILI9341_CMD_WRNVMFCTL1
	// ILI9341_CMD_WRNVMFCTL2
	// ILI9341_CMD_WRNVMFCTL3

	// // Read Functions: (Not Yet Supported)
	// ILI9341_CMD_RDID1
	// ILI9341_CMD_RDID2
	// ILI9341_CMD_RDID3
};




////////////////////////////////////////////////////////////
//				Examples of Derived Classes    			  //
////////////////////////////////////////////////////////////

/*

These classes are included as a demonstration of how to use the
SSD1357 superclass to derive more specific implementations. The
SSD1357 class imposes limits based on the maximum dimensions 
that the driver can handle (128 x 128) and allows for several
different data bus widths and color modes. 

Generally a particular display will contrain the class by the
dimensions and data width. Case study: the UG6464TDDBG01 by
WiseChip. It requires a one-way SPI interface and has only 
64x64 pixels. 

To create a class for this display we first created a derived 
class for one-way SPI that is derived from SSD1357. This 
allowed for definitin of the control pins and a SPI port, as
well as to implement the required (by SSD1357) hooks:
- write_bytes
- selectDrver
- deselectDriver

The Arduino SPI oneway class has now defined the communication
type but has not constrained the display size and location. 
Using a final derived class (UG6464TDDBG01) these parameters 
were built-in to the begin function. The begin function also 
calls functions to set up the required parameters from the 
two main superclasses (hyperdisplay and SSD1357). Additionally
in this final class we implemented the pure-virtual function
'getCharInfo.' This completes the class so that it is no longer
of an abstract type - this means that you can actually
instantiate an object of this class. 

*/


////////////////////////////////////////////////////////////
//				Arduino SPI Oneway Class    			  //
////////////////////////////////////////////////////////////
// Here are a few implementation-specific classes that can be used on the appropirate system
#define ILI9341_SPI_DATA_ORDER MSBFIRST
#define ILI9341_SPI_MODE SPI_MODE0

#define ILI9341_SPI_DEFAULT_FREQ 24000000
#define ILI9341_SPI_MAX_FREQ 	32000000

class ILI9341_4WSPI : public ILI9341{									// General for use with Arduino / SPI with arbitrary display size
private:
protected:

	ILI9341_4WSPI(uint16_t xSize, uint16_t ySize);

	uint8_t _dc, _rst, _cs;		// Pin definitions
	SPIClass * _spi;			// Which SPI port to use
	SPISettings _spisettings;

public:
	ILI9341_STAT_t writePacket(ILI9341_CMD_t* pcmd = NULL, uint8_t* pdata = NULL, uint16_t dlen = 0);
	ILI9341_STAT_t selectDriver( void );
	ILI9341_STAT_t deselectDriver( void );
	ILI9341_STAT_t setSPIFreq( uint32_t freq );
	virtual ILI9341_STAT_t transferSPIbuffer(uint8_t* pdata, size_t count, bool arduinoStillBroken );	// This function is necessary only because Arduino's built-in SPI.transfer() function is broken for one-way transfers. (It overwrites the TX data with whatever was received on RX at the time)


	virtual void 	hwxline(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t len, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0, bool goLeft = false);
	virtual void    hwyline(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t len, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0, bool goUp = false);
	// virtual void 	hwrectangle(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t x1, hd_hw_extent_t y1, bool filled = false, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0, bool reverseGradient = false, bool gradientVertical = false); 
	virtual void 	hwfillFromArray(hd_hw_extent_t x0, hd_hw_extent_t y0, hd_hw_extent_t x1, hd_hw_extent_t y1, color_t data = NULL, hd_pixels_t numPixels = 0, bool Vh = false);

};



#endif /* HPYERDISPLAY_ILI9341_H */