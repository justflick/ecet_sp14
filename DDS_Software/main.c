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
				{ .flags = 0, .select = adjust_value, .name = "Amplitude", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Time", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "About", .value = 0, }, }, .num_entries = 3,
				.previous = NULL, };

menu_t freq_sub1 =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "Freq (Hz)", .value = &userParameters.Hz,
						.index = 1 },
				{ .flags = 0, .select = adjust_value, .name = "Period (1/Hz)", .value =
						&userParameters.period, }, }, .num_entries = 2, .previous = &status_sub1_menu, };

menu_t amp_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "Offset", .value = &userParameters.offset, },
				{ .flags = 0, .select = adjust_value, .name = "Vmax", .value = &userParameters.vMax, },
				{ .flags = 0, .select = adjust_value, .name = "Vmin", .value = &userParameters.vMin },
				{ .flags = 0, .select = adjust_value, .name = "Vpp", .value = &userParameters.VPP, },
				{ .flags = 0, .select = adjust_value, .name = "Vrms", .value = &userParameters.vRMS, }, },
				.num_entries = 5, .previous = &freq_sub1, };

menu_t shape_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = waveType, .name = "Sine", .value = 0, },
				{ .flags = 0, .select = waveType, .name = "Ramp", .value = 0, },
				{ .flags = 0, .select = waveType, .name = "Square", .index = AD9833_SQUARE, }, },
				.num_entries = 3, .previous = &amp_sub1_menu, };

menu_t sync_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "Phase", .value = &userParameters.phase, },
				{ .flags = 0, .select = adjust_value, .name = "Duty Cycle", .value =
						&userParameters.dutyCycle, }, }, .num_entries = 2, .previous = &shape_sub1_menu, };

menu_t main_menu =
	{ .top_entry = 0, .current_entry = 0, .entry =
		{
			{ .flags =
			MENU_FLAG_SUBMENU, .select = NULL, .name = "Status", .value = &status_sub1_menu, },
			{ .flags =
			MENU_FLAG_SUBMENU, .select = NULL, .name = "Frequency", .value = &freq_sub1, },
			{ .flags =
			MENU_FLAG_SUBMENU, .select = NULL, .name = "Amplitude", .value = &amp_sub1_menu, },
					{ .flags = MENU_FLAG_SUBMENU, .select = NULL, .name = "Wave Shape", .value =
							&shape_sub1_menu, },
					{ .flags =
					MENU_FLAG_SUBMENU, .select = NULL, .name = "Sync", .value = &sync_sub1_menu, }, },
			.num_entries = 5, .previous = NULL, };

menu_context_t menu_context =
	{ .x_loc = 0, .y_loc = 0, .height = 4, .width = 20, };
/**
 * @brief used to select the current active waveform type
 * @param arg	Pointer to value struct
 * @param name Pointer to string for display use
 */
