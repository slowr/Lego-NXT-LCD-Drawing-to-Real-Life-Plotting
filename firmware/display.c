#include  "display.h"
#include "AT91SAM7S256.h"

// Display 128 x 64
// 1/65 duty, 1/9 bias
// VLCD 12.0V

typedef   struct
{
  UBYTE   FormatMsb;
  UBYTE   FormatLsb;
  UBYTE   DataBytesMsb;
  UBYTE   DataBytesLsb;
  UBYTE   ItemsX;
  UBYTE   ItemsY;
  UBYTE   ItemPixelsX;
  UBYTE   ItemPixelsY;
  UBYTE   Data[];
} __attribute__((__packed__))
FONT, ICON;

// SPI interface
//
// PCB        LCD       ARM       PIO
// ------     -----     ----      -----
// CS_DIS     -CS1      PA10      NPCS2 (PB)
// DIS_A0     A0        PA12      PA12
// DIS_SCL    SCL       PA14      SPCK  (PA)
// DIS_SDA    SI        PA13      MOSI  (PA)

#define   SPI_BITRATE                   2000000
#define   SPIA0High                     { *AT91C_PIOA_SODR = AT91C_PIO_PA12; }
#define   SPIA0Low                      { *AT91C_PIOA_CODR = AT91C_PIO_PA12; }
#define   SPIInit                       {\
                                          *AT91C_PMC_PCER             = (1L << AT91C_ID_SPI);       /* Enable MCK clock     */\
                                          *AT91C_PIOA_PER             = AT91C_PIO_PA12;             /* Enable A0 on PA12    */\
                                          *AT91C_PIOA_OER             = AT91C_PIO_PA12;\
                                          *AT91C_PIOA_CODR            = AT91C_PIO_PA12;\
                                          *AT91C_PIOA_PDR             = AT91C_PA14_SPCK;            /* Enable SPCK on PA14  */\
                                          *AT91C_PIOA_ASR             = AT91C_PA14_SPCK;\
                                          *AT91C_PIOA_ODR             = AT91C_PA14_SPCK;\
                                          *AT91C_PIOA_OWER            = AT91C_PA14_SPCK;\
                                          *AT91C_PIOA_MDDR            = AT91C_PA14_SPCK;\
                                          *AT91C_PIOA_PPUDR           = AT91C_PA14_SPCK;\
                                          *AT91C_PIOA_IFDR            = AT91C_PA14_SPCK;\
                                          *AT91C_PIOA_CODR            = AT91C_PA14_SPCK;\
                                          *AT91C_PIOA_IDR             = AT91C_PA14_SPCK;\
                                          *AT91C_PIOA_PDR             = AT91C_PA13_MOSI;            /* Enable mosi on PA13  */\
                                          *AT91C_PIOA_ASR             = AT91C_PA13_MOSI;\
                                          *AT91C_PIOA_ODR             = AT91C_PA13_MOSI;\
                                          *AT91C_PIOA_OWER            = AT91C_PA13_MOSI;\
                                          *AT91C_PIOA_MDDR            = AT91C_PA13_MOSI;\
                                          *AT91C_PIOA_PPUDR           = AT91C_PA13_MOSI;\
                                          *AT91C_PIOA_IFDR            = AT91C_PA13_MOSI;\
                                          *AT91C_PIOA_CODR            = AT91C_PA13_MOSI;\
                                          *AT91C_PIOA_IDR             = AT91C_PA13_MOSI;\
                                          *AT91C_PIOA_PDR             = AT91C_PA10_NPCS2;           /* Enable npcs0 on PA11  */\
                                          *AT91C_PIOA_BSR             = AT91C_PA10_NPCS2;\
                                          *AT91C_PIOA_ODR             = AT91C_PA10_NPCS2;\
                                          *AT91C_PIOA_OWER            = AT91C_PA10_NPCS2;\
                                          *AT91C_PIOA_MDDR            = AT91C_PA10_NPCS2;\
                                          *AT91C_PIOA_PPUDR           = AT91C_PA10_NPCS2;\
                                          *AT91C_PIOA_IFDR            = AT91C_PA10_NPCS2;\
                                          *AT91C_PIOA_CODR            = AT91C_PA10_NPCS2;\
                                          *AT91C_PIOA_IDR             = AT91C_PA10_NPCS2;\
                                          *AT91C_SPI_CR               = AT91C_SPI_SWRST;            /* Soft reset           */\
                                          *AT91C_SPI_CR               = AT91C_SPI_SPIEN;            /* Enable spi           */\
                                          *AT91C_SPI_MR               = AT91C_SPI_MSTR  | AT91C_SPI_MODFDIS | (0xB << 16);\
                                          AT91C_SPI_CSR[2]              = ((OSC / SPI_BITRATE) << 8) | AT91C_SPI_CPOL;\
                                        }

