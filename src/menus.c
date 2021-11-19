// Header files
#include <string.h>
#include <ux.h>
#include "common.h"
#include "currency_information.h"
#include "menus.h"
#include "process_requests.h"


// Global variables

// Requestor line buffer
char requestorLineBuffer[REQUESTOR_LINE_BUFFER_SIZE];

// Time line buffer
char timeLineBuffer[TIME_LINE_BUFFER_SIZE];

// Public key line buffer
char publicKeyLineBuffer[PUBLIC_KEY_LINE_BUFFER_SIZE];

// Public key type line buffer
char publicKeyTypeLineBuffer[PUBLIC_KEY_TYPE_LINE_BUFFER_SIZE];

// Amount line buffer
char amountLineBuffer[AMOUNT_LINE_BUFFER_SIZE];

// Fee line buffer
char feeLineBuffer[FEE_LINE_BUFFER_SIZE];

// Receiver line buffer
char receiverLineBuffer[RECEIVER_LINE_BUFFER_SIZE];


// Constants

// About menu version screen
static UX_STEP_NOCB(aboutMenuVersionScreen, 

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bn,
		
		{
		
			// Bold first line
			"Version",
			
			// Second line
			currencyInformation.version
		}
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
		
		{

			// Title
			.title = "Version",
			
			// Text
			.text = currencyInformation.version
		}
	#endif
);

// About menu currency screen
static UX_STEP_NOCB(aboutMenuCurrencyScreen, 

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bn,
		
		{
		
			// Bold first line
			"Currency",
			
			// Second line
			currencyInformation.name
		}
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
		
		{

			// Title
			.title = "Currency",
			
			// Text
			.text = currencyInformation.name
		}
	#endif
);

// About menu copyright screen
static UX_STEP_NOCB(aboutMenuCopyrightScreen, 

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bnnn_paging,
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
	#endif
{

	// Title
	.title = "Copyright",
	
	// Text
	.text = "(c) 2021 Nicolas Flamel. All rights reserved."
});

// About menu back screen
static UX_STEP_CB(aboutMenuBackScreen, pb, showMainMenu(ABOUT_SCREEN), {

	// Picture
	&C_icon_back,
	
	// Bold line
	"Back"
});

// About menu
static UX_FLOW(aboutMenu,

	// About menu version screen
	&aboutMenuVersionScreen,
	
	// About menu currency screen
	&aboutMenuCurrencyScreen,
	
	// About menu copyright screen
	&aboutMenuCopyrightScreen,
	
	// About menu back screen
	&aboutMenuBackScreen,
	
	// Loop
	FLOW_LOOP
);

// Main menu ready screen
static UX_STEP_NOCB(mainMenuReadyScreen, pnn, {

	// Picture
	&currencyInformation.iconDetails,
	
	// First line
	"Application",
	
	// Second line
	"is ready"
});

// Main menu about screen
static UX_STEP_CB(mainMenuAboutScreen, pb, ux_flow_init(0, aboutMenu, NULL), {

	// Picture
	&C_icon_about,
	
	// Bold line
	"About"
});

// Main menu exit screen
static UX_STEP_VALID(mainMenuExitScreen, pb, exitApplication(), {

	// Picture
	&C_icon_back,
	
	// Bold line
	"Back to dashboard"
});

// Main menu
static UX_FLOW(mainMenu,

	// Main menu ready screen
	&mainMenuReadyScreen,
	
	// Main menu about screen
	&mainMenuAboutScreen,
	
	// Main menu exit screen
	&mainMenuExitScreen,
	
	// Loop
	FLOW_LOOP
);

// Export root public key menu notify screen
static UX_STEP_NOCB(exportRootPublicKeyMenuNotifyScreen, pnn, {

	// Picture
	&C_icon_view,

	// First line
	"Export root",
	
	// Second line
	"public key?"
});

// Export root public key menu requestor screen
static UX_STEP_NOCB(exportRootPublicKeyMenuRequestorScreen,

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bn,
		
		{
		
			// Bold first line
			"Requestor",
			
			// Second line
			requestorLineBuffer
		}
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
		
		{

			// Title
			.title = "Requestor",
			
			// Text
			.text = requestorLineBuffer
		}
	#endif
);

// Export root public key menu approve screen
static UX_STEP_CB(exportRootPublicKeyMenuApproveScreen, pbb, processUserInteraction(GET_ROOT_PUBLIC_KEY_INSTRUCTION, true), {

	// Picture
	&C_icon_approve,
	
	// First bold line
	"Approve",
	
	// Second bold line
	"request"
});