void waveType(void *arg, char *name) {

	uint16_t timeTemp = systemTicks;
	if (shape_sub1_menu.current_entry == 0) {
		ad9833_set_mode(AD9833_SINE);

	}
	if (shape_sub1_menu.current_entry == 1) {
		ad9833_set_mode(AD9833_TRIANGLE);

	}
	if (shape_sub1_menu.current_entry == 2) {
		ad9833_set_mode(AD9833_SQUARE);

	}
	lcd_menu_clear();
	delayTicker_ms(10);
//	uint8_t *tempMode=arg;

	lcd_move_cursor(0, 0);
	lcd_putstring("Selected: ");
	lcd_move_cursor(0, 1);
	lcd_putstring(name);

	while (joystick_read() == JOYSTICK_NOPRESS) {
		if (timeTemp + 5000 < systemTicks) break;
		//wait for user to exit
	}

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

	parameter_defs *localParam = arg;  //create pointer of crrect type to allow for indirection
	int32_t tempValue = localParam->currentValue;

	uint8_t leadDigit = 0, j = 0;  ///j is joystick input, decade represents digit being modified
	int8_t cursoroffset, bcdArray[localParam->digits];  //instantiate BCD array.

	/* function operation:
	 *
	 * take arg, convert from long to bcd array
	 * manipulate via user input
	 * 		during each iteration, check for digit over/underflow
	 * convert from bcd array to long and copyback via reference
	 */

	for (int i = 0; i < localParam->digits; i++) {  //reduce the long to a BCD array
		bcdArray[i] = tempValue % 10;
		tempValue /= 10;
	}
	delayTicker_ms(20);  //this delay is to ensure that the LCD ready for a new command.
	lcd_set_mode(LCD_CMD_ON_CURSOR);
	while (1) {

		/*
		 * The following nested comparisons conform the bcdarray to base-10.
		 * Functionally, this is a ripple carry adder which simply detects over/underflows
		 * and carries/borrows from adjacent digits as needed.
		 */
		for (int i = 0; i < localParam->digits; i++) {

			if (bcdArray[i] >= 10) {   //detect top radix of current digit
				if (bcdArray[i] >= 200) {  //detect underflow
					bcdArray[i + 1]--;  //borrow from next highest
					bcdArray[i] = 9;		//carry down
				} else
					if ((bcdArray[i] >= 10) && (bcdArray[i] < 30)) {  //detect top radix of higher decades
						bcdArray[i + 1]++;  //carry upward as needed
						bcdArray[i] = 0;  //wrap current digit back to bottom radix
					}
			}
			if ((bcdArray[i] > 0) && (bcdArray[i] < 10)) leadDigit = i;
		}
		tempValue = 0;
		for (int i = localParam->digits - 1; i >= 0; i--) {  //Convert the BCD array back to long
			tempValue *= 10;
			tempValue += bcdArray[i];
		}
		if ((tempValue > localParam->max) || (tempValue < localParam->min)) {  //prevent the user from over/underflowing input
			tempValue = localParam->currentValue;  //undo change to local copy of tempValue
			for (int i = 0; i < localParam->digits; i++) {  //restore the BCD array with in-range value
				bcdArray[i] = tempValue % 10;
				tempValue /= 10;
			}
		} else localParam->currentValue = tempValue;  //copy calculated value back to pointed loc.
		if (localParam->decade >= localParam->decimal) cursoroffset = localParam->digits;  //calculate the LCD decimal placement
		else cursoroffset = localParam->digits + 1;

		lcd_move_cursor(0, 0);
		lcd_putstring(name);
		lcd_move_cursor(0, 1);

		updateParameters(localParam);
//		ad9833_set_frequency(localParam->currentValue / 100);

		lcd_print_numeric(localParam->currentValue, localParam->digits, localParam->decimal);
		lcd_move_cursor(cursoroffset - localParam->decade, 1);  //move cursor to indicate active digit.
		j = JOYSTICK_NOPRESS;  //reset the switch condition to avoid looping
		while (j == JOYSTICK_NOPRESS) {

			j = joystick_read();
			switch (j) {			//this switch case takes user input and acts on the bcd array
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
					if (localParam->decade < localParam->digits - 1) localParam->decade++;
					break;
				case JOYSTICK_ENTER:
					lcd_set_mode(LCD_CMD_ON);
					delayTicker_ms(10);  //allow LCD command to finish
					return;
				default:
					break;
			}
		}
	}
}