#define   SPIWrite(pString,Length)      {\
                                          *AT91C_SPI_TPR = (unsigned int)pString;\
                                          *AT91C_SPI_TCR = (unsigned int)Length;\
                                          *AT91C_SPI_PTCR = AT91C_PDC_TXTEN;\
                                        }
#define   CMD             0
#define   DAT             1
#define   DISP_LINES      8
#define   ACTUAL_WIDTH    100

UBYTE     DisplayInitString[] =
{
  0xEB,   // LCD bias setting = 1/9         0xEB
  0x2F,   // Power control    = internal    0x2F
  0xA4,   // All points not on              0xA4
  0xA6,   // Not inverse                    0xA6
  0x40,   // Start line = 0                 0x40
  0x81,   // Electronic volume              0x81
  0x5A,   //      -"-                       0x5F
  0xC4,   // LCD mapping                    0xC4
  0x27,   // Set temp comp.                 0x27-
  0x29,   // Panel loading                  0x28    0-1
  0xA0,   // Framerate                      0xA0-
  0x88,   // CA++                           0x88-
  0x23,   // Multiplex 1:65                 0x23
  0xAF    // Display on                     0xAF
};

UBYTE     DisplayLineString[DISP_LINES][3] =
{
  { 0xB0,0x10,0x00 },
  { 0xB1,0x10,0x00 },
  { 0xB2,0x10,0x00 },
  { 0xB3,0x10,0x00 },
  { 0xB4,0x10,0x00 },
  { 0xB5,0x10,0x00 },
  { 0xB6,0x10,0x00 },
  { 0xB7,0x10,0x00 }
};

const ICON Font = {
  0x04,0x00, // Graphics Format
  0x02,0x40, // Graphics DataSize
  0x10,      // Graphics Count X
  0x06,      // Graphics Count Y
  0x06,      // Graphics Width
  0x08,      // Graphics Height
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x5F,0x06,0x00,0x00,0x07,0x03,0x00,0x07,0x03,0x00,0x24,0x7E,0x24,0x7E,0x24,0x00,0x24,0x2B,0x6A,0x12,0x00,0x00,0x63,0x13,0x08,0x64,0x63,0x00,0x30,0x4C,0x52,0x22,0x50,0x00,0x00,0x07,0x03,0x00,0x00,0x00,0x00,0x3E,0x41,0x00,0x00,0x00,0x00,0x41,0x3E,0x00,0x00,0x00,0x08,0x3E,0x1C,0x3E,0x08,0x00,0x08,0x08,0x3E,0x08,0x08,0x00,0x80,0x60,0x60,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x60,0x60,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x00,
  0x3E,0x51,0x49,0x45,0x3E,0x00,0x00,0x42,0x7F,0x40,0x00,0x00,0x62,0x51,0x49,0x49,0x46,0x00,0x22,0x49,0x49,0x49,0x36,0x00,0x18,0x14,0x12,0x7F,0x10,0x00,0x2F,0x49,0x49,0x49,0x31,0x00,0x3C,0x4A,0x49,0x49,0x30,0x00,0x01,0x71,0x09,0x05,0x03,0x00,0x36,0x49,0x49,0x49,0x36,0x00,0x06,0x49,0x49,0x29,0x1E,0x00,0x00,0x6C,0x6C,0x00,0x00,0x00,0x00,0xEC,0x6C,0x00,0x00,0x00,0x08,0x14,0x22,0x41,0x00,0x00,0x24,0x24,0x24,0x24,0x24,0x00,0x00,0x41,0x22,0x14,0x08,0x00,0x02,0x01,0x59,0x09,0x06,0x00,
  0x3E,0x41,0x5D,0x55,0x1E,0x00,0x7E,0x11,0x11,0x11,0x7E,0x00,0x7F,0x49,0x49,0x49,0x36,0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x7F,0x41,0x41,0x41,0x3E,0x00,0x7F,0x49,0x49,0x49,0x41,0x00,0x7F,0x09,0x09,0x09,0x01,0x00,0x3E,0x41,0x49,0x49,0x7A,0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x00,0x41,0x7F,0x41,0x00,0x00,0x30,0x40,0x40,0x40,0x3F,0x00,0x7F,0x08,0x14,0x22,0x41,0x00,0x7F,0x40,0x40,0x40,0x40,0x00,0x7F,0x02,0x04,0x02,0x7F,0x00,0x7F,0x02,0x04,0x08,0x7F,0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,
  0x7F,0x09,0x09,0x09,0x06,0x00,0x3E,0x41,0x51,0x21,0x5E,0x00,0x7F,0x09,0x09,0x19,0x66,0x00,0x26,0x49,0x49,0x49,0x32,0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x3F,0x40,0x40,0x40,0x3F,0x00,0x1F,0x20,0x40,0x20,0x1F,0x00,0x3F,0x40,0x3C,0x40,0x3F,0x00,0x63,0x14,0x08,0x14,0x63,0x00,0x07,0x08,0x70,0x08,0x07,0x00,0x71,0x49,0x45,0x43,0x00,0x00,0x00,0x7F,0x41,0x41,0x00,0x00,0x02,0x04,0x08,0x10,0x20,0x00,0x00,0x41,0x41,0x7F,0x00,0x00,0x04,0x02,0x01,0x02,0x04,0x00,0x80,0x80,0x80,0x80,0x80,0x00,
  0x00,0x02,0x05,0x02,0x00,0x00,0x20,0x54,0x54,0x54,0x78,0x00,0x7F,0x44,0x44,0x44,0x38,0x00,0x38,0x44,0x44,0x44,0x28,0x00,0x38,0x44,0x44,0x44,0x7F,0x00,0x38,0x54,0x54,0x54,0x08,0x00,0x08,0x7E,0x09,0x09,0x00,0x00,0x18,0x24,0xA4,0xA4,0xFC,0x00,0x7F,0x04,0x04,0x78,0x00,0x00,0x00,0x00,0x7D,0x40,0x00,0x00,0x40,0x80,0x84,0x7D,0x00,0x00,0x7F,0x10,0x28,0x44,0x00,0x00,0x00,0x00,0x7F,0x40,0x00,0x00,0x7C,0x04,0x18,0x04,0x78,0x00,0x7C,0x04,0x04,0x78,0x00,0x00,0x38,0x44,0x44,0x44,0x38,0x00,
  0xFC,0x44,0x44,0x44,0x38,0x00,0x38,0x44,0x44,0x44,0xFC,0x00,0x44,0x78,0x44,0x04,0x08,0x00,0x08,0x54,0x54,0x54,0x20,0x00,0x04,0x3E,0x44,0x24,0x00,0x00,0x3C,0x40,0x20,0x7C,0x00,0x00,0x1C,0x20,0x40,0x20,0x1C,0x00,0x3C,0x60,0x30,0x60,0x3C,0x00,0x6C,0x10,0x10,0x6C,0x00,0x00,0x9C,0xA0,0x60,0x3C,0x00,0x00,0x64,0x54,0x54,0x4C,0x00,0x00,0x08,0x3E,0x41,0x41,0x00,0x00,0x00,0x00,0x77,0x00,0x00,0x00,0x00,0x41,0x41,0x3E,0x08,0x00,0x02,0x01,0x02,0x01,0x00,0x00,0x10,0x20,0x40,0x38,0x07,0x00}
};

static struct {
	UBYTE   *Display;
	UBYTE   DataArray[DISPLAY_HEIGHT / 8][DISPLAY_WIDTH];
} IOMapDisplay;

void      DisplayInit(void) { 
	SPIInit; 
	IOMapDisplay.Display  =  (UBYTE*)IOMapDisplay.DataArray;
}

void      DisplayOn(UBYTE On)
{
  if (On) { 
	DisplayInitString[6]  = 0x5A;\
	DisplayInitString[13] = 0xAF;\
  }
  else { 
	DisplayInitString[6]  = 0x00;\
	DisplayInitString[13] = 0xAE;\
  }
}

void      DisplayExit(void) { ; }

UBYTE     DisplayWrite(UBYTE Type,UBYTE *pData,UWORD Length)
{
  UBYTE   Result = FALSE;

  if ((*AT91C_SPI_SR & AT91C_SPI_TXEMPTY))
  {
    if (Type)
    {
      SPIA0High;
    }
    else
    {
      SPIA0Low;
    }
    SPIWrite(pData,Length);
    Result = TRUE;
  }  

  return (Result);
}

UBYTE     DisplayUpdateAsync(void)
{
  static  UWORD State = 0;
  static  UWORD Line;
  UBYTE *pImage = (UBYTE*)IOMapDisplay.Display;
  UWORD Height = DISPLAY_HEIGHT;
  UWORD Width = DISPLAY_WIDTH;

  if (State == 0)
  {
    if (DisplayWrite(CMD,(UBYTE*)DisplayInitString,sizeof(DisplayInitString)) == TRUE)
    {
      Line = 0;
      State++;
    }
  }
  else
  {
    if ((State & 1))
    {
      if (DisplayWrite(CMD,(UBYTE*)DisplayLineString[Line],3) == TRUE)
      {
        State++;
      }
    }
    else
    {
      if (DisplayWrite(DAT,(UBYTE*)&pImage[Line * Width],ACTUAL_WIDTH) == TRUE)
      {
        State++;
        if (++Line >= (Height / 8))
        {
          State = 0;
        }
      }
    }
  }
  return (State);
}