// Export root public key menu deny screen
static UX_STEP_CB(exportRootPublicKeyMenuDenyScreen, pbb, processUserInteraction(GET_ROOT_PUBLIC_KEY_INSTRUCTION, false), {

	// Picture
	&C_icon_reject,
	
	// First bold line
	"Deny",
	
	// Second bold line
	"request"
});

// Export root public key menu
static UX_FLOW(exportRootPublicKeyMenu,

	// Export root public key menu notify screen
	&exportRootPublicKeyMenuNotifyScreen,
	
	// Export root public key menu requestor screen
	&exportRootPublicKeyMenuRequestorScreen,

	// Export root public key menu approve screen
	&exportRootPublicKeyMenuApproveScreen,
	
	// Export root public key menu deny screen
	&exportRootPublicKeyMenuDenyScreen,
	
	// Loop
	FLOW_LOOP
);

// Verify public key menu notify screen
static UX_STEP_NOCB(verifyPublicKeyMenuNotifyScreen, pnn, {

	// Picture
	&C_icon_view,

	// First line
	publicKeyTypeLineBuffer,
	
	// Second line
	"public key?"
});

// Verify public key menu public key screen
static UX_STEP_NOCB(verifyPublicKeyMenuPublicKeyScreen,

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bnnn_paging,
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
	#endif
{

	// Title
	.title = "Public key",
	
	// Text
	.text = publicKeyLineBuffer
});

// Verify public key menu valid screen
static UX_STEP_CB(verifyPublicKeyMenuValidScreen, pb, processUserInteraction(GET_PUBLIC_KEY_OR_ADDRESS_VERIFICATION_INSTRUCTION, true), {

	// Picture
	&C_icon_approve,
	
	// Bold line
	"Valid"
});

// Verify public key menu invalid screen
static UX_STEP_CB(verifyPublicKeyMenuInvalidScreen, pb, processUserInteraction(GET_PUBLIC_KEY_OR_ADDRESS_VERIFICATION_INSTRUCTION, false), {

	// Picture
	&C_icon_reject,
	
	// Bold line
	"Invalid"
});

// Verify public key menu
static UX_FLOW(verifyPublicKeyMenu,

	// Verify public key menu notify screen
	&verifyPublicKeyMenuNotifyScreen,
	
	// Verify public key menu public key screen
	&verifyPublicKeyMenuPublicKeyScreen,

	// Verify public key menu approve screen
	&verifyPublicKeyMenuValidScreen,
	
	// Verify public key menu deny screen
	&verifyPublicKeyMenuInvalidScreen,
	
	// Loop
	FLOW_LOOP
);

// Verify address menu notify screen
static UX_STEP_NOCB(verifyAddressMenuNotifyScreen, pnn, {

	// Picture
	&C_icon_view,

	// First line
	publicKeyTypeLineBuffer,
	
	// Second line
	"address?"
});

// Verify address menu address screen
static UX_STEP_NOCB(verifyAddressMenuAddressScreen,

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bnnn_paging,
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
	#endif
{

	// Title
	.title = "Address",
	
	// Text
	.text = publicKeyLineBuffer
});

// Verify address menu
static UX_FLOW(verifyAddressMenu,

	// Verify address menu notify screen
	&verifyAddressMenuNotifyScreen,
	
	// Verify address menu public key screen
	&verifyAddressMenuAddressScreen,

	// Verify public key menu approve screen
	&verifyPublicKeyMenuValidScreen,
	
	// Verify public key menu deny screen
	&verifyPublicKeyMenuInvalidScreen,
	
	// Loop
	FLOW_LOOP
);

// Sign Tor certificate menu notify screen
static UX_STEP_NOCB(signTorCertificateMenuNotifyScreen, pnn, {

	// Picture
	&C_icon_view,

	// First line
	"Sign temporary",
	
	// Second line
	"Tor certificate?"
});

// Sign Tor certificate menu requestor screen
static UX_STEP_NOCB(signTorCertificateMenuExpirationScreen,

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bnnn_paging,
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
	#endif
{

	// Title
	.title = "Expires",
	
	// Text
	.text = timeLineBuffer
});

// Sign Tor certificate menu public key screen
static UX_STEP_NOCB(signTorCertificateMenuPublicKeyScreen,

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bnnn_paging,
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
	#endif
{

	// Title
	.title = "Public key",
	
	// Text
	.text = publicKeyLineBuffer
});

