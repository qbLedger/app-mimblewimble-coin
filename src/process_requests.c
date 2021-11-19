// Header files
#include <os_io_seproxyhal.h>
#include <string.h>
#include <ux.h>
#include "common.h"
#include "continue_decrypting_mqs_data.h"
#include "continue_decrypting_slatepack_data.h"
#include "continue_encrypting_mqs_data.h"
#include "continue_encrypting_slatepack_data.h"
#include "continue_transaction_apply_offset.h"
#include "continue_transaction_get_public_key.h"
#include "continue_transaction_include_input.h"
#include "continue_transaction_include_output.h"
#include "finish_decrypting_mqs_data.h"
#include "finish_decrypting_slatepack_data.h"
#include "finish_encrypting_mqs_data.h"
#include "finish_encrypting_slatepack_data.h"
#include "finish_transaction.h"
#include "get_application_information.h"
#include "get_commitment.h"
#include "get_bulletproof_components.h"
#include "get_mqs_public_key.h"
#include "get_mqs_transaction_signature.h"
#include "get_public_key_or_address_verification.h"
#include "get_root_public_key.h"
#include "get_seed_cookie.h"
#include "get_tor_public_key.h"
#include "get_tor_certificate_signature.h"
#include "get_tor_transaction_signature.h"
#include "menus.h"
#include "process_requests.h"
#include "start_decrypting_mqs_data.h"
#include "start_decrypting_slatepack_data.h"
#include "start_encrypting_mqs_data.h"
#include "start_encrypting_slatepack_data.h"
#include "start_transaction.h"
#include "state.h"


// Constants

// Request class
static const uint8_t REQUEST_CLASS = 0xC7;


// Supporting function implementation

