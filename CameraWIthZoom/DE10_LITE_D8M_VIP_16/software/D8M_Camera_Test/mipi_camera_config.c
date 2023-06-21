/*
 * config_reg.c
 *
 *  Created on: 2015/3/27
 *      Author: User
 */

#include <stdio.h>
#include "I2C_core.h"
#include "terasic_includes.h"
#include "system.h"
#include "mipi_camera_config.h"
#include "auto_focus.h"




#define Sleep(x)	usleep(x*1000)
#define OV8865DB(x) printf(x)

typedef struct{
	alt_u8 Type;
	alt_u16 Addr;
	alt_u8 Data;
}SZ_CONFIG_T;


#define REG_8BIT 1
#define REG_16BIT 2
#define TIME_DELAY 3
#define END_OF_SCRIPT 4

static SZ_CONFIG_T MipiCameraReg[] = {
	     {0x6c,0x0103, 0x01}, // software reset
	  		{TIME_DELAY, 0, 10},
	     {0x6c,0x0100, 0x00}, // software standby
	     {0x6c,0x0100, 0x00}, // software standby
	     {0x6c,0x0100, 0x00}, // software standby
	     {0x6c,0x0100, 0x00}, // software standby
	  		{TIME_DELAY, 0, 10},

	     {0x6c,0x3638, 0xff}, // analog control
  // 25MHz MCLK input
  // PHY_CLK : 600 MHz (data rate,not clock rate)
  // PCLK    : 75  MHz
  // SCLK    : 150 MHz
	     {0x6c,0x0302, 24}, // PLL pll1_multiplier
	     {0x6c,0x0303, 0x00}, // PLL pll1_divm1+pll1_divm
	     {0x6c,0x0304, 3}, // PLL pll1_div_mipi

	     {0x6c,0x030e, 0x00}, // PLL pll2_r_divs : /1
	     {0x6c,0x030f, 0x04}, // PLL  1 + pll2_r_divsp
	     {0x6c,0x0312, 0x01}, // PLL pll2_pre_div0: /1,pll2_r_divdac1+pll2_r_divdac
	     {0x6c,0x031e, 0x0c}, // PLL

	     {0x6c,0x3015, 0x01}, // clock Div
	     {0x6c,0x3018, 0x72}, // MIPI 4 lane
	     {0x6c,0x3020, 0x93}, // clock normal, pclk/1
	     {0x6c,0x3022, 0x01}, // pd_mini enable when rst_sync
	     {0x6c,0x3031, 0x0a}, // 10-bit
	     {0x6c,0x3106, 0x01}, // PLL
	     {0x6c,0x3305, 0xf1},
	     {0x6c,0x3308, 0x00},
	     {0x6c,0x3309, 0x28},
	     {0x6c,0x330a, 0x00},
	     {0x6c,0x330b, 0x20},
	     {0x6c,0x330c, 0x00},
	     {0x6c,0x330d, 0x00},
	     {0x6c,0x330e, 0x00},
	     {0x6c,0x330f, 0x40},
	     {0x6c,0x3307, 0x04},
	     {0x6c,0x3604, 0x04}, // analog control
	     {0x6c,0x3602, 0x30},
	     {0x6c,0x3605, 0x00},
	     {0x6c,0x3607, 0x20},
	     {0x6c,0x3608, 0x11},
	     {0x6c,0x3609, 0x68},
	     {0x6c,0x360a, 0x40},
	     {0x6c,0x360c, 0xdd},
	     {0x6c,0x360e, 0x0c},
	     {0x6c,0x3610, 0x07},
	     {0x6c,0x3612, 0x86},
	     {0x6c,0x3613, 0x58},
	     {0x6c,0x3614, 0x28},
	     {0x6c,0x3617, 0x40},
	     {0x6c,0x3618, 0x5a},
	     {0x6c,0x3619, 0x9b},
	     {0x6c,0x361c, 0x00},
	     {0x6c,0x361d, 0x60},
	     {0x6c,0x3631, 0x60},
	     {0x6c,0x3633, 0x10},
	     {0x6c,0x3634, 0x10},
	     {0x6c,0x3635, 0x10},
	     {0x6c,0x3636, 0x10},
	     {0x6c,0x3641, 0x55}, // MIPI settings
	     {0x6c,0x3646, 0x86}, // MIPI settings
	     {0x6c,0x3647, 0x27}, // MIPI settings
	     {0x6c,0x364a, 0x1b}, // MIPI settings

	     // exposure
//	     {0x6c,0x3500, 0x00}, // exposure HH
	     {0x6c,0x3501, 0x18}, // exposure H
//	     {0x6c,0x3502, 0x60}, // exposure L
	     {0x6c,0x3500, 0x00}, // exposure HH
	     {0x6c,0x3501, 0x2C}, // exposure H
	     {0x6c,0x3502, 0x00}, // exposure L

	     {0x6c,0x3503, 0x00}, // gain no delay, exposure no delay
	     {0x6c,0x3508, 0x05}, // gain H
	     {0x6c,0x3509, 0x00}, // gain L
	     {0x6c,0x3700, 0x48}, // sensor control  // TODO: to check
	     {0x6c,0x3701, 0x18},
	     {0x6c,0x3702, 0x50},
	     {0x6c,0x3703, 0x32},
	     {0x6c,0x3704, 0x28},
	     {0x6c,0x3705, 0x00},
	     {0x6c,0x3706, 0x70},
	     {0x6c,0x3707, 0x08},
	     {0x6c,0x3708, 0x48},
	     {0x6c,0x3709, 0x80},
	     {0x6c,0x370a, 0x01},
	     {0x6c,0x370b, 0x70},
	     {0x6c,0x370c, 0x07},
	     {0x6c,0x3718, 0x14},
	     {0x6c,0x3719, 0x31},
	     {0x6c,0x3712, 0x44},
	     {0x6c,0x3714, 0x12},
	     {0x6c,0x371e, 0x31},
	     {0x6c,0x371f, 0x7f},
	     {0x6c,0x3720, 0x0a},
	     {0x6c,0x3721, 0x0a},
	     {0x6c,0x3724, 0x04},
	     {0x6c,0x3725, 0x04},
	     {0x6c,0x3726, 0x0c},
	     {0x6c,0x3728, 0x0a},
	     {0x6c,0x3729, 0x03},
	     {0x6c,0x372a, 0x06},
	     {0x6c,0x372b, 0xa6},
	     {0x6c,0x372c, 0xa6},
	     {0x6c,0x372d, 0xa6},
	     {0x6c,0x372e, 0x0c},
	     {0x6c,0x372f, 0x20},
	     {0x6c,0x3730, 0x02},
	     {0x6c,0x3731, 0x0c},
	     {0x6c,0x3732, 0x28},
	     {0x6c,0x3733, 0x10},
	     {0x6c,0x3734, 0x40},
	     {0x6c,0x3736, 0x30},
	     {0x6c,0x373a, 0x04},
	     {0x6c,0x373b, 0x18},
	     {0x6c,0x373c, 0x14},
	     {0x6c,0x373e, 0x06},
	     {0x6c,0x3755, 0x40},
	     {0x6c,0x3758, 0x00},
	     {0x6c,0x3759, 0x4c},
	     {0x6c,0x375a, 0x0c},
	     {0x6c,0x375b, 0x26},
	     {0x6c,0x375c, 0x20},
	     {0x6c,0x375d, 0x04},
	     {0x6c,0x375e, 0x00},
	     {0x6c,0x375f, 0x28},
	     {0x6c,0x3767, 0x04},
	     {0x6c,0x3768, 0x04},
	     {0x6c,0x3769, 0x20},
	     {0x6c,0x376c, 0x00},
	     {0x6c,0x376d, 0x00},
	     {0x6c,0x376a, 0x08},
	     {0x6c,0x3761, 0x00},
	     {0x6c,0x3762, 0x00},
	     {0x6c,0x3763, 0x00},
	     {0x6c,0x3766, 0xff},
	     {0x6c,0x376b, 0x42},
	     {0x6c,0x3772, 0x46},
	     {0x6c,0x3773, 0x04},
	     {0x6c,0x3774, 0x2c},
	     {0x6c,0x3775, 0x13},
	     {0x6c,0x3776, 0x10},
	     {0x6c,0x37a0, 0x88},
	     {0x6c,0x37a1, 0x7a},
	     {0x6c,0x37a2, 0x7a},
	     {0x6c,0x37a3, 0x02},
	     {0x6c,0x37a4, 0x00},
	     {0x6c,0x37a5, 0x09},
	     {0x6c,0x37a6, 0x00},
	     {0x6c,0x37a7, 0x88},
	     {0x6c,0x37a8, 0xb0},
	     {0x6c,0x37a9, 0xb0},
	     {0x6c,0x3760, 0x00},
	     {0x6c,0x376f, 0x01},
	     {0x6c,0x37aa, 0x88},
	     {0x6c,0x37ab, 0x5c},
	     {0x6c,0x37ac, 0x5c},
	     {0x6c,0x37ad, 0x55},
	     {0x6c,0x37ae, 0x19},
	     {0x6c,0x37af, 0x19},
	     {0x6c,0x37b0, 0x00},
	     {0x6c,0x37b1, 0x00},
	     {0x6c,0x37b2, 0x00},
	     {0x6c,0x37b3, 0x84},
	     {0x6c,0x37b4, 0x84},
	     {0x6c,0x37b5, 0x66},
	     {0x6c,0x37b6, 0x00},
	     {0x6c,0x37b7, 0x00},
	     {0x6c,0x37b8, 0x00},
	     {0x6c,0x37b9, 0xff}, // sensor control
	     // don't care , use auto size
//	     {0x6c,0x3800, 0x00}, // X start H
//	     {0x6c,0x3801, 0x0c}, // X start L
//	     {0x6c,0x3802, 0x00}, // Y start H
//	     {0x6c,0x3803, 0x0c}, // Y start L
//	     {0x6c,0x3804, 0x0c}, // X end H
//	     {0x6c,0x3805, 0xd3}, // X end L
//	     {0x6c,0x3806, 0x09}, // Y end H
//	     {0x6c,0x3807, 0xa3}, // Y end L


//	     // 800x600
//	     {0x6c,0x3808, 0x03}, // X output size H
//	     {0x6c,0x3809, 0x20}, // X output size L
//	     {0x6c,0x380a, 0x02}, // Y output size H
//	     {0x6c,0x380b, 0x58}, // Y output size L

	     // 640x480
	     {0x6c,0x3808, 0x02}, // X output size H
	     {0x6c,0x3809, 0x80}, // X output size L
	     {0x6c,0x380a, 0x01}, // Y output size H
	     {0x6c,0x380b, 0xE0}, // Y output size L



	     // 60 fps (combined with pll settings)
	     {0x6c,0x380c, 0x12}, // HTS H
	     {0x6c,0x380d, 0x00}, // HTS L
	     {0x6c,0x380e, 0x02}, // VTS H
	     {0x6c,0x380f, 0x1E}, // VTS L



	     {0x6c,0x3810, 0x00}, // ISP X win H
	     {0x6c,0x3811, 0x04}, // ISP X win L
	     {0x6c,0x3813, 0x02}, // ISP Y win L

	     {0x6c,0x3814, 0x01}, // X inc odd
	     {0x6c,0x3815, 0x01}, // X inc even
	     {0x6c,0x3820, 0x06}, // flip on
	     {0x6c,0x3821, 0x70}, // hsync_en_o, fst_vbin, mirror on
	     {0x6c,0x382a, 0x01}, // Y inc odd
	     {0x6c,0x382b, 0x01}, // Y inc even
	     {0x6c,0x3830, 8}, // ablc_use_num[5:1]
	     {0x6c,0x3836, 2}, // zline_use_num[5:1]
	     {0x6c,0x3837, 0x18}, // vts_add_dis, cexp_gt_vts_offs=8
	     {0x6c,0x3841, 0xff}, // auto size
	     {0x6c,0x3846, 0x48}, // Y/X boundary pixel number for auto size mode
	     {0x6c,0x3f08, 0x16},
	     {0x6c,0x4000, 0xf1}, // our range trig en, format chg en, gan chg en, exp chg en, median en
	     {0x6c,0x4001, 0x04}, // left 32 column, final BLC offset limitation enable
	     {0x6c,0x4005, 0x10}, // BLC target
	     {0x6c,0x400b, 0x0c}, // start line =0, offset limitation en, cut range function en
	     {0x6c,0x400d, 0x10}, // offset trigger threshold
	     {0x6c,0x4011, 0x30},
	     {0x6c,0x4013, 0xcf},
	     {0x6c,0x401b, 0x00},
	     {0x6c,0x401d, 0x00},
	     {0x6c,0x4020, 0x02}, // anchor left start H
	     {0x6c,0x4021, 0x40}, // anchor left start L
	     {0x6c,0x4022, 0x03}, // anchor left end H
	     {0x6c,0x4023, 0x3f}, // anchor left end L
	     {0x6c,0x4024, 0x07}, // anchor right start H
	     {0x6c,0x4025, 0xc0}, // anchor right start L
	     {0x6c,0x4026, 0x08}, // anchor right end H
	     {0x6c,0x4027, 0xbf}, // anchor right end L
	     {0x6c,0x4028, 0x00}, // top zero line start
	     {0x6c,0x4029, 0x02}, // top zero line number
	     {0x6c,0x402a, 0x04}, // top black line start
	     {0x6c,0x402b, 0x04}, // top black line number
	     {0x6c,0x402c, 0x02}, // bottom zero line start
	     {0x6c,0x402d, 0x02}, // bottom zero line number
	     {0x6c,0x402e, 0x08}, // bottom black line start
	     {0x6c,0x402f, 0x02}, // bottom black line number
	     {0x6c,0x401f, 0x00}, // anchor one disable
	     {0x6c,0x4034, 0x3f}, // limitation BLC offset
	     {0x6c,0x4300, 0xff}, // clip max H
	     {0x6c,0x4301, 0x00}, // clip min H
	     {0x6c,0x4302, 0x0f}, // clip min L/clip max L
	     {0x6c,0x4500, 0x68}, // ADC sync control

	     {0x6c,0x4503, 0x10},
	     {0x6c,0x4601, 0x10}, // V FIFO control

	     // clock prepare  50+ Tui*ui_clk_prepare_min(0) : 50 ns
	     {0x6c,0x481f, 70}, // clk_prepare_min

	     {0x6c,0x4837, 0x16}, // clock period
	     {0x6c,0x4850, 0x10}, // lane select
	     {0x6c,0x4851, 0x32}, // lane select
	     {0x6c,0x4b00, 0x2a}, // LVDS settings
	     {0x6c,0x4b0d, 0x00}, // LVDS settings
	     {0x6c,0x4d00, 0x04}, // temperature sensor
	     {0x6c,0x4d01, 0x18}, // temperature sensor
	     {0x6c,0x4d02, 0xc3}, // temperature sensor
	     {0x6c,0x4d03, 0xff}, // temperature sensor
	     {0x6c,0x4d04, 0xff}, // temperature sensor
	     {0x6c,0x4d05, 0xff}, // temperature sensor
	     {0x6c,0x5000, 0x16}, // LENC on, MWB on, BPC on, WPC on
	     {0x6c,0x5001, 0x01}, // BLC on
	     {0x6c,0x5002, 0x08}, // vario pixel off
	     {0x6c,0x5901, 0x00},

	     {0x6c,0x5e00, 0x00}, // test pattern off


	     {0x6c,0x5018, 0x15}, // Red MWB gain
	     {0x6c,0x501A, 0x15}, // Green MWB gain
	     {0x6c,0x501C, 0x15}, // Blue MWB gain


#if 0  // test pattern
	     {0x6c,0x5e00, 0x82}, // test pattern on
#endif
	     {0x6c,0x5e01, 0x41}, // window cut enable


	     {TIME_DELAY, 0, 10},

	     {0x6c,0x5780, 0xfc},
	     {0x6c,0x5781, 0xdf},
	     {0x6c,0x5782, 0x3f},
	     {0x6c,0x5783, 0x08},
	     {0x6c,0x5784, 0x0c},
	     {0x6c,0x5786, 0x20},
	     {0x6c,0x5787, 0x40},
	     {0x6c,0x5788, 0x08},
	     {0x6c,0x5789, 0x08},
	     {0x6c,0x578a, 0x02},
	     {0x6c,0x578b, 0x01},
	     {0x6c,0x578c, 0x01},
	     {0x6c,0x578d, 0x0c},
	     {0x6c,0x578e, 0x02},
	     {0x6c,0x578f, 0x01},
	     {0x6c,0x5790, 0x01},
	     {0x6c,0x5800, 0x1d}, // lens correction
	     {0x6c,0x5801, 0x0e},
	     {0x6c,0x5802, 0x0c},
	     {0x6c,0x5803, 0x0c},
	     {0x6c,0x5804, 0x0f},
	     {0x6c,0x5805, 0x22},
	     {0x6c,0x5806, 0x0a},
	     {0x6c,0x5807, 0x06},
	     {0x6c,0x5808, 0x05},
	     {0x6c,0x5809, 0x05},
	     {0x6c,0x580a, 0x07},
	     {0x6c,0x580b, 0x0a},
	     {0x6c,0x580c, 0x06},
	     {0x6c,0x580d, 0x02},
	     {0x6c,0x580e, 0x00},
	     {0x6c,0x580f, 0x00},
	     {0x6c,0x5810, 0x03},
	     {0x6c,0x5811, 0x07},
	     {0x6c,0x5812, 0x06},
	     {0x6c,0x5813, 0x02},
	     {0x6c,0x5814, 0x00},
	     {0x6c,0x5815, 0x00},
	     {0x6c,0x5816, 0x03},
	     {0x6c,0x5817, 0x07},
	     {0x6c,0x5818, 0x09},
	     {0x6c,0x5819, 0x06},
	     {0x6c,0x581a, 0x04},
	     {0x6c,0x581b, 0x04},
	     {0x6c,0x581c, 0x06},
	     {0x6c,0x581d, 0x0a},
	     {0x6c,0x581e, 0x19},
	     {0x6c,0x581f, 0x0d},
	     {0x6c,0x5820, 0x0b},
	     {0x6c,0x5821, 0x0b},
	     {0x6c,0x5822, 0x0e},
	     {0x6c,0x5823, 0x22},
	     {0x6c,0x5824, 0x23},
	     {0x6c,0x5825, 0x28},
	     {0x6c,0x5826, 0x29},
	     {0x6c,0x5827, 0x27},
	     {0x6c,0x5828, 0x13},
	     {0x6c,0x5829, 0x26},
	     {0x6c,0x582a, 0x33},
	     {0x6c,0x582b, 0x32},
	     {0x6c,0x582c, 0x33},
	     {0x6c,0x582d, 0x16},
	     {0x6c,0x582e, 0x14},
	     {0x6c,0x582f, 0x30},
	     {0x6c,0x5830, 0x31},
	     {0x6c,0x5831, 0x30},
	     {0x6c,0x5832, 0x15},
	     {0x6c,0x5833, 0x26},
	     {0x6c,0x5834, 0x23},
	     {0x6c,0x5835, 0x21},
	     {0x6c,0x5836, 0x23},
	     {0x6c,0x5837, 0x05},
	     {0x6c,0x5838, 0x36},
	     {0x6c,0x5839, 0x27},
	     {0x6c,0x583a, 0x28},
	     {0x6c,0x583b, 0x26},
	     {0x6c,0x583c, 0x24},
	     {0x6c,0x583d, 0xdf}, // lens correction

	     {0x6c,0x0100, 0x01}, //; wake up, streaming

	     {END_OF_SCRIPT, 0, 0}
   };