// Sign Tor certificate menu approve screen
static UX_STEP_CB(signTorCertificateMenuApproveScreen, pbb, processUserInteraction(GET_TOR_CERTIFICATE_SIGNATURE_INSTRUCTION, true), {

	// Picture
	&C_icon_approve,
	
	// First bold line
	"Approve",
	
	// Second bold line
	"request"
});

// Sign Tor certificate menu deny screen
static UX_STEP_CB(signTorCertificateMenuDenyScreen, pbb, processUserInteraction(GET_TOR_CERTIFICATE_SIGNATURE_INSTRUCTION, false), {

	// Picture
	&C_icon_reject,
	
	// First bold line
	"Deny",
	
	// Second bold line
	"request"
});

// Sign Tor certificate menu
static UX_FLOW(signTorCertificateMenu,

	// Sign Tor certificate menu notify screen
	&signTorCertificateMenuNotifyScreen,
	
	// Sign Tor certificate menu expiration screen
	&signTorCertificateMenuExpirationScreen,
	
	// Sign Tor certificate menu public key screen
	&signTorCertificateMenuPublicKeyScreen,

	// Sign Tor certificate menu approve screen
	&signTorCertificateMenuApproveScreen,
	
	// Sign Tor certificate menu deny screen
	&signTorCertificateMenuDenyScreen,
	
	// Loop
	FLOW_LOOP
);

// Finalize transaction menu notify screen
static UX_STEP_NOCB(finalizeTransactionMenuNotifyScreen, pnn, {

	// Picture
	&C_icon_view,

	// First line
	"Finalize",
	
	// Second line
	"transaction?"
});

// Finalize transaction menu amount screen
static UX_STEP_NOCB(finalizeTransactionMenuAmountScreen,

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bnnn_paging,
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
	#endif
{

	// Title
	.title = "Amount",
	
	// Text
	.text = amountLineBuffer
});

// Finalize transaction menu fee screen
static UX_STEP_NOCB(finalizeTransactionMenuFeeScreen,

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bnnn_paging,
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
	#endif
{

	// Title
	.title = "Fee",
	
	// Text
	.text = feeLineBuffer
});

// Finalize transaction menu receiver screen
static UX_STEP_NOCB(finalizeTransactionMenuReceiverScreen,

	// Check if target is the Nano X
	#ifdef TARGET_NANOX
	
		// Layout
		bnnn_paging,
	
	// Otherwise
	#else
	
		// Layout
		nb_paging,
	#endif
{

	// Title
	.title = "To",
	
	// Text
	.text = receiverLineBuffer
});

// Finalize transaction menu no payment proof screen
static UX_STEP_NOCB(finalizeTransactionMenuNoPaymentProofScreen, nn, {

	// First line
	"No payment",
	
	// Second line
	"proof"
});

// Finalize transaction menu approve screen
static UX_STEP_CB(finalizeTransactionMenuApproveScreen, pbb, processUserInteraction(FINISH_TRANSACTION_INSTRUCTION, true), {

	// Picture
	&C_icon_approve,
	
	// First bold line
	"Approve",
	
	// Second bold line
	"request"
});

// Finalize transaction menu deny screen
static UX_STEP_CB(finalizeTransactionMenuDenyScreen, pbb, processUserInteraction(FINISH_TRANSACTION_INSTRUCTION, false), {

	// Picture
	&C_icon_reject,
	
	// First bold line
	"Deny",
	
	// Second bold line
	"request"
});

// Finalize transaction receiver menu
static UX_FLOW(finalizeTransactionReceiverMenu,

	// Finalize transaction menu notify screen
	&finalizeTransactionMenuNotifyScreen,
	
	// Finalize transaction menu amount screen
	&finalizeTransactionMenuAmountScreen,
	
	// Finalize transaction menu fee screen
	&finalizeTransactionMenuFeeScreen,
	
	// Finalize transaction menu no receiver screen
	&finalizeTransactionMenuReceiverScreen,
	
	// Finalize transaction menu approve screen
	&finalizeTransactionMenuApproveScreen,
	
	// Finalize transaction menu deny screen
	&finalizeTransactionMenuDenyScreen,
	
	// Loop
	FLOW_LOOP
);