int main() {
	DDRD = 0xf0;
	DDRC = (1 << 5) | (1 << 4);

	/*
	 * Initialize structures to set default values as well as parameter limits
	 *
	 *
	 */
	userParameters.Hz.min = 10;
	userParameters.Hz.max = 50000000;
	userParameters.Hz.currentValue = 100000;
	userParameters.Hz.digits = 9;
	userParameters.Hz.decimal = 2;
	userParameters.Hz.decade = 3;
	userParameters.Hz.parameterName=Parameter_Hz;

	userParameters.period.min = 2;
	userParameters.period.max = 10000000;
	userParameters.period.currentValue = 25000;
	userParameters.period.digits = 8;
	userParameters.period.decimal = 6;
	userParameters.period.decade = 5;
	userParameters.period.parameterName=Parameter_Period;

	userParameters.PWM.min = 0;
	userParameters.PWM.max = 1000;
	userParameters.PWM.currentValue = 500;
	userParameters.PWM.digits = 3;
	userParameters.PWM.decimal = 1;
	userParameters.PWM.decade = 2;
	userParameters.PWM.parameterName=Parameter_PWM;

	userParameters.VPP.min = 0;
	userParameters.VPP.max = 1200;
	userParameters.VPP.currentValue = 300;
	userParameters.VPP.digits = 4;
	userParameters.VPP.decimal = 2;
	userParameters.VPP.decade = 2;
	userParameters.VPP.parameterName=Parameter_VPP;

	userParameters.offset.min = -60;
	userParameters.offset.max = 60;
	userParameters.offset.currentValue = 0;
	userParameters.offset.digits = 3;
	userParameters.offset.decimal = 1;
	userParameters.offset.decade = 2;
	userParameters.offset.parameterName=Parameter_Offset;

	userParameters.vMax.min = -110;
	userParameters.vMax.max = 120;
	userParameters.vMax.currentValue = 30;
	userParameters.vMax.digits = 3;
	userParameters.vMax.decimal = 1;
	userParameters.vMax.decade = 2;
	userParameters.vMax.parameterName=Parameter_Vmax;

	userParameters.vMin.min = -110;
	userParameters.vMin.max = 120;
	userParameters.vMin.currentValue = 30;
	userParameters.vMin.digits = 3;
	userParameters.vMin.decimal = 1;
	userParameters.vMin.decade = 2;
	userParameters.vMin.parameterName=Parameter_Vmin;

	userParameters.vRMS.min = 0;
	userParameters.vRMS.max = 1200;
	userParameters.vRMS.currentValue = 300;
	userParameters.vRMS.digits = 4;
	userParameters.vRMS.decimal = 2;
	userParameters.vRMS.decade = 3;
	userParameters.vRMS.parameterName=Parameter_Vrms;

	userParameters.phase.min = 0;
	userParameters.phase.max = 36000;
	userParameters.phase.currentValue = 18000;
	userParameters.phase.digits = 5;
	userParameters.phase.decimal = 2;
	userParameters.phase.decade = 2;
	userParameters.phase.parameterName=Parameter_Phase;

	userParameters.dutyCycle.min = 0;
	userParameters.dutyCycle.max = 10000;
	userParameters.dutyCycle.currentValue = 5000;
	userParameters.dutyCycle.digits = 5;
	userParameters.dutyCycle.decimal = 2;
	userParameters.dutyCycle.decade = 2;
	userParameters.dutyCycle.parameterName=Parameter_DutyCycle;
	serialInit(57600);
	timerInit(1000);

	serialWriteString("\e[2J\e[f\nSerial init . . . .\tcomplete\n");
	serialWriteString("LCD init  . . . . .\t");
	lcd_initialize(LCD_FUNCTION_8x2, LCD_CMD_ENTRY_INC, LCD_CMD_ON);
	serialWriteString("Complete\n");
	serialWriteString("AD9833 init . . . .\t");
	ad9833Init();
	serialWriteString("Complete\n");
	serialWriteString("joystick init . . .\t");
	serialWriteString("complete\n");
	serialWriteString("timer init  . . . .\t");
	systemTicks = 0;
	serialWriteString("complete\n");
	serialWriteString("F_CPU . . . . . . .\tclck= ");
	serialWriteNum(F_CPU / (1000UL), 6);
	serialWriteString("\nTimer test  . . . .\ttick= ");
	serialWriteNum(systemTicks, 3);
	delayTicker_ms(10);
	serialWriteString("\nTimer test  . . . .\ttick= ");
	serialWriteNum(systemTicks, 3);
	joystickInit(0);
	serialWriteString("\nADC Test  . . . . .\tAin= ");
	delayTicker_ms(25);
	serialWriteNum(pbVal, 3);
	serialPutChar('\n');

	menu_enter(&menu_context, &main_menu);  //Establish menu root
	while (1) {

		switch (joystick_read()) {
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
	}
}
