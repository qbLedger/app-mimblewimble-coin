// Header guard
#ifndef MENUS_H
#define MENUS_H


// Header files
#include "crypto.h"
#include "currency_information.h"


// Definitions

// Check if target is the Nano X
#ifdef TARGET_NANOX

	// Line character size
	#define LINE_CHARACTER_SIZE 20

// Otherwise
#else

	// Line character size
	#define LINE_CHARACTER_SIZE 14
#endif

// Requestor line buffer size
#define REQUESTOR_LINE_BUFFER_SIZE (LINE_CHARACTER_SIZE * 2 + sizeof((char)'\0'))

// Time line buffer size
#define TIME_LINE_BUFFER_SIZE sizeof("HH:MM:SS on YYYYYY-mm-dd ZZZ")

// Public key line buffer size
#define PUBLIC_KEY_LINE_BUFFER_SIZE (COMPRESSED_PUBLIC_KEY_SIZE * HEXADECIMAL_CHARACTER_SIZE + sizeof((char)'\0'))

// Public key type line buffer size
#define PUBLIC_KEY_TYPE_LINE_BUFFER_SIZE sizeof("Verify Slatepack")

// Amount line buffer size
#define AMOUNT_LINE_BUFFER_SIZE (sizeof("18446744073709551615") - sizeof((char)'\0') + sizeof('.') + sizeof(' ') + CURRENCY_INFORMATION_ABBREVIATION_SIZE)

// Fee line buffer size
#define FEE_LINE_BUFFER_SIZE (sizeof("18446744073709551615") - sizeof((char)'\0') + sizeof('.') + sizeof(' ') + CURRENCY_INFORMATION_ABBREVIATION_SIZE)

// Receiver line buffer size
#define RECEIVER_LINE_BUFFER_SIZE (ED25519_PUBLIC_KEY_SIZE * HEXADECIMAL_CHARACTER_SIZE + sizeof((char)'\0'))


// Constants

// Menu
enum Menu {

	// Main menu
	MAIN_MENU,
	
	// About menu
	ABOUT_MENU,
	
	// Export root public key menu
	EXPORT_ROOT_PUBLIC_KEY_MENU,
	
	// Verify public key menu
	VERIFY_PUBLIC_KEY_MENU,
	
	// Verify address menu
	VERIFY_ADDRESS_MENU,
	
	// Sign Tor certificate menu
	SIGN_TOR_CERTIFICATE_MENU,
	
	// Finalize transaction menu
	FINALIZE_TRANSACTION_MENU,
	
	// Processing menu
	PROCESSING_MENU
};

// Main menu screen
enum MainMenuScreen {

	// Ready
	READY_SCREEN,
	
	// About screen
	ABOUT_SCREEN,
	
	// Exit screen
	EXIT_SCREEN
};


// Global variables

// Requestor line buffer
extern char requestorLineBuffer[REQUESTOR_LINE_BUFFER_SIZE];

// Time line buffer
extern char timeLineBuffer[TIME_LINE_BUFFER_SIZE];

// Public key line buffer
extern char publicKeyLineBuffer[PUBLIC_KEY_LINE_BUFFER_SIZE];

// Public key type line buffer
extern char publicKeyTypeLineBuffer[PUBLIC_KEY_TYPE_LINE_BUFFER_SIZE];

// Amount line buffer
extern char amountLineBuffer[AMOUNT_LINE_BUFFER_SIZE];

// Fee line buffer
extern char feeLineBuffer[FEE_LINE_BUFFER_SIZE];

// Receiver line buffer
extern char receiverLineBuffer[RECEIVER_LINE_BUFFER_SIZE];


// Function prototypes

// Clear menu buffers
void clearMenuBuffers(void);

// Show main menu
void showMainMenu(enum MainMenuScreen mainMenuScreen);

// Show menu
void showMenu(enum Menu menu);


#endif