// Finalize transaction no payment proof menu
static UX_FLOW(finalizeTransactionNoPaymentProofMenu,

	// Finalize transaction menu notify screen
	&finalizeTransactionMenuNotifyScreen,
	
	// Finalize transaction menu amount screen
	&finalizeTransactionMenuAmountScreen,
	
	// Finalize transaction menu fee screen
	&finalizeTransactionMenuFeeScreen,
	
	// Finalize transaction menu no payment proof screen
	&finalizeTransactionMenuNoPaymentProofScreen,
	
	// Finalize transaction menu approve screen
	&finalizeTransactionMenuApproveScreen,
	
	// Finalize transaction menu deny screen
	&finalizeTransactionMenuDenyScreen,
	
	// Loop
	FLOW_LOOP
);

// Processing menu screen
static UX_STEP_NOCB(processingMenuScreen, pb, {

	// Picture
	&C_icon_processing,
	
	// Bold line
	"Processing"
});

// Processing menu
static UX_FLOW(processingMenu,

	// Processing menu screen
	&processingMenuScreen,
	
	// End
	FLOW_END_STEP
);


// Supporting function implementation

// Clear menu buffers
void clearMenuBuffers(void) {

	// Clear the requestor line buffer
	explicit_bzero(requestorLineBuffer, sizeof(requestorLineBuffer));
	
	// Clear the time line buffer
	explicit_bzero(timeLineBuffer, sizeof(timeLineBuffer));
	
	// Clear the public key line buffer
	explicit_bzero(publicKeyLineBuffer, sizeof(publicKeyLineBuffer));
	
	// Clear the public key type line buffer
	explicit_bzero(publicKeyTypeLineBuffer, sizeof(publicKeyTypeLineBuffer));
	
	// Clear the amount line buffer
	explicit_bzero(amountLineBuffer, sizeof(amountLineBuffer));
	
	// Clear the fee line buffer
	explicit_bzero(feeLineBuffer, sizeof(feeLineBuffer));
	
	// Clear the receiver line buffer
	explicit_bzero(receiverLineBuffer, sizeof(receiverLineBuffer));
}

// Show main menu
void showMainMenu(enum MainMenuScreen mainMenuScreen) {

	// Check if UX stack doesn't exist
	if(!G_ux.stack_count) {
	
		// Create UX stack
		ux_stack_push();
	}
	
	// Show main menu
	ux_flow_init(0, mainMenu, NULL);
	
	// Go though all main menu screens until specified screen
	for(enum MainMenuScreen i = READY_SCREEN; i < mainMenuScreen; ++i) {
	
		// Go to next screen
		ux_flow_next();
	}
}

// Show menu
void showMenu(enum Menu menu) {

	// Initialize menu steps
	const ux_flow_step_t *const *menuSteps;
	
	// Check menu
	switch(menu) {
	
		// Main menu
		case MAIN_MENU:
		
			// Set menu steps to main menu
			menuSteps = mainMenu;
			
			// Break
			break;
		
		// About menu
		case ABOUT_MENU:
		
			// Set menu steps to about menu
			menuSteps = aboutMenu;
			
			// Break
			break;
		
		// Export root public key menu
		case EXPORT_ROOT_PUBLIC_KEY_MENU:
		
			// Set menu steps to export root public key menu
			menuSteps = exportRootPublicKeyMenu;
			
			// Break
			break;
		
		// Verify public key menu
		case VERIFY_PUBLIC_KEY_MENU:
		
			// Set menu steps to verify public key menu
			menuSteps = verifyPublicKeyMenu;
		
			// Break
			break;
		
		// Verify address menu
		case VERIFY_ADDRESS_MENU:
		
			// Set menu steps to verify address menu
			menuSteps = verifyAddressMenu;
		
			// Break
			break;
		
		// Sign Tor certificate menu
		case SIGN_TOR_CERTIFICATE_MENU:
		
			// Set menu steps to sign Tor certificate menu
			menuSteps = signTorCertificateMenu;
			
			// Break
			break;
		
		// Finalize transaction menu
		case FINALIZE_TRANSACTION_MENU:
		
			// Check if receiver line buffer isn't empty
			if(strlen(receiverLineBuffer)) {
			
				// Set menu steps to finalize transaction receiver menu
				menuSteps = finalizeTransactionReceiverMenu;
			}
			
			// Otherwise
			else {
			
				// Set menu steps to finalize transaction no payment proof menu
				menuSteps = finalizeTransactionNoPaymentProofMenu;
			}
			
			// Break
			break;
		
		// Processing menu
		case PROCESSING_MENU:
		
			// Set menu steps to processing menu
			menuSteps = processingMenu;
			
			// Break
			break;
	}
	
	// Show menu steps
	ux_flow_init(0, menuSteps, NULL);
}