// Process request
void processRequest(unsigned short requestLength, volatile unsigned short *responseLength, volatile unsigned char *responseFlags) {

	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {

			// Check if request is malformed
			if(requestLength < APDU_OFF_DATA || requestLength - APDU_OFF_DATA != G_io_apdu_buffer[APDU_OFF_LC]) {
			
				// Throw malformed request error
				THROW(MALFORMED_REQUEST_ERROR);
			}
			
			// Get request's class
			const uint8_t class = G_io_apdu_buffer[APDU_OFF_CLA];
			
			// Check if request's class is unknown
			if(class != REQUEST_CLASS) {
			
				// Throw unknown class error
				THROW(UNKNOWN_CLASS_ERROR);
			}
			
			// Get request's instruction
			const enum Instruction instruction = G_io_apdu_buffer[APDU_OFF_INS];
			
			// Reset unrelated state
			resetUnrelatedState(instruction);
			
			// Check request's instruction
			switch(instruction) {
			
				// Get application information instruction
				case GET_APPLICATION_INFORMATION_INSTRUCTION:
				
					// Process get application information request
					processGetApplicationInformationRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Get root public key instruction
				case GET_ROOT_PUBLIC_KEY_INSTRUCTION:
				
					// Process get root public key request
					processGetRootPublicKeyRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Get public key or address verification instruction
				case GET_PUBLIC_KEY_OR_ADDRESS_VERIFICATION_INSTRUCTION:
				
					// Process get public key or address verification request
					processGetPublicKeyOrAddressVerificationRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Get seed cookie instruction
				case GET_SEED_COOKIE_INSTRUCTION:
				
					// Process get seed cookie request
					processGetSeedCookieRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Get commitment instruction
				case GET_COMMITMENT_INSTRUCTION:
				
					// Process get commitment request
					processGetCommitmentRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Get bulletproof components instruction
				case GET_BULLETPROOF_COMPONENTS_INSTRUCTION:
				
					// Process get bulletproof components request
					processGetBulletproofComponentsRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Get Tor public key instruction
				case GET_TOR_PUBLIC_KEY_INSTRUCTION:
				
					// Process get Tor public key request
					processGetTorPublicKeyRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Get Tor transaction signature instruction
				case GET_TOR_TRANSACTION_SIGNATURE_INSTRUCTION:
				
					// Process get Tor transaction signature request
					processGetTorTransactionSignatureRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// break
					break;
				
				// Get Tor certificate signature instruction
				case GET_TOR_CERTIFICATE_SIGNATURE_INSTRUCTION:
				
					// Process get Tor certificate signature request
					processGetTorCertificateSignatureRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// break
					break;
				
				// Start encrypting Slatepack data instruction
				case START_ENCRYPTING_SLATEPACK_DATA_INSTRUCTION:
				
					// Process start encrypting Slatepack data request
					processStartEncryptingSlatepackDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Continue encrypting Slatepack data instruction
				case CONTINUE_ENCRYPTING_SLATEPACK_DATA_INSTRUCTION:
				
					// Process continue encrypting Slatepack data request
					processContinueEncryptingSlatepackDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Finish encrypting Slatepack data instruction
				case FINISH_ENCRYPTING_SLATEPACK_DATA_INSTRUCTION:
				
					// Process finish encrypting Slatepack data request
					processFinishEncryptingSlatepackDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Start decrypting Slatepack data instruction
				case START_DECRYPTING_SLATEPACK_DATA_INSTRUCTION:
				
					// Process start decrypting Slatepack data request
					processStartDecryptingSlatepackDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Continue decrypting Slatepack data instruction
				case CONTINUE_DECRYPTING_SLATEPACK_DATA_INSTRUCTION:
				
					// Process continue decrypting Slatepack data request
					processContinueDecryptingSlatepackDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Finish decrypting Slatepack data instruction
				case FINISH_DECRYPTING_SLATEPACK_DATA_INSTRUCTION:
				
					// Process finish decrypting Slatepack data request
					processFinishDecryptingSlatepackDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Get MQS public key instruction
				case GET_MQS_PUBLIC_KEY_INSTRUCTION:
				
					// Process get MQS public key request
					processGetMqsPublicKeyRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Get MQS transaction signature instruction
				case GET_MQS_TRANSACTION_SIGNATURE_INSTRUCTION:
				
					// Process get MQS transaction signature request
					processGetMqsTransactionSignatureRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// break
					break;
					
				// Start encrypting MQS data instruction
				case START_ENCRYPTING_MQS_DATA_INSTRUCTION:
				
					// Process start encrypting MQS data request
					processStartEncryptingMqsDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Continue encrypting MQS data instruction
				case CONTINUE_ENCRYPTING_MQS_DATA_INSTRUCTION:
				
					// Process continue encrypting MQS data request
					processContinueEncryptingMqsDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Finish encrypting MQS data instruction
				case FINISH_ENCRYPTING_MQS_DATA_INSTRUCTION:
				
					// Process finish encrypting MQS data request
					processFinishEncryptingMqsDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Start decrypting MQS data instruction
				case START_DECRYPTING_MQS_DATA_INSTRUCTION:
				
					// Process start decrypting MQS data request
					processStartDecryptingMqsDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Continue decrypting MQS data instruction
				case CONTINUE_DECRYPTING_MQS_DATA_INSTRUCTION:
				
					// Process continue decrypting MQS data request
					processContinueDecryptingMqsDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Finish decrypting MQS data instruction
				case FINISH_DECRYPTING_MQS_DATA_INSTRUCTION:
				
					// Process finish decrypting MQS data request
					processFinishDecryptingMqsDataRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
				
					// Break
					break;
				
				// Start transaction instruction
				case START_TRANSACTION_INSTRUCTION:
				
					// Process start transaction request
					processStartTransactionRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Continue transaction include output instruction
				case CONTINUE_TRANSACTION_INCLUDE_OUTPUT_INSTRUCTION:
				
					// Process continue transaction include output request
					processContinueTransactionIncludeOutputRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Continue transaction include input instruction
				case CONTINUE_TRANSACTION_INCLUDE_INPUT_INSTRUCTION:
				
					// Process continue transaction include input request
					processContinueTransactionIncludeInputRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Continue transaction apply offset instruction
				case CONTINUE_TRANSACTION_APPLY_OFFSET_INSTRUCTION:
				
					// Process continue transaction apply offset request
					processContinueTransactionApplyOffsetRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Continue transaction get public key instruction
				case CONTINUE_TRANSACTION_GET_PUBLIC_KEY_INSTRUCTION:
				
					// Process continue transaction get public key request
					processContinueTransactionGetPublicKeyRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;
				
				// Finish transaction instruction
				case FINISH_TRANSACTION_INSTRUCTION:
				
					// Process finish transaction request
					processFinishTransactionRequest((unsigned short *)responseLength, (unsigned char *)responseFlags);
					
					// Break
					break;

				// Default
				default:
				
					// Throw unknown instruction error
					THROW(UNKNOWN_INSTRUCTION_ERROR);
			}
		}
		
		// Catch IO reset error
		CATCH(EXCEPTION_IO_RESET) {
		
			// Throw IO reset error
			THROW(EXCEPTION_IO_RESET);
		}
		
		// Catch length error
		CATCH(ERR_APD_LEN) {
		
			// Throw length error
			THROW(ERR_APD_LEN);
		}
		
		// Catch other errors
		CATCH_OTHER(error) {
		
			// Check error type
			switch(error & ERROR_TYPE_MASK) {
			
				// Application errors or success
				case ERR_APP_RANGE_01:
				case ERR_APP_RANGE_02:
				case ERR_APP_RANGE_03:
				case ERR_APP_RANGE_04:
				case SWO_SUCCESS:
				
					// Throw error
					THROW(error);
				
				// Default
				default:
		
					// Throw internal error error
					THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
		
		}
	}
	
	// End try
	END_TRY;
}