alt_u8 OV8865_read_cmos_sensor_8(alt_u16 Addr){
	const alt_u8 device_address = MIPI_I2C_ADDR;
	alt_u8 Value;

	//OC_I2CL_Write(I2C_OPENCORES_CAMERA_BASE, device_address, SWAP16(Addr), (alt_u8 *)&Value, sizeof(Value));
	OC_I2CL_Read(I2C_OPENCORES_CAMERA_BASE, device_address, Addr, (alt_u8 *)&Value, sizeof(Value));

	return (Value);
}


void OV8865_write_cmos_sensor_8(alt_u16 Addr, alt_u8 Value){
	const alt_u8 device_address = MIPI_I2C_ADDR;
	//OC_I2CL_Write(I2C_OPENCORES_CAMERA_BASE, device_address, SWAP16(Addr), (alt_u8 *)&Value, sizeof(Value));
	OC_I2CL_Write(I2C_OPENCORES_CAMERA_BASE, device_address, Addr, (alt_u8 *)&Value, sizeof(Value));
}


void OV8865_write_AF(alt_u8 msb, alt_u8 lsb){
	// VCM149C
	const alt_u8 device_address = MIPI_AF_I2C_ADDR;
	OC_I2C_Write(I2C_OPENCORES_CAMERA_BASE, device_address, msb, (alt_u8 *)&lsb, sizeof(lsb));
}

