#include "main.h"
//#include "joystick.h"

/*!instantiate linked list here
 * Main
 *  + Freq
 *    |-Hz 			- adjust
 *    |-f  			- adjust
 *  +Amplitude
 *    |-Offset		-adjust
 *    |-Vmax		-adjust
 *    |-Vmin		-adjust
 *    |-Vpp			-adjust
 *    |-Vrms		-adjust
 *  +Shape
 *    |-Sine		-adjust
 *    |-Ramp		-adjust
 *    |-Square		-adjust
 *  +Sync
 *    |-Duty_cyc	-adjust
 *    |-Phase		-adjust
 */
//begin freq submenus
menu_t status_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = my_select, .name = "Amplitude", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Time", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "About", .value = 0, }, }, .num_entries = 3,
				.previous = NULL, };

menu_t freq_sub1 =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "Freq (Hz)", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Freq (1/S)", .value = 0, }, }, .num_entries =
				2, .previous = &status_sub1_menu, };

menu_t amp_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "Offset", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Vmax", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Vmin", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Vpp", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Vrms", .value = 0, }, }, .num_entries = 5,
				.previous = &freq_sub1, };

menu_t shape_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "Sine", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Ramp", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Square", .value = 0, }, }, .num_entries = 3,
				.previous = &amp_sub1_menu, };

menu_t sync_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "Phase", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Duty Cycle", .value = 0, }, }, .num_entries =
				2, .previous = &shape_sub1_menu, };

menu_t main_menu =
	{ .top_entry = 0, .current_entry = 0,
			.entry =
						{
							{ .flags =
							MENU_FLAG_SUBMENU, .select = NULL, .name = "Status", .value = &status_sub1_menu, },
							{ .flags =
							MENU_FLAG_SUBMENU, .select = NULL, .name = "Frequency", .value = &freq_sub1, },
							{ .flags =
							MENU_FLAG_SUBMENU, .select = NULL, .name = "Amplitude", .value = &amp_sub1_menu, },
							{ .flags = MENU_FLAG_SUBMENU, .select = NULL, .name = "Wave Shape", .value =
									&shape_sub1_menu, },
									{ .flags = MENU_FLAG_SUBMENU, .select = NULL, .name = "Sync", .value =
											&sync_sub1_menu, }, }, .num_entries = 5, .previous = NULL, };

menu_context_t menu_context =
	{ .x_loc = 0, .y_loc = 0, .height = 4, .width = 20, };

void my_select(void *arg, char *name) {
	lcd_menu_clear();

	lcd_move_cursor(0, 0);
	lcd_putstring("Selected: ");
	lcd_move_cursor(0, 1);
	lcd_putstring(name);
	_delay_ms(100);

}

void adjust_value(void *arg, char *name) {
	/*!
	 * Reference by address to increase or decrease a
	 * value.
	 * User instructions: press left/right to change the
	 * adjusted digit. up/down to adjust the digit.
	 * press enter to finalize the value and return
	 * to menu.
	 *
	 * Numeric format: ###,###.##
	 * range: 499,999.9 - 0.1
	 * Todo: finish underflow/overflow handling,
	 */
	lcd_set_mode(LCD_CMD_ON_CURSOR_BLINK);
	uint16_t menuVal = 0;
	uint8_t j = 0, decade = 2;  ///j is joystick input, decade represents digit being modified
	uint8_t bcdArray[8] =
		{ 7, 0, 0, 5, 0, 0, 3 };	//instantiate BCD array. Values are in **REVERSE** order for simplified math
	while (1) {
//		_delay_ms(100);				//prevent runaway reading
//		j = joystick_read();	//inactive during debug

		serialGetChar(&j, 1);		//serial input for debug purposes.
		switch (j) {
			case JOYSTICK_DOWN:
				bcdArray[decade]--;
				break;
			case JOYSTICK_UP:
				bcdArray[decade]++;
				break;
			case JOYSTICK_RIGHT:
				if ((decade >= 1) && (decade <= 8)) decade--;
				break;
			case JOYSTICK_LEFT:
				if ((decade >= 0) && (decade <= 8)) decade++;
				break;
			case JOYSTICK_ENTER:
				lcd_set_mode(LCD_CMD_ON);
				return;
		}
		menuVal = 0;
		serialWriteString("\n\n decade=");
		serialPutChar('0' + decade);
		serialWriteString("\t\tbcd string=");

//		for (int i = 0; i < 8; i++) {  //handle overflow and underflow condition
//			serialPutChar('0' + bcdArray[i]);
//			if (bcdArray[i] > 9) {
//
//				if ((bcdArray[i] >= 10)&&(bcdArray[i]<30)) {
//
//					bcdArray[i] = 0;
//					bcdArray[i + 1] += 1;
//				} else
//					if (bcdArray[i] >= 200) {
//						bcdArray[i + 1] -= 1;
//					}
//			}
//			menuVal *= 10;
//			menuVal += bcdArray[i];
//		}

		j = 0;
		lcd_move_cursor(0, 0);
		lcd_putstring(arg);
		lcd_move_cursor(11, 1);  //move cursor to print reverse order numeric
		lcd_print_numeric(menuVal, 8, 2);
		lcd_move_cursor(11 + decade, 1);  //move cursor to indicate active digit.
	}
}

