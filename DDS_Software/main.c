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
menu_t freq_sub1 =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "\e[2J\e[f\nFreq (Hz)", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "Freq (S)", .value = 0, }, },
				.num_entries = 3, .previous = NULL, };

menu_t amp_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "\e[2J\e[f\nOffset", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nVmax", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nVmin", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nVpp", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nVrms", .value = 0, }, }, .num_entries = 3,
				.previous = &freq_sub1, };

menu_t shape_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "\e[2J\e[f\nSine", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nRamp", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nSquare", .value = 0, }, },
				.num_entries = 3, .previous = &amp_sub1_menu, };

menu_t sync_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "\e[2J\e[f\nPhase", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nDuty Cycle", .value = 0, }, },
				.num_entries = 3, .previous = &shape_sub1_menu, };

menu_t status_sub1_menu =
	{  //new info
		.top_entry = 0, .current_entry = 0, .entry =
			{
				{ .flags = 0, .select = adjust_value, .name = "\e[2J\e[f\nAmplitude", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nTime", .value = 0, },
				{ .flags = 0, .select = adjust_value, .name = "\nAbout", .value = 0, }, }, .num_entries = 3,
				.previous = &shape_sub1_menu, };

menu_t main_menu =
	{ .top_entry = 0, .current_entry = 0, .entry =
		{
			{ .flags =
			MENU_FLAG_SUBMENU, .select = NULL, .name = "\e[2J\e[f\nStatus", .value = &status_sub1_menu, },
			{ .flags =
			MENU_FLAG_SUBMENU, .select = &freq_sub1, .name = "\nFrequency", .value = &freq_sub1, },
			{ .flags =
			MENU_FLAG_SUBMENU, .select = NULL,  //function to be called onSelectAction
					.name = "\nAmplitude",  //name to be displayed on LCD
					.value = &amp_sub1_menu,	//address used to maintain context
				},
				{ .flags = MENU_FLAG_SUBMENU, .select = NULL, .name = "\nWave Shape", .value =
						&shape_sub1_menu, },
				{ .flags = MENU_FLAG_SUBMENU, .select = NULL, .name = "\nSync", .value = &sync_sub1_menu, },
				{ .flags = 0,  //0 just means do not switch context. instead, call fxn.
						.select = my_select, .name = "\nsomething", .value = 0, } }, .num_entries = 5,  //required for accurate height display
			.previous = NULL, };

menu_context_t menu_context =
	{ .x_loc = 0, .y_loc = 0, .height = 4, .width = 18, .menu = NULL, };

void my_select(void *arg, char *name) {
//	lcd_clear();
	lcd_clrscr();
	lcd_puts("Selected: ");
	lcd_puts(name);
//	ms_spin(750);
	_delay_ms(750);
}

void adjust_value(void *arg, char *name) {
	/*!
	 * Reference by address to increase or decrease a
	 *value. additional mod needed for ramping speed
	 * context for this fxn is "adjust selected value"
	 *
	 * User instructions: press left/right to change the
	 * adjusted digit. up/down to adjust the digit.
	 * press enter to finalize the value and return
	 * to menu.
	 *
	 */
//	menu_entry_t *context = context_temp;
//	menu_context_t *context;
//	context = context_temp;	//complete hack....
	int j = 0, i = 0, decade = 1;  ///j is joystick input, i is iteration count for
	while (1) {
		_delay_ms(100);				//prevent runaway reading
		j = joystick_read();
		if (j == 0) i = 0;
		else {
			i++;	//for accelerating inc/dec action
			switch (j) {
				case JOYSTICK_DOWN:
//				context->menu->entry->
					arg -= (i * decade);

					break;
				case JOYSTICK_UP:
//				context->menu->entry->
					arg += (i * decade);
//				context->menu->entry.
					break;
				case JOYSTICK_RIGHT:
					//code to move active digit right
					break;
				case JOYSTICK_LEFT:
					//code to move joystick left
					break;
				case JOYSTICK_ENTER:
					return;
			}

			//redraw screen(&arg, &name);
		}
	}
}
void debugBlink(uint8_t bit, uint8_t ratems) {
	DDRB = 0xff;
	uint8_t i = 20;
	while (i--) {
		_delay_ms(ratems);
		PORTB ^= (1 << bit);
	}
}

typedef struct {  ///not sure about placement. global? main? DEFINE SCOPE!!
	uint8_t thing2, value, statusflag;
	uint16_t instruction;
} serStruct_t;

int main() {
	cli();
	uint8_t msg = 0;

//	DDRB
//	PORTB

//	uint8_t j;
//	uint8_t *serBuff = malloc(sizeof(uint8_t));  //init a place for incoming serial buffer
	debugBlink(5, 25);
	spiInit();
	serialInit(2000);  //is defaulting to 19200

	serialWriteString("\e[2J\e[f\n**Serial init . . .\tcomplete\n");

	serialWriteString("LCD init  . . . . .\t");

//	lcd_init(LCD_ON_CURSOR);	// will not return if LCD fails to init!
	serialWriteString("disabled\n");

	serialWriteString("AD9833 init . . . .\t");
	ad9833_init();
	serialWriteString("complete\n");
	serialWriteString("joystick init . . .\t");

	joystickInit(0);
	serialWriteString("complete\n");

	serialWriteString("timer init  . . . .\t");

	timerInit(1000);
	serialWriteString("complete\n");

	ticks = 0;

	serialWriteString("Timer test  . . . .\tcurrent tick=");
	SerialPutChar('0' + (ticks / 100) % 10);
	SerialPutChar('0' + (ticks / 10) % 10);
	SerialPutChar('0' + (ticks % 10));

	SerialPutChar('\n');

	_delay_ms(50);
	serialWriteString("Timer test  . . . .\tcurrent tick=");
	SerialPutChar('0' + (ticks / 100) % 10);
	SerialPutChar('0' + (ticks / 10) % 10);
	SerialPutChar('0' + (ticks % 10));
	SerialPutChar('\n');

	serialWriteString("ADC Test  . . . . .\tcurrent Ain=");

	SerialPutChar('0' + (ADCH / 100) % 10);
	SerialPutChar('0' + (ADCH / 10) % 10);
	SerialPutChar('0' + (ADCH % 10));
	SerialPutChar('\n');

	menu_enter(&menu_context, &main_menu);
//	serialWriteString("menu init . . . . . \t");
//	serialWriteString("complete\n");		//////////////////////////////////
	uint8_t serial_menu_debug = '0';
	while (1) {
		serialGetChar(&serial_menu_debug);
//serialWriteString("keypad input =");
//SerialPutChar(serial_menu_debug);
		if (serial_menu_debug != '0') {
			_delay_ms(50);  //		ms_spin(50);
			switch (serial_menu_debug) {  //joystick_read()) {
				case JOYSTICK_UP:
					menu_next_entry(&menu_context);
					break;
				case JOYSTICK_DOWN:
					menu_prev_entry(&menu_context);
					break;
				case JOYSTICK_LEFT:
					menu_exit(&menu_context);
					break;
				case JOYSTICK_RIGHT:
					break;
				case JOYSTICK_ENTER:
					menu_select(&menu_context);
					break;
			}
		}
	}
}