void OV8865_read_AF(void){
	// VCM149C
	const alt_u8 device_address = MIPI_AF_I2C_ADDR;
	alt_u8 szData8[2];
	bool bSuccess;

		bSuccess = OC_I2C_Read_Continue(I2C_OPENCORES_CAMERA_BASE, device_address, szData8, sizeof(szData8));
		if (bSuccess)
			printf("Read MSB=%xh, LSB=%xh\r\n", szData8[0], szData8[1]);
}

void OV8865_FOCUS_Move_to(alt_u16 a_u2MovePosition)
{
  if (a_u2MovePosition > 1023)   {a_u2MovePosition = 1023;}
  if (a_u2MovePosition < 0)     {a_u2MovePosition = 0;}
  int bSuccess;

  Focus_Released(); // waiting for VCM release I2C bus

	bSuccess = oc_i2c_init_ex(I2C_OPENCORES_CAMERA_BASE, 50*1000*1000,400*1000); //I2C: 400K
	if (!bSuccess)
		printf("failed to init MIPI- Camera i2c\r\n");

	//printf("Manual set focus to %d\r\n",a_u2MovePosition);
  alt_u8 msb,lsb;
  msb = (a_u2MovePosition >> 4)&0x00FF;
  lsb = (a_u2MovePosition << 4 )&0x00F0;
  lsb += 0x06;
//	printf("Write MSB=%xh, LSB=%xh\r\n", msb, lsb);
	OV8865_write_AF(msb, lsb+0x6);
	usleep(1000);
//	OV8865_read_AF();

	oc_i2c_uninit(I2C_OPENCORES_CAMERA_BASE);  // Release I2C bus , due to two I2C master shared!

}

