
#ifndef __TSLIB_H__
#define __TSLIB_H__

#define XRES 240
#define YRES 400

typedef struct {
	int x[5], xfb[5];
	int y[5], yfb[5];
	int a[7];
} calibration_t;

struct tslib_linear {
	//struct tslib_module_info module;
	int	swap_xy;

// Linear scaling and offset parameters for pressure
	int	p_offset;
	int	p_mult;
	int	p_div;

// Linear scaling and offset parameters for x,y (can include rotation)
	int	a[7];

// Screen resolution at the time when calibration was performed
	unsigned int cal_res_x;
	unsigned int cal_res_y;
};

extern void Tp_Put_Cross(int x, int y, unsigned colidx);
extern int ts_test(void);

#endif //__TSLIB_H__