void DisplayUpdateSync(void)
{
	while(DisplayUpdateAsync()){/* spin */;};
}

void      DisplaySetPixel(UBYTE X,UBYTE Y)
{
  if ((X < DISPLAY_WIDTH) && (Y < DISPLAY_HEIGHT))
  {
    IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] |= (1 << (Y % 8));
  }
}

void      DisplayClrPixel(UBYTE X,UBYTE Y)
{
  if ((X < DISPLAY_WIDTH) && (Y < DISPLAY_HEIGHT))
  {
    IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] &= ~(1 << (Y % 8));
  }
}

void      DisplayLineX(UBYTE X1,UBYTE X2,UBYTE Y)
{
  UBYTE   X;
  UBYTE   M;

  M   = 1 << (Y % 8);
  Y >>= 3;
  for (X = X1;X < X2;X++)
  {
    IOMapDisplay.Display[Y * DISPLAY_WIDTH + X] |= M;
  }
}

void      DisplayLineY(UBYTE X,UBYTE Y1,UBYTE Y2)
{
  UBYTE   Y;

  for (Y = Y1;Y < Y2;Y++)
  {
    IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] |= (1 << (Y % 8));
  }
}

void      DisplayEraseLine(UBYTE Line)
{
  UBYTE   Tmp;

  for (Tmp = 0;Tmp < DISPLAY_WIDTH;Tmp++)
  {
    IOMapDisplay.Display[Line * DISPLAY_WIDTH + Tmp] = 0x00;
  }
}

void      DisplayErase(void)
{
  UBYTE   Tmp;

  for (Tmp = 0;Tmp < (DISPLAY_HEIGHT / 8);Tmp++)
  {
    DisplayEraseLine(Tmp);
  }
}

void      DisplayChar(UBYTE On,UBYTE X,UBYTE Y,UBYTE Char)
{
  UBYTE   *pSource;
  UBYTE   FontWidth;
  UBYTE   FontHeight;
  UBYTE   Items;
  UBYTE   Item;
  UBYTE   TmpY;
  const	  ICON *pFont = &Font;

  Items          = pFont->ItemsX * pFont->ItemsY;
  Item           = Char - ' ';
  if (Item < Items)
  {
    FontWidth    = pFont->ItemPixelsX;
    pSource      = (UBYTE*)&pFont->Data[Item * FontWidth];
    while (FontWidth--)
    {
      TmpY       = 0;
      FontHeight = pFont->ItemPixelsY;
      while (FontHeight--)
      {
        if (On == TRUE)
        {
          if (((*pSource) & (1 << TmpY)))
          {
            DisplaySetPixel(X,Y + TmpY);
          }
          else
          {
            DisplayClrPixel(X,Y + TmpY);
          }
        }
        else
        {
          if (((*pSource) & (1 << TmpY)))
          {
            DisplayClrPixel(X,Y + TmpY);
          }
          else
          {
            DisplaySetPixel(X,Y + TmpY);
          }
        }
        TmpY++;
      }
      X++;
      pSource++;
    }
  }
}

void DisplaySignedNum(UBYTE On,UBYTE X,UBYTE Y, SWORD Num){
  if(Num < 0){
    DisplayString(X,Y,(UBYTE *)"-");
    DisplayNum(On,X+8,Y,-Num);
  }else{
    DisplayNum(On,X,Y,Num);
  }
}

void DisplayNum(UBYTE On,UBYTE X,UBYTE Y,UWORD Num)
{	
    UWORD t1=0;
    UBYTE t2=0,ch=0;
    static UBYTE digits=0,i=0;
    t1=Num;
    t2=t1%10;
    t1=(t1-t2)/10;
    if(t1>0){
	    digits++;
      DisplayNum(On,X,Y,t1);
    } 
    else {
	     i=digits;
    }
    ch='0'+t2;
    DisplayChar(On,X+(6*(digits-i--)),Y,ch);    
}

