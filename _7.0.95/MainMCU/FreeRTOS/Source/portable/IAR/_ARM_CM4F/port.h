#ifdef __cplusplus
 extern "C" {
#endif 
/*
 * Setup the timer to generate the tick interrupts.
 */
void prvSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortSysTickHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
extern void vPortStartFirstTask( void );

#ifdef __cplusplus
}
#endif