void OV8865SetExposure(alt_u32 exposure){

	Focus_Released(); // waiting for VCM release I2C bus

	int bSuccess = oc_i2c_init_ex(I2C_OPENCORES_CAMERA_BASE, 50*1000*1000,400*1000); //I2C: 400K
	if (!bSuccess)
		printf("failed to init MIPI- Camera i2c\r\n");

	if (exposure > 0xFFFFF) exposure = 0xFFFFF;
	if (exposure < 0x20) exposure = 0x20;

	OV8865_write_cmos_sensor_8(0x3500, (exposure >> 16) & 0x0F);
	OV8865_write_cmos_sensor_8(0x3501, (exposure >> 8) & 0xFF);
	OV8865_write_cmos_sensor_8(0x3502, exposure & 0xFF);


	oc_i2c_uninit(I2C_OPENCORES_CAMERA_BASE);
}

void OV8865SetGain(alt_u16 gain){

	Focus_Released(); // waiting for VCM release I2C bus

	int bSuccess = oc_i2c_init_ex(I2C_OPENCORES_CAMERA_BASE, 50*1000*1000,400*1000); //I2C: 400K
	if (!bSuccess)
		printf("failed to init MIPI- Camera i2c\r\n");

	if (gain > 0x7FF) gain = 0x7FF;
	if (gain < 0x080) gain = 0x080;

	OV8865_write_cmos_sensor_8(0x3508, (gain >> 8) & 0x0F);
	OV8865_write_cmos_sensor_8(0x3509, gain & 0xFF);


	oc_i2c_uninit(I2C_OPENCORES_CAMERA_BASE);
}

