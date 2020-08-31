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

DECLARE_GLOBAL_DATA_PTR;

typedef struct cmd_tbl cmd_tbl_t;

#define WEAK_PULLUP	(PAD_CTL_PUS_22K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

static iomux_v3_cfg_t const power_key_pads[] = {
	MX6_PAD_SD4_DAT3__GPIO2_IO11	| MUX_PAD_CTRL(WEAK_PULLUP),
#	define PIN_POWER_OFF	IMX_GPIO_NR(2, 11)
};

int do_poweroff(cmd_tbl_t *cmdtp, int flag, int argc,
	char * const argv[])
{
	imx_iomux_v3_setup_multiple_pads(power_key_pads, ARRAY_SIZE(power_key_pads));
	gpio_direction_output(PIN_POWER_OFF, 0);

	return 0;
}

U_BOOT_CMD(
	poweroff, 1, 0, do_poweroff,
	"Power off system.\n", "Just set power-off GPIO to 0 and wait\n"
);
