#include <common.h>
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

iomux_v3_cfg_t const bb_spi_9b_pads[] = {
	MX6_PAD_NANDF_D0__GPIO2_IO00	| MUX_PAD_CTRL(NO_PAD_CTRL),	/* CS   - soDimm 111 */
	MX6_PAD_EIM_RW__GPIO2_IO26	| MUX_PAD_CTRL(NO_PAD_CTRL),	/* SCK  - soDimm 112 */
	MX6_PAD_NANDF_D1__GPIO2_IO01	| MUX_PAD_CTRL(NO_PAD_CTRL),	/* MOSI - soDimm 113 */
	MX6_PAD_EIM_LBA__GPIO2_IO27	| MUX_PAD_CTRL(NO_PAD_CTRL),	/* MISO - soDimm 114 */
#	define PIN_CS	IMX_GPIO_NR(2, 0)
#	define PIN_SCK	IMX_GPIO_NR(2, 26)
#	define PIN_MOSI	IMX_GPIO_NR(2, 1)
#	define PIN_MISO	IMX_GPIO_NR(2, 27)
};

static void spi_bitbang_init(void)
{
	imx_iomux_v3_setup_multiple_pads(bb_spi_9b_pads, ARRAY_SIZE(bb_spi_9b_pads));
	gpio_direction_output(PIN_CS, 1);
	gpio_direction_output(PIN_SCK, 1);
	gpio_direction_output(PIN_MOSI, 1);
	gpio_direction_input(PIN_MISO);
}

void scl(uint8_t val){ gpio_set_value(PIN_SCK, !!val); mdelay(1); }
void cs(uint8_t val){ gpio_set_value(PIN_CS, !!val); mdelay(1); }
void mosi(uint8_t val){ gpio_set_value(PIN_MOSI, !!val); mdelay(1); }
void miso(uint8_t *val){ mdelay(1); *val = gpio_get_value(PIN_MISO); }

static uint8_t send_bit(uint8_t bit)
{
	uint8_t ret;

	scl(1);
	scl(0);
	mosi(bit);
	miso(&ret);
	scl(1);

	return !!ret;
}

static uint8_t send_byte(uint8_t byte)
{
	uint8_t ret = 0;
	int i;

	for(i=7;i>-1;i--)
		ret |= send_bit(byte & (1<<i)) << i;

	return ret;
}

static void spi_cmd(uint8_t cmd, uint8_t arg, bool is_arg)
{
	cs(1);	/* inactivate */
	scl(1);	/* scl high at start */
	cs(0);	/* select chip */
	send_bit(0);	/* command */
	send_byte(cmd);
	if(is_arg) {
		send_bit(1);	/* param */
		send_byte(arg);
	}
	cs(1);	/* inactive */
}

int do_koe_display_init(cmd_tbl_t *cmdtp, int flag, int argc,
	char * const argv[])
{
	if(argc != 2) {
		printf("Need mode: on, off or refresh!\n");
		return -1;
	};

	spi_bitbang_init();
	spi_cmd(0x10, 0x00, false);		/* SLPIN */
	mdelay(250);
	spi_cmd(0x01, 0x00, false);		/* SWRESET */
	spi_cmd(0x00, 0x00, false);		/* NOP */

	if(!strcmp(argv[1],"on")) {
		printf("Turn on koe display\n");
		spi_cmd(0x3A, 0x70, true);	/* COLMOD */
		spi_cmd(0x26, 0x00, true);	/* GAMSET */
		spi_cmd(0x36, 0x00, true);	/* MADCTL */
		spi_cmd(0x11, 0x00, false);	/* SLPOUT */
		mdelay(150); /* more, than 120 ms. */
		spi_cmd(0x29, 0x00, false);	/* DISON */
	} else if (!strcmp(argv[1],"off")) {
		printf("Turn off koe display\n");
		spi_cmd(0x10, 0x00, false); 	/* SLPIN */
		mdelay(250);			/* more, than 200 ms. */
	} else if (!strcmp(argv[1],"refresh")) {
		printf("Refresh koe display\n");
		spi_cmd(0x3A, 0x70, true);	/* COLMOD */
		spi_cmd(0x26, 0x00, true);	/* GAMSET */
		spi_cmd(0x36, 0x00, true);	/* MADCTL */
		spi_cmd(0x20, 0x00, false);	/* DISINOFF */
		spi_cmd(0x29, 0x00, false);	/* DISON */
		spi_cmd(0x11, 0x00, false);	/* SLPOUT */
	} else {
		printf("Unknown command: %s\n", argv[1]);
		return -1;
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	koe_display_init, 2, 0, do_koe_display_init,
	"Init KOE display connected to 111(CS),112(SCK),113(MOSI),114(MISO) SoDimm pins.",
	"<mode>\nWhere mode: on,off,refresh\n"
);