void      DisplayString(UBYTE X,UBYTE Y,UBYTE *pString)
{
  UBYTE   *pSource;
  UBYTE   *pDestination;
  UBYTE   FontWidth;
  UBYTE   Line;
  UBYTE   Items;
  UBYTE   Item;
  const	  ICON *pFont = &Font;

  Line         = (Y & 0xF8) / 8;
  Items        = pFont->ItemsX * pFont->ItemsY;
  pDestination = (UBYTE*)&IOMapDisplay.Display[Line * DISPLAY_WIDTH + X];

  while (*pString)
  {
    Item           = *pString - ' ';
    if (Item < Items)
    {
      FontWidth    = pFont->ItemPixelsX;
      pSource      = (UBYTE*)&pFont->Data[Item * FontWidth];
      while (FontWidth--)
      {
        *pDestination = *pSource;
        pDestination++;
        pSource++;
      }
    }
    pString++;
  }
}

void printHexWord(UBYTE x, UBYTE y, UWORD word)
{
  char digit[9] = {0};
  int i;

  digit[0] = (char)((word & 0xf0000000) >> 28);
  digit[1] = (char)((word & 0x0f000000) >> 24);
  digit[2] = (char)((word & 0x00f00000) >> 20);
  digit[3] = (char)((word & 0x000f0000) >> 16);
  digit[4] = (char)((word & 0x0000f000) >> 12);
  digit[5] = (char)((word & 0x00000f00) >> 8);
  digit[6] = (char)((word & 0x000000f0) >> 4);
  digit[7] = (char)((word & 0x0000000f) >> 0);

  for (i = 0; i < 8; i++)
    digit[i] = (digit[i] < 10) ? 0x30 + digit[i] : 0x41 + digit[i] - 10;

  //DisplayErase();
  //DisplayOn(1);
  DisplayString(x,y,digit);
  //DisplayUpdateSync();

}

void DisplayLineXY(UBYTE X1, UBYTE Y1, UBYTE X2, UBYTE Y2)
{
    UBYTE   i, x, y;
    UBYTE page1 = Y1 / 8;
    UBYTE page2 = Y2 / 8;
    UBYTE offs1 = Y1 % 8;
    UBYTE offs2 = Y2 % 8;
    
    UBYTE page, offset;
    
    SBYTE sgnX = (X2 - X1 < 0) ? -1 : 1;
    SBYTE sgnY = (Y2 - Y1 < 0) ? -1 : 1;
    SBYTE dX = (sgnX > 0) ? X2 - X1 : X1 - X2;
    SBYTE dY = (sgnY > 0) ? Y2 - Y1 : Y1 - Y2;
    
    UBYTE pixels;
    
    pixels = (dX > dY) ? dX : dY;
    
    for (i = 0; i < pixels; i++) {
        
        x = X1 + sgnX*(i*dX)/pixels;
        y = Y1 + sgnY*(i*dY)/pixels;
        
        page = y / 8;
        offset = y % 8;
        
        IOMapDisplay.DataArray[page][x] |= (1 << offset);
    }
    
#if (0)
    if (page1 == page2) {
        for (i = offs1; i <= offs2; i++)
            IOMapDisplay.DataArray[page1][X] = IOMapDisplay.DataArray[page1][X] | (1 << i);
    } else {
        for (i = offs1; i < 8; i++)
            IOMapDisplay.DataArray[page1][X] = IOMapDisplay.DataArray[page1][X] | (1 << i);
        for (i = page1 + 1; i < page2; i++)
            IOMapDisplay.DataArray[i][X] = 0xFF;
        for (i = 0; i <= offs2; i++)
            IOMapDisplay.DataArray[page1][X] = IOMapDisplay.DataArray[page1][X] | (1 << i);
    }
 
    
    UBYTE i;
    UBYTE page = Y / 8;
    UBYTE pixel = 1 << (Y % 8);
    
    for (i = X1; i <= X2; i++)
        IOMapDisplay.DataArray[page][i] = IOMapDisplay.DataArray[page][i] | pixel;
#endif
   
}

void DisplayCross(UBYTE X, UBYTE Y)
{
    UBYTE cross[15][15] =  {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
        {0,0,0,0,0,1,0,0,0,1,0,0,0,0,0},
        {0,1,1,1,1,1,0,0,0,1,1,1,1,1,0},
        {0,0,0,0,0,1,0,0,0,1,0,0,0,0,0},
        {0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
    
    SBYTE i, j;
    UBYTE page, offset;
    
    for (i = -7; i <= 7; i++)
        for (j = -7; j <= 7; j++) {
            if (X + i < 0 || Y + j < 0 || X + i >= 100 || Y + j >= 64) continue;
            
            page = (Y + j) / 8;
            offset = (Y + j) % 8;
            
            IOMapDisplay.DataArray[page][X + i] |= (cross[j + 7][i + 7] << offset);
        }
    
}