void delayTicker(uint16_t ms) {
	uint16_t tmpTimer = systemTicks;
	while ((tmpTimer + ms) > systemTicks) {

	}
}
void debugBlink(uint8_t bit, uint8_t ratems) {
	DDRB = 0xff;
	uint8_t i = 30;
	while (i--) {
		_delay_ms(ratems);
		PORTB ^= (1 << bit);
	}
}

int main() {
//	cli();
	uint8_t msg = 0;
	DDRD = 0xf0;

//	uint8_t j;
//	uint8_t *serBuff = malloc(sizeof(uint8_t));  //init a place for incoming serial buffer
//	timerInit(1000);

	serialInit(57600);
	timerInit(1000);

	AD9833SpiInit();
	serialWriteString("\e[2J\e[f\n**Serial init . . .\tcomplete\n");
	serialWriteString("LCD init  . . . . .\t");

	lcd_initialize(LCD_FUNCTION_8x2, LCD_CMD_ENTRY_INC, LCD_CMD_ON);

//	lcd_move_cursor(1, 5);
//	lcd_putc('8');
//	while (1) {
//		;;
//
//	}
	serialWriteString("disabled\n");

	serialWriteString("AD9833 init . . . .\t");
//	ad9833_init();
	serialWriteString("complete\n");
	serialWriteString("joystick init . . .\t");

//	joystickInit(0);
	serialWriteString("complete\n");
	serialWriteString("timer init  . . . .\t");
	systemTicks = 0;
	serialWriteString("complete\n");

	serialWriteString("F_CPU test  . . . .\ttick= ");
	serialWriteNum(F_CPU / (1000UL));
	serialWriteString("\nTimer test  . . . .\ttick= ");
	serialWriteNum(systemTicks);
//	debugBlink(5, 50);
	delayTicker(17);
	//	debugBlink(5, 50);

	serialWriteString("\nTimer test  . . . .\ttick= ");
	serialWriteNum(systemTicks);

	serialWriteString("\nADC Test  . . . . .\tAin= ");
	serialWriteNum(ADCH);
	serialPutChar('\n');
//
//	uint32_t numTest = 12345678;
//	lcd_menu_clear();
//	lcd_move_cursor(0, 0);
//	lcd_putstring("numeric test");
//	lcd_move_cursor(0 + 8, 1);
//	lcd_print_numeric(numTest, 8, 1);
//	lcd_move_cursor(0, 2);
//	lcd_putstring("hello");
//	while (1) {
//		;;
//
//	}

	menu_enter(&menu_context, &main_menu);

	uint8_t serial_menu_debug = '0';
	while (1) {
		serialGetChar(&serial_menu_debug, 1);
//					delayNoBlock(30);
		if (serial_menu_debug != '0') {
//			serialWriteString("\e[2J\e[f");  //clears terminal
//			serialPutChar('\n');
			switch (serial_menu_debug) {  //joystick_read()) {
				case JOYSTICK_UP:
//					serialWriteString("up");
					menu_prev_entry(&menu_context);
					break;
				case JOYSTICK_DOWN:
//					serialWriteString("down");
					menu_next_entry(&menu_context);
					break;
				case JOYSTICK_LEFT:
//					serialWriteString("left");
					menu_exit(&menu_context);
					break;
				case JOYSTICK_RIGHT:
//					serialWriteString("right");
					break;
				case JOYSTICK_ENTER:
					lcd_menu_clear()
					;

//					serialWriteString("enter");
					menu_select(&menu_context);
					break;
			}
			serial_menu_debug = '0';

		}
	}
}

//leftovers from debugging:

//Code to clear screen and write value
//
//			serialWriteString("\e[2J\e[f");
//			serialWriteString("current value    = ");
//			serialWriteNum(value);
//			serialPutChar('\n');

//Code to dump contents of ringbuffer to serial
//
//		for (int i = 0; i < SERIAL_BUFFER_LEN; ++i) {
//			serialPutChar(rxSerialBuff[i]);
//		}