alt_u32 OV8865ReadExposure(){

	alt_u32 exposure;

	Focus_Released(); // waiting for VCM release I2C bus

	int bSuccess = oc_i2c_init_ex(I2C_OPENCORES_CAMERA_BASE, 50*1000*1000,400*1000); //I2C: 400K
	if (!bSuccess)
		printf("failed to init MIPI- Camera i2c\r\n");

	exposure = OV8865_read_cmos_sensor_8(0x3500);
	exposure = (exposure <<8) | OV8865_read_cmos_sensor_8(0x3501);
	exposure = (exposure <<8) | OV8865_read_cmos_sensor_8(0x3502);

	oc_i2c_uninit(I2C_OPENCORES_CAMERA_BASE);

	return exposure;
}




//ZOOM
void MIPI_BIN_LEVEL(alt_u8 level){
	if(level <= 1) level = 1;
	if(level >= 3) level = 3;

	  Focus_Released(); // waiting for VCM release I2C bus

	  int bSuccess;
		bSuccess = oc_i2c_init_ex(I2C_OPENCORES_CAMERA_BASE, 50*1000*1000,400*1000); //I2C: 400K
		if (!bSuccess)
			printf("failed to init MIPI- Camera i2c\r\n");


	OV8865_write_cmos_sensor_8(0x0100, 0x00);

	if(level == 1){

		OV8865_write_cmos_sensor_8(0x3814, 0x01);
		OV8865_write_cmos_sensor_8(0x3815, 0x01);
		OV8865_write_cmos_sensor_8(0x382a, 0x01);
		OV8865_write_cmos_sensor_8(0x382b, 0x01);

		OV8865_write_cmos_sensor_8(0x3830, 8);
		OV8865_write_cmos_sensor_8(0x3836, 2);
	}
	else if(level == 2){

		OV8865_write_cmos_sensor_8(0x3814, 0x03);
		OV8865_write_cmos_sensor_8(0x3815, 0x01);
		OV8865_write_cmos_sensor_8(0x382a, 0x03);
		OV8865_write_cmos_sensor_8(0x382b, 0x01);

		OV8865_write_cmos_sensor_8(0x3830, 4);
		OV8865_write_cmos_sensor_8(0x3836, 1);

	}
	else if(level == 3){

		OV8865_write_cmos_sensor_8(0x3814, 0x07);
		OV8865_write_cmos_sensor_8(0x3815, 0x01);
		OV8865_write_cmos_sensor_8(0x382a, 0x07);
		OV8865_write_cmos_sensor_8(0x382b, 0x01);

		OV8865_write_cmos_sensor_8(0x3830, 8);
		OV8865_write_cmos_sensor_8(0x3836, 2);
	}
    usleep(10000);
	OV8865_write_cmos_sensor_8(0x0100, 0x01);

	oc_i2c_uninit(I2C_OPENCORES_CAMERA_BASE);  // Release I2C bus , due to two I2C master shared!

}
//
//
//void BLC_LEVEL(alt_u8 blc0,alt_u8 blc1){
//	if(blc0 < 1) blc0 = 0;
//	if(blc0 >= 31) blc0 = 31;
//
//	if(blc1 < 1) blc1 = 0;
//	if(blc1 >= 31) blc1 = 31;
//
//        printf("BLC0 %d ,BLC1 %d \n",blc0,blc1);
//        OV8865_write_cmos_sensor_8(0x0100, 0x00);
//
//		OV8865_write_cmos_sensor_8(0x3830, blc0);
//		OV8865_write_cmos_sensor_8(0x3836, blc1);
//
//	    usleep(10000);
//		OV8865_write_cmos_sensor_8(0x0100, 0x01);
//}


void MipiCameraInit(void)
{

    int i, num;
    int bSuccess;

    Focus_Released(); // waiting for VCM release I2C bus


		bSuccess = oc_i2c_init_ex(I2C_OPENCORES_CAMERA_BASE, 50*1000*1000,400*1000); //I2C: 400K
		if (!bSuccess)
			printf("failed to init MIPI- Camera i2c\r\n");

//  searching for active I2C address.
//    int ii;
//    for(ii= 0; ii<256;ii++){
//    	printf("%x:  ",ii);
//    	alt_u8 data = 0x30;
//        OC_I2C_Write(I2C_OPENCORES_CAMERA_BASE, ii, 0x30,  (alt_u8 *)&data, 1);
//        usleep(10000);
//    }
//


	 OV8865DB("\nStart MipiCameraInit -OV8865!\r\n");
	 OV8865DB("Write Read Test!\n");

	    for(i=0;i<10;i++){
	       OV8865_write_cmos_sensor_8(0x3809,i);
	      usleep(100);
	        printf("%d (%d)\n",OV8865_read_cmos_sensor_8(0x3809),i);
	      usleep(100);
	    }
	 num = sizeof(MipiCameraReg)/sizeof(MipiCameraReg[0]);
     for(i=0;i<num;i++){

    	 if (MipiCameraReg[i].Type == TIME_DELAY)   usleep(MipiCameraReg[i].Data*100);
    	 else if(MipiCameraReg[i].Type == END_OF_SCRIPT)   break;
    	 else if(MipiCameraReg[i].Type == 0x6c)   OV8865_write_cmos_sensor_8(MipiCameraReg[i].Addr, MipiCameraReg[i].Data);
     }


 	oc_i2c_uninit(I2C_OPENCORES_CAMERA_BASE);  // Release I2C bus , due to two I2C master shared!



	 OV8865DB("\nEnd MipiCameraInit! -OV8865!\r\n\n");

}