// Process user interaction
void processUserInteraction(size_t instruction, bool isApprovedResult) {
	
	// Clear menu buffers
	clearMenuBuffers();

	// Initialize response length
	volatile unsigned short responseLength = 0;
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Check if user approved the interaction
			if(isApprovedResult) {
			
				// Show processing menu
				showMenu(PROCESSING_MENU);
				
				// Wait for display to update
				UX_WAIT_DISPLAYED();
			
				// Check instruction
				switch(instruction) {
				
					// Get root public key instruction
					case GET_ROOT_PUBLIC_KEY_INSTRUCTION:
					
						// Process get root public key user interaction
						processGetRootPublicKeyUserInteraction((unsigned short *)&responseLength);
						
						// Break
						break;
					
					// Get public key or address verification instruction
					case GET_PUBLIC_KEY_OR_ADDRESS_VERIFICATION_INSTRUCTION:
					
						// Process get public key or address verification user interaction
						processGetPublicKeyOrAddressVerificationUserInteraction((unsigned short *)&responseLength);
						
						// Break
						break;
					
					// Get Tor certificate signature instruction
					case GET_TOR_CERTIFICATE_SIGNATURE_INSTRUCTION:
					
						// Process get Tor certificate signature user interaction
						processGetTorCertificateSignatureUserInteraction((unsigned short *)&responseLength);
					
						// break
						break;
					
					// Finish transaction instruction
					case FINISH_TRANSACTION_INSTRUCTION:
					
						// Process finish transaction user interaction
						processFinishTransactionUserInteraction((unsigned short *)&responseLength);
						
						// Break
						break;
				}
			}
			
			// Otherwise
			else {
			
				// Throw user rejected error
				THROW(USER_REJECTED_ERROR);
			}
		}
		
		// Catch IO reset error
		CATCH(EXCEPTION_IO_RESET) {
		
			// Throw IO reset error
			THROW(EXCEPTION_IO_RESET);
		}
		
		// Catch length error
		CATCH(ERR_APD_LEN) {
		
			// Throw length error
			THROW(ERR_APD_LEN);
		}
		
		// Catch other errors
		CATCH_OTHER(error) {
		
			// Check error type
			switch(error & ERROR_TYPE_MASK) {
				
				// Default
				default:
				
					// Set error to internal error error
					error = INTERNAL_ERROR_ERROR;
				
				// Application errors or success
				case ERR_APP_RANGE_01:
				case ERR_APP_RANGE_02:
				case ERR_APP_RANGE_03:
				case ERR_APP_RANGE_04:
				case SWO_SUCCESS:
				
					// Check if response with the error will overflow
					if(willResponseOverflow(responseLength, sizeof(uint16_t))) {
				
						// Throw length error
						THROW(ERR_APD_LEN);
					}
					
					// Otherwise
					else {
				
						// Append error to response
						U2BE_ENCODE(G_io_apdu_buffer, responseLength, error);
						
						responseLength += sizeof(uint16_t);
						
						// Send response
						io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, responseLength);
						
						// Break
						break;
					}
			}
		}
		
		// Finally
		FINALLY {
		
		}
	}
	
	// End try
	END_TRY;
	
	// Show main menu's ready screen
	showMainMenu(READY_SCREEN);
}
