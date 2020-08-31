#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/sys_proto.h>
#include <malloc.h>
#include <asm/arch/mx6-pins.h>
#include <asm/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <fsl_esdhc.h>
#include <asm/arch/crm_regs.h>
#include <linux/delay.h>

DECLARE_GLOBAL_DATA_PTR;

typedef struct cmd_tbl cmd_tbl_t;

#define WEAK_PULLUP	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

static iomux_v3_cfg_t const power_key_pads[] = {
	MX6_PAD_CSI0_DATA_EN__GPIO5_IO20| MUX_PAD_CTRL(WEAK_PULLUP),
#	define PIN_POWER	IMX_GPIO_NR(5, 20)
};

int do_need_recovery(cmd_tbl_t *cmdtp, int flag, int argc,
	char * const argv[])
{
	uint8_t src, val1,val2,val3;

	imx_iomux_v3_setup_multiple_pads(power_key_pads, ARRAY_SIZE(power_key_pads));
	gpio_direction_input(PIN_POWER);

	val1 = gpio_get_value(PIN_POWER);
	src = val1;
	mdelay(250);
	val2 = gpio_get_value(PIN_POWER);
	mdelay(250);
	val3 = gpio_get_value(PIN_POWER);
	mdelay(250);

	if ( src == (val1 ^ val2 ^ val3)) {
		puts("Normal boot mode requested!\n");
		return 0;
	} else {
		puts("Recovery mode requested!\n");
		return 1;
	}
}

U_BOOT_CMD(
	need_recovery, 1, 0, do_need_recovery,
	"Check for SoDimm pin45 stable for 750ms.\nIf stable return 0,",
	"(no need recovery mode), else return 1 (recovery mode required)\n"
);
