#include "main.h"

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
				{ .flags = 0, .select = adjust_value, .name = "Freq (Hz)", .value = &userParameters.Hz, },
						{ .flags = 0, .select = adjust_value, .name = "Freq (1/S)", .value =
								&userParameters.period, }, }, .num_entries = 2, .previous = &status_sub1_menu, };

menu_t amp_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "Offset", .value = &userParameters.offset, },
				{ .flags = 0, .select = adjust_value, .name = "Vmax", .value = &userParameters.vMax, },
				{ .flags = 0, .select = adjust_value, .name = "Vmin", .value = &userParameters.vMin },
				{ .flags = 0, .select = adjust_value, .name = "Vpp", .value = &userParameters.VPP, },
				{ .flags = 0, .select = adjust_value, .name = "Vrms", .value = &userParameters.vRMS, }, }, .num_entries = 5,
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
				{ .flags = 0, .select = adjust_value, .name = "Phase", .value = &userParameters.phase, },
				{ .flags = 0, .select = adjust_value, .name = "Duty Cycle", .value = &userParameters.dutyCycle, }, }, .num_entries =
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
/**
 *
 * @param arg requires uint32_t number as input
 * @param name takes a pointer to a string as the input.
 *
 *  Reference by address to increase or decrease a
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
void adjust_value(void *arg, char *name) {

	parameter_defs *localParam = arg;
	uint32_t argTemp = localParam->currentValue;

	uint8_t outputLen = 10, j = 0;  ///j is joystick input, decade represents digit being modified
	uint8_t cursoroffset, bcdArray[outputLen];  //instantiate BCD array. Values are in **REVERSE** order for simplified math
	uint8_t decimal = 3;	//# of decimal places to print to LCD

	if (localParam->decade == 0) localParam->decade = 2;  //set initial value
	/* function operation:
	 *
	 * take arg, convert from long to bcd array
	 * manipulate via user input
	 * 		during each iteration, check for digit over/underflow
	 * convert from bcd array to long and copyback via reference
	 */

	for (int i = 0; i < outputLen; i++) {  //reduce the long to a BCD array
		bcdArray[i] = argTemp % 10;
		argTemp /= 10;

	}
	lcd_set_mode(LCD_CMD_ON_CURSOR_BLINK);

	while (1) {

		/*
		 * The following nested comparisons conform the bcdarray to base-10.
		 * Functionally, this is a ripple carry adder which simply detects over/underflows
		 * and carries/borrows from adjacent digits as needed.
		 */
		for (int i = 0; i < outputLen; i++) {
			if (bcdArray[i] >= 10) {   //detect top radix of current digit
				if (bcdArray[i] >= 200) {  //detect underflow
					bcdArray[i + 1]--;	//borrow from next highest
					bcdArray[i] = 9;		//carry down
				} else
					if ((bcdArray[i] >= 10) && (bcdArray[i] < 30)) {	//detect top radix of higher decades
						bcdArray[i + 1]++;	//carry upward as needed
						bcdArray[i] = 0;		//wrap current digit back to bottom radix
					}
			}
		}
		argTemp = 0;
		for (int i = outputLen - 1; i >= 0; i--) {  //Convert the BCD array back to long
			argTemp *= 10;
			argTemp += bcdArray[i];
		}
		if (localParam->decade >= decimal) cursoroffset = outputLen - 1;  //calculate the LCD decimal placement
		else cursoroffset = outputLen;
		localParam->currentValue = argTemp;  //copy calculated value back to pointed loc.
		lcd_move_cursor(0, 0);
		lcd_putstring(name);
		lcd_move_cursor(0, 1);
		lcd_print_numeric(argTemp, outputLen, decimal);
		lcd_move_cursor(cursoroffset - localParam->decade, 1);  //move cursor to indicate active digit.
		j = 0;	//reset the switch condition to avoid looping

		while (j == 0) {
			serialGetChar(&j, 1, 100);	 		//serial input for debug purposes.
			//		j = joystick_read();	//inactive during debug
			switch (j) {	//this switch case takes user input and acts on the bcd array
				case JOYSTICK_DOWN:
					bcdArray[localParam->decade]--;
					break;
				case JOYSTICK_UP:
					bcdArray[localParam->decade]++;
					break;
				case JOYSTICK_RIGHT:
					if (localParam->decade > 0) localParam->decade--;
					break;
				case JOYSTICK_LEFT:
					if (localParam->decade < outputLen - 1) localParam->decade++;
					break;
				case JOYSTICK_ENTER:
					lcd_set_mode(LCD_CMD_ON);
					return;
				default:
					break;
			}

		}
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
	DDRD = 0xf0;

//	uint8_t j;
//	uint8_t *serBuff = malloc(sizeof(uint8_t));  //init a place for incoming serial buffer
//	timerInit(1000);
	userParameters.Hz.currentValue = 654321;
	serialInit(57600);
	timerInit(1000);

	AD9833SpiInit();
	serialWriteString("\e[2J\e[f\nSerial init . . . .\tcomplete\n");
	serialWriteString("LCD init  . . . . .\t");

	lcd_initialize(LCD_FUNCTION_8x2, LCD_CMD_ENTRY_INC, LCD_CMD_ON);

//	lcd_move_cursor(1, 5);
//	lcd_putc('8');
//	while (1) {
//		;;
//
//	}
	serialWriteString("Complete\n");

	serialWriteString("AD9833 init . . . .\t");
//	ad9833_init();
	serialWriteString("disabled\n");
	serialWriteString("joystick init . . .\t");

//	joystickInit(0);
	serialWriteString("complete\n");
	serialWriteString("timer init  . . . .\t");
	systemTicks = 0;
	serialWriteString("complete\n");

	serialWriteString("F_CPU . . . . . . .\tclck= ");
	serialWriteNum(F_CPU / (1000UL),6);
	serialWriteString("\nTimer test  . . . .\ttick= ");
	serialWriteNum(systemTicks,3);
	delayTicker(17);

	serialWriteString("\nTimer test  . . . .\ttick= ");
	serialWriteNum(systemTicks,3);

	serialWriteString("\nADC Test  . . . . .\tAin= ");
	serialWriteNum(ADCH,3);
	serialPutChar('\n');

//	uint32_t numTest = 12345678;
//	lcd_menu_clear();
//	lcd_move_cursor(0, 0);
//	lcd_putstring("numeric test");
//	lcd_move_cursor(0 , 1);
//	lcd_print_numeric(numTest, 8, 1);
//	lcd_move_cursor(0, 2);
//	lcd_putstring("hello");
//	while (1) {
//		;;
//
//	}

	menu_enter(&menu_context, &main_menu);	//Set menu system base location

	uint8_t serial_menu_debug = '0';
	while (1) {
		if (!serialGetChar(&serial_menu_debug, 1, 100)) {
//					delayNoBlock(30);
			if (serial_menu_debug != '0') {
				switch (serial_menu_debug) {  //joystick_read()) {
					case JOYSTICK_UP:
						menu_prev_entry(&menu_context);
						break;
					case JOYSTICK_DOWN:
						menu_next_entry(&menu_context);
						break;
					case JOYSTICK_LEFT:
						menu_exit(&menu_context);
						break;
					case JOYSTICK_RIGHT:
						break;
					case JOYSTICK_ENTER:
						lcd_menu_clear()
						menu_select(&menu_context);
						break;
				}
				serial_menu_debug = '0';
			}
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