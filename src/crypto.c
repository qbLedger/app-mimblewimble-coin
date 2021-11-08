// Header files
#include <os.h>
#include <string.h>
#include "blake2b.h"
#include "crypto.h"
#include "currency_information.h"
#include "mqs.h"
#include "secp256k1_aggsig.h"
#include "secp256k1_bulletproofs.h"
#include "secp256k1_preallocated.h"
#include "tor.h"


// Constants

// Nonce size
const size_t NONCE_SIZE = 32;

// Commitment size
const size_t COMMITMENT_SIZE = 33;

// Uncompressed public key size
const size_t UNCOMPRESSED_PUBLIC_KEY_SIZE = 65;

// Identifier maximum depth
const size_t IDENTIFIER_MAXIMUM_DEPTH = 4;

// Identifier size
const size_t IDENTIFIER_SIZE = sizeof(uint8_t) + IDENTIFIER_MAXIMUM_DEPTH * sizeof(uint32_t);

// Single-signer compact signature size
const size_t SINGLE_SIGNER_COMPACT_SIGNATURE_SIZE = 64;

// Single-signer message size
const size_t SINGLE_SIGNER_MESSAGE_SIZE = 32;

// Maximum DER signature size
const size_t MAXIMUM_DER_SIGNATURE_SIZE = 72;

// Keep private
const int KEEP_PRIVATE_KEY = 1;

// Even compressed public key prefix
const uint8_t EVEN_COMPRESSED_PUBLIC_KEY_PREFIX = 0x02;

// Odd compressed public key prefix
const uint8_t ODD_COMPRESSED_PUBLIC_KEY_PREFIX = 0x03;

// Uncompressed public key prefix
const uint8_t UNCOMPRESSED_PUBLIC_KEY_PREFIX = 0x04;

// Ed25519 compressed public key prefix
const uint8_t ED25519_COMPRESSED_PUBLIC_KEY_PREFIX = 0x02;

// Ed25519 signature size
const size_t ED25519_SIGNATURE_SIZE = 64;

// Secp256k1 curve order
const uint8_t SECP256K1_CURVE_ORDER[CURVE_ORDER_SIZE] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xBA, 0xAE, 0xDC, 0xE6, 0xAF, 0x48, 0xA0, 0x3B, 0xBF, 0xD2, 0x5E, 0x8C, 0xD0, 0x36, 0x41, 0x41
};

// Secp256k1 curve order half
const uint8_t SECP256k1_CURVE_ORDER_HALF[CURVE_ORDER_SIZE] = {
	0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5D, 0x57, 0x6E, 0x73, 0x57, 0xA4, 0x50, 0x1D, 0xDF, 0xE9, 0x2F, 0x46, 0x68, 0x1B, 0x20, 0xA0
};

// Ed25519 curve order
static const uint8_t ED25519_CURVE_ORDER[] = {
	0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xED
};

// Hardened path mask
static const uint32_t HARDENED_PATH_MASK = 0x80000000;

// Maximum account
const uint32_t MAXIMUM_ACCOUNT = HARDENED_PATH_MASK - 1;

// X25519 public key size
const size_t X25519_PUBLIC_KEY_SIZE = 32;

// X25519 compressed public key prefix
const uint8_t X25519_COMPRESSED_PUBLIC_KEY_PREFIX = 0x02;

// Generator G
const secp256k1_generator GENERATOR_G = {
	{
		0x79, 0xBE, 0x66, 0x7E, 0xF9, 0xDC, 0xBB, 0xAC, 0x55, 0xA0, 0x62, 0x95, 0xCE, 0x87, 0x0B, 0x07, 0x02, 0x9B, 0xFC, 0xDB, 0x2D, 0xCE, 0x28, 0xD9, 0x59, 0xF2, 0x81, 0x5B, 0x16, 0xF8, 0x17, 0x98, 0x48, 0x3A, 0xDA, 0x77, 0x26, 0xA3, 0xC4, 0x65, 0x5D, 0xA4, 0xFB, 0xFC, 0x0E, 0x11, 0x08, 0xA8, 0xFD, 0x17, 0xB4, 0x48, 0xA6, 0x85, 0x54, 0x19, 0x9C, 0x47, 0xD0, 0x8F, 0xFB, 0x10, 0xD4, 0xB8
	}
};

// Generator H
static const secp256k1_generator GENERATOR_H = {
	{
		0x50, 0x92, 0x9B, 0x74, 0xC1, 0xA0, 0x49, 0x54, 0xB7, 0x8B, 0x4B, 0x60, 0x35, 0xE9, 0x7A, 0x5E, 0x07, 0x8A, 0x5A, 0x0F, 0x28, 0xEC, 0x96, 0xD5, 0x47, 0xBF, 0xEE, 0x9A, 0xCE, 0x80, 0x3A, 0xC0, 0x31, 0xD3, 0xC6, 0x86, 0x39, 0x73, 0x92, 0x6E, 0x04, 0x9E, 0x63, 0x7C, 0xB1, 0xB5, 0xF4, 0x0A, 0x36, 0xDA, 0xC2, 0x8A, 0xF1, 0x76, 0x69, 0x68, 0xC3, 0x0C, 0x23, 0x13, 0xF3, 0xA3, 0x89, 0x04
	}
};

// Node size
static const size_t NODE_SIZE = 64;

// Chain code size
static const size_t CHAIN_CODE_SIZE = 32;

// Single-signer seed size
static const size_t SINGLE_SIGNER_SEED_SIZE = 32;

// BIP32 path without coin type and account
static const uint32_t BIP32_PATH_WITHOUT_COIN_TYPE_AND_ACCOUNT[] = {

	// Purpose
	44 | HARDENED_PATH_MASK,
	
	// Coin type
	HARDENED_PATH_MASK,
	
	// Account
	HARDENED_PATH_MASK,
	
	// Change
	0,
	
	// Address index
	0
};

// BIP32 path coin type index
static const size_t BIP32_PATH_COIN_TYPE_INDEX = 1;

// BIP32 path account index
static const size_t BIP32_PATH_ACCOUNT_INDEX = 2;

// Seed key
static const char SEED_KEY[] = "IamVoldemort";

// Generator J public
static const secp256k1_pubkey GENERATOR_J_PUBLIC = {
	{
		0x5F, 0x15, 0x21, 0x36, 0x93, 0x93, 0x01, 0x2A, 0x8D, 0x8B, 0x39, 0x7E, 0x9B, 0xF4, 0x54, 0x29, 0x2F, 0x5A, 0x1B, 0x3D, 0x38, 0x85, 0x16, 0xC2, 0xF3, 0x03, 0xFC, 0x95, 0x67, 0xF5, 0x60, 0xB8, 0x3A, 0xC4, 0xC5, 0xA6, 0xDC, 0xA2, 0x01, 0x59, 0xFC, 0x56, 0xCF, 0x74, 0x9A, 0xA6, 0xA5, 0x65, 0x31, 0x6A, 0xA5, 0x03, 0x74, 0x42, 0x3F, 0x42, 0x53, 0x8F, 0xAA, 0x2C, 0xD3, 0x09, 0x3F, 0xA4
	}
};

// Address private key blinding factor value
static uint64_t ADDRESS_PRIVATE_KEY_BLINDING_FACTOR_VALUE = 713;

// Address private key hash key
static const char ADDRESS_PRIVATE_KEY_HASH_KEY[] = "Grinbox_seed";

// Secp256k1 private key size
static const size_t SECP256k1_PRIVATE_KEY_SIZE = 32;

// Bits proven per range
static const size_t BITS_PROVEN_PER_RANGE = sizeof(uint64_t) * BITS_IN_A_BYTE;

// Number of generators tau x
static const size_t NUMBER_OF_GENERATORS_TAU_X = 256;

// Number of generators t one and t two
static const size_t NUMBER_OF_GENERATORS_T_ONE_AND_T_TWO = 128;

// Scratch space size
static const size_t SCRATCH_SPACE_SIZE = 132;


// Supporting function implementation

// Get private key and chain code
void getPrivateKeyAndChainCode(volatile cx_ecfp_private_key_t *privateKey, volatile uint8_t *chainCode, uint32_t account) {

	// Check if account is invalid
	if(account > MAXIMUM_ACCOUNT) {
	
		// Throw invalid parameters error
		THROW(INVALID_PARAMETERS_ERROR);
	}

	// Copy BIP32 path without coin type and account
	uint32_t bip32Path[ARRAYLEN(BIP32_PATH_WITHOUT_COIN_TYPE_AND_ACCOUNT)];
	memcpy(bip32Path, BIP32_PATH_WITHOUT_COIN_TYPE_AND_ACCOUNT, sizeof(BIP32_PATH_WITHOUT_COIN_TYPE_AND_ACCOUNT));
	
	// Set BIP32 path's coin type
	bip32Path[BIP32_PATH_COIN_TYPE_INDEX] |= currencyInformation.bip44CoinType;
	
	// Set BIP32 path's account
	bip32Path[BIP32_PATH_ACCOUNT_INDEX] |= account;

	// Initialize node
	volatile uint8_t node[NODE_SIZE];

	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {

			// Derive node and chain code from path and seed key
			os_perso_derive_node_with_seed_key(HDW_NORMAL, CX_CURVE_SECP256K1, bip32Path, ARRAYLEN(bip32Path), (uint8_t *)node, (uint8_t *)chainCode, (unsigned char *)SEED_KEY, sizeof(SEED_KEY) - sizeof((char)'\0'));
			
			// Get private key from node
			cx_ecfp_init_private_key(CX_CURVE_SECP256K1, (uint8_t *)node, sizeof(privateKey->d), (cx_ecfp_private_key_t *)privateKey);
			
			// Check if private key isn't a valid secret key
			if(!isValidSecp256k1PrivateKey((uint8_t *)privateKey->d, privateKey->d_len)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
		
			// Clear the node
			explicit_bzero((uint8_t *)node, sizeof(node));
		}
	}
	
	// End try
	END_TRY;
}

// Get public key from private key
void getPublicKeyFromPrivateKey(uint8_t *publicKey, const cx_ecfp_private_key_t *privateKey) {
	
	// Get uncompressed public key from the private key
	cx_ecfp_public_key_t uncompressedPublicKey;
	cx_ecfp_generate_pair(CX_CURVE_SECP256K1, &uncompressedPublicKey, (cx_ecfp_private_key_t *)privateKey, KEEP_PRIVATE_KEY);
	
	// Set prefix in the public key
	publicKey[0] = (uncompressedPublicKey.W[uncompressedPublicKey.W_len - 1] & 1) ? ODD_COMPRESSED_PUBLIC_KEY_PREFIX : EVEN_COMPRESSED_PUBLIC_KEY_PREFIX;
	
	// Set X part in the public key
	memcpy(&publicKey[PUBLIC_KEY_PREFIX_SIZE], &uncompressedPublicKey.W[PUBLIC_KEY_PREFIX_SIZE], COMPRESSED_PUBLIC_KEY_SIZE - PUBLIC_KEY_PREFIX_SIZE);
}

// Derive child key
void deriveChildKey(volatile cx_ecfp_private_key_t *privateKey, volatile uint8_t *chainCode, uint32_t account, const uint32_t *path, size_t pathLength, bool useProvidedPrivateKeyAndChainCode) {

	// Check if not using the provided private key and chain code
	if(!useProvidedPrivateKeyAndChainCode) {

		// Get private key and chain code
		getPrivateKeyAndChainCode(privateKey, chainCode, account);
	}
	
	// Go through the path
	for(size_t i = 0; i < pathLength; ++i) {
	
		// Initialize data
		volatile uint8_t data[COMPRESSED_PUBLIC_KEY_SIZE + sizeof(path[i])];
		
		// Initialize node
		volatile uint8_t node[NODE_SIZE];
		
		// Initialize new private key
		volatile cx_ecfp_private_key_t newPrivateKey;
		
		// Begin try
		BEGIN_TRY {
		
			// Try
			TRY {
			
				// Check if path is hardened
				if(path[i] & HARDENED_PATH_MASK) {
				
					// Set the first part of data to zero
					data[0] = 0;
					
					// Append private key to the data
					memcpy((uint8_t *)&data[sizeof(data[0])], (uint8_t *)privateKey->d, privateKey->d_len);
				}
				
				// Otherwise
				else {
				
					// Change the private key's curve
					cx_curve_t curve = privateKey->curve;
					privateKey->curve = CX_CURVE_SECP256K1;
				
					// Get compressed public key from the private key set it in the data
					getPublicKeyFromPrivateKey((uint8_t *)data, (cx_ecfp_private_key_t *)privateKey);
					
					// Restore the private key's curve
					privateKey->curve = curve;
				}
				
				// Append the path to the data
				U4BE_ENCODE((uint8_t *)data, COMPRESSED_PUBLIC_KEY_SIZE, path[i]);
				
				// Get the path's node as the HMAC-SHA512 of the data with the chain code as the key
				cx_hmac_sha512((uint8_t *)chainCode, CHAIN_CODE_SIZE, (uint8_t *)data, sizeof(data), (uint8_t *)node, sizeof(node));
				
				// Get new private key from node
				cx_ecfp_init_private_key(privateKey->curve, (uint8_t *)node, sizeof(newPrivateKey.d), (cx_ecfp_private_key_t *)&newPrivateKey);
				
				// Check if new private key isn't a valid private key
				if(!isValidSecp256k1PrivateKey((uint8_t *)newPrivateKey.d, newPrivateKey.d_len)) {
				
					// Throw internal error error
					THROW(INTERNAL_ERROR_ERROR);
				}
				
				// Add private key to the new private key
				cx_math_addm((uint8_t *)newPrivateKey.d, (uint8_t *)newPrivateKey.d, (uint8_t *)privateKey->d, SECP256K1_CURVE_ORDER, newPrivateKey.d_len);
				
				// Check if new private key isn't a valid private key
				if(!isValidSecp256k1PrivateKey((uint8_t *)newPrivateKey.d, newPrivateKey.d_len)) {
				
					// Throw internal error error
					THROW(INTERNAL_ERROR_ERROR);
				}
				
				// Get chain code from the node
				memcpy((uint8_t *)chainCode, (uint8_t *)&node[sizeof(newPrivateKey.d)], CHAIN_CODE_SIZE);
				
				// Set private key to the new private key
				memcpy((cx_ecfp_private_key_t *)privateKey, (cx_ecfp_private_key_t *)&newPrivateKey, sizeof(newPrivateKey));
			}
		
			// Finally
			FINALLY {
			
				// Clear the data
				explicit_bzero((uint8_t *)data, sizeof(data));
				
				// Clear the node
				explicit_bzero((uint8_t *)node, sizeof(node));
				
				// Clear the new private key
				explicit_bzero((cx_ecfp_private_key_t *)&newPrivateKey, sizeof(newPrivateKey));
			}
		}
		
		// End try
		END_TRY;
	}
}

// Derive blinding factor
void deriveBlindingFactor(volatile uint8_t *blindingFactor, uint32_t account, uint64_t value, const uint32_t *path, size_t pathLength, enum SwitchType switchType) {

	// Initialize child private key and chain code
	volatile cx_ecfp_private_key_t childPrivateKey;
	volatile uint8_t childChainCode[CHAIN_CODE_SIZE];
	
	// Create context
	volatile uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY)];
	volatile secp256k1_context *context = secp256k1_context_preallocated_create((uint8_t *)contextBuffer, SECP256K1_CONTEXT_VERIFY);
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Derive child's private key and chain code at path
			deriveChildKey(&childPrivateKey, childChainCode, account, path, pathLength, false);
			
			// Check switch type
			switch(switchType) {
			
				// No switch type
				case NO_SWITCH_TYPE:
				
					// Set blinding factor to the child's private key
					memcpy((uint8_t *)blindingFactor, (uint8_t *)childPrivateKey.d, childPrivateKey.d_len);
				
					// Break
					break;
				
				// Regular switch type
				case REGULAR_SWITCH_TYPE:
				
					// Check if performing blind switch with the child's private key and the value failed
					if(!secp256k1_blind_switch((secp256k1_context *)context, (uint8_t *)blindingFactor, (uint8_t *)childPrivateKey.d, value, &GENERATOR_H, &GENERATOR_G, &GENERATOR_J_PUBLIC)) {
					
						// Throw internal error error
						THROW(INTERNAL_ERROR_ERROR);
					}
				
					// Break
					break;
			}
		}
		
		// Finally
		FINALLY {
		
			// Clear the child private key and chain code
			explicit_bzero((cx_ecfp_private_key_t *)&childPrivateKey, sizeof(childPrivateKey));
			explicit_bzero((uint8_t *)childChainCode, sizeof(childChainCode));
			
			// Destroy context
			secp256k1_context_preallocated_destroy((secp256k1_context *)context);
			explicit_bzero((uint8_t *)contextBuffer, sizeof(contextBuffer));
		}
	}
	
	// End try
	END_TRY;
}

// Commit value
void commitValue(volatile uint8_t *commitment, uint64_t value, const uint8_t *blindingFactor) {

	// Create context
	volatile uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY)];
	volatile secp256k1_context *context = secp256k1_context_preallocated_create((uint8_t *)contextBuffer, SECP256K1_CONTEXT_VERIFY);
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
	
			// Check if performing Pedersen commit with the value and the blinding factor failed
			secp256k1_pedersen_commitment commitmentValue;
			if(!secp256k1_pedersen_commit((secp256k1_context *)context, &commitmentValue, blindingFactor, value, &GENERATOR_H, &GENERATOR_G)) {
				
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
			
			// Check if serializing commitment failed
			if(!secp256k1_pedersen_commitment_serialize((secp256k1_context *)context, (uint8_t *)commitment, &commitmentValue)) {
				
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
		
			// Destroy context
			secp256k1_context_preallocated_destroy((secp256k1_context *)context);
			explicit_bzero((uint8_t *)contextBuffer, sizeof(contextBuffer));
		}
	}
	
	// End try
	END_TRY;
}

// Get rewind nonce
void getRewindNonce(volatile uint8_t *rewindNonce, uint32_t account, const uint8_t *commitment) {

	// Initialize child private key
	volatile cx_ecfp_private_key_t privateKey;
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {

			// Get private key
			getPrivateKeyAndChainCode(&privateKey, NULL, account);

			// Get public key from the private key
			uint8_t publicKey[COMPRESSED_PUBLIC_KEY_SIZE];
			getPublicKeyFromPrivateKey(publicKey, (cx_ecfp_private_key_t *)&privateKey);
			
			// Get rewind hash from the public key
			uint8_t rewindHash[NONCE_SIZE];
			getBlake2b(rewindHash, sizeof(rewindHash), publicKey, sizeof(publicKey), NULL, 0);
			
			// Get rewind nonce from the rewind hash and the commitment
			getBlake2b((uint8_t *)rewindNonce, NONCE_SIZE, rewindHash, sizeof(rewindHash), commitment, COMMITMENT_SIZE);
			
			// Check if rewind nonce isn't a valid secret key
			if(!isValidSecp256k1PrivateKey((uint8_t *)rewindNonce, NONCE_SIZE)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
		
			// Clear the private key
			explicit_bzero((cx_ecfp_private_key_t *)&privateKey, sizeof(privateKey));
		}
	}
	
	// End try
	END_TRY;
}

// Get private nonce
void getPrivateNonce(volatile uint8_t *privateNonce, uint32_t account, const uint8_t *commitment) {

	// Initialize child private key and chain code
	volatile cx_ecfp_private_key_t childPrivateKey;
	volatile uint8_t childChainCode[CHAIN_CODE_SIZE];
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Derive child's private key and chain code at root path
			deriveChildKey(&childPrivateKey, childChainCode, account, NULL, 0, false);
			
			// Get private hash from the child's private key
			uint8_t privateHash[NONCE_SIZE];
			getBlake2b(privateHash, sizeof(privateHash), (uint8_t *)childPrivateKey.d, childPrivateKey.d_len, NULL, 0);
			
			// Get private nonce from the private hash and the commitment
			getBlake2b((uint8_t *)privateNonce, NONCE_SIZE, privateHash, sizeof(privateHash), commitment, COMMITMENT_SIZE);
			
			// Check if private nonce isn't a valid secret key
			if(!isValidSecp256k1PrivateKey((uint8_t *)privateNonce, NONCE_SIZE)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
		
			// Clear the child private key and chain code
			explicit_bzero((cx_ecfp_private_key_t *)&childPrivateKey, sizeof(childPrivateKey));
			explicit_bzero((uint8_t *)childChainCode, sizeof(childChainCode));
		}
	}
	
	// End try
	END_TRY;
}

// Get address private key
void getAddressPrivateKey(volatile cx_ecfp_private_key_t *addressPrivateKey, uint32_t account, uint32_t index, cx_curve_t curve) {

	// Initialize blinding factor
	volatile uint8_t blindingFactor[BLINDING_FACTOR_SIZE];
	
	// Initialize node
	volatile uint8_t node[NODE_SIZE];
	
	// Initialize chain code
	volatile uint8_t chainCode[CHAIN_CODE_SIZE];
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Derive blinding factor from the address private key blinding factor value and the root path
			deriveBlindingFactor(blindingFactor, account, ADDRESS_PRIVATE_KEY_BLINDING_FACTOR_VALUE, NULL, 0, REGULAR_SWITCH_TYPE);
			
			// Get the node as the HMAC-SHA512 of the blinding factor with the addres private key hash key as the key
			cx_hmac_sha512((uint8_t *)ADDRESS_PRIVATE_KEY_HASH_KEY, sizeof(ADDRESS_PRIVATE_KEY_HASH_KEY), (uint8_t *)blindingFactor, sizeof(blindingFactor), (uint8_t *)node, sizeof(node));
			
			// Get address private key from node
			cx_ecfp_init_private_key(curve, (uint8_t *)node, sizeof(addressPrivateKey->d), (cx_ecfp_private_key_t *)addressPrivateKey);
			
			// Check if address private key isn't a valid private key
			if(!isValidSecp256k1PrivateKey((uint8_t *)addressPrivateKey->d, addressPrivateKey->d_len)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
			
			// Get chain code from the node
			memcpy((uint8_t *)chainCode, (uint8_t *)&node[sizeof(addressPrivateKey->d)], CHAIN_CODE_SIZE);
			
			// Derive child key from the address private key and chain code at the index
			deriveChildKey(addressPrivateKey, chainCode, account, &index, 1, true);
		}
		
		// Finally
		FINALLY {
		
			// Clear the blinding factor
			explicit_bzero((uint8_t *)blindingFactor, sizeof(blindingFactor));
			
			// Clear the node
			explicit_bzero((uint8_t *)node, sizeof(node));
			
			// Clear the chain code
			explicit_bzero((uint8_t *)chainCode, sizeof(chainCode));
		}
	}
	
	// End try
	END_TRY;
}

// Update blinding factor sum
void updateBlindingFactorSum(volatile uint8_t *blindingFactorSum, const uint8_t *blindingFactor, bool blindingFactorIsPositive) {

	// Create context
	volatile uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE)];
	volatile secp256k1_context *context = secp256k1_context_preallocated_create((uint8_t *)contextBuffer, SECP256K1_CONTEXT_NONE);
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Initialize blinding factors
			const uint8_t *blindingFactors[] = {
			
				// Blinding factor sum
				(uint8_t *)blindingFactorSum,
				
				// Blinding factor
				blindingFactor
			}; 
	
			// Check if performing Pedersen blind sum with the blinding factor sum and the blinding factor failed
			if(!secp256k1_pedersen_blind_sum((secp256k1_context *)context, (uint8_t *)blindingFactorSum, blindingFactors, ARRAYLEN(blindingFactors), ARRAYLEN(blindingFactors) - (blindingFactorIsPositive ? 0 : 1))) {
				
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
			
			// Check if blinding factor sum isn't a zero blinding factor
			if(!isZeroArray((uint8_t *)blindingFactorSum, BLINDING_FACTOR_SIZE)) {
			
				// Check if blinding factor sum isn't a valid secret key
				if(!isValidSecp256k1PrivateKey((uint8_t *)blindingFactorSum, BLINDING_FACTOR_SIZE)) {
				
					// Throw invalid parameters error
					THROW(INVALID_PARAMETERS_ERROR);
				}
			}
		}
		
		// Finally
		FINALLY {
		
			// Destroy context
			secp256k1_context_preallocated_destroy((secp256k1_context *)context);
			explicit_bzero((uint8_t *)contextBuffer, sizeof(contextBuffer));
		}
	}
	
	// End try
	END_TRY;
}

// Create single-signer signature
void createSingleSignerSignature(volatile uint8_t *signature, const uint8_t *message, const cx_ecfp_private_key_t *privateKey, const uint8_t *secretNonce, const uint8_t *publicNonce, const uint8_t *publicKey) {

	// Create context
	volatile uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN)];
	volatile secp256k1_context *context = secp256k1_context_preallocated_create((uint8_t *)contextBuffer, SECP256K1_CONTEXT_SIGN);
	
	// Initialize seed
	volatile uint8_t seed[SINGLE_SIGNER_SEED_SIZE];
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Create random seed
			cx_rng((uint8_t *)seed, sizeof(seed));
			
			// Check if a public nonce is provided
			secp256k1_pubkey publicNonceData;
			if(publicNonce) {
			
				// Check if parsing public nonce failed
				if(!secp256k1_ec_pubkey_parse((secp256k1_context *)context, &publicNonceData, publicNonce, COMPRESSED_PUBLIC_KEY_SIZE)) {
				
					// Throw invalid parameters error
					THROW(INVALID_PARAMETERS_ERROR);
				}
				
				// Check if public nonce is a zero array
				if(isZeroArray(publicNonceData.data, sizeof(publicNonceData.data))) {
				
					// Throw invalid parameters error
					THROW(INVALID_PARAMETERS_ERROR);
				}
			}
			
			// Check if parsing public key failed
			secp256k1_pubkey publicKeyData;
			if(!secp256k1_ec_pubkey_parse((secp256k1_context *)context, &publicKeyData, publicKey, COMPRESSED_PUBLIC_KEY_SIZE)) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Check if public key is a zero array
			if(isZeroArray(publicKeyData.data, sizeof(publicKeyData.data))) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Check if creating single-signer signature failed
			secp256k1_ecdsa_signature signatureData;
			if(!secp256k1_aggsig_sign_single((secp256k1_context *)context, signatureData.data, message, privateKey->d, secretNonce, NULL, publicNonce ? &publicNonceData : NULL, publicNonce ? &publicNonceData : NULL, &publicKeyData, (uint8_t *)seed)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
			
			// Check if serializing signature failed
			if(!secp256k1_ecdsa_signature_serialize_compact((secp256k1_context *)context, (uint8_t *)signature, &signatureData)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
		
			// Clear the seed
			explicit_bzero((uint8_t *)seed, sizeof(seed));
		
			// Destroy context
			secp256k1_context_preallocated_destroy((secp256k1_context *)context);
			explicit_bzero((uint8_t *)contextBuffer, sizeof(contextBuffer));
		}
	}
	
	// End try
	END_TRY;
}

// Get encrypted data length
size_t getEncryptedDataLength(size_t dataLength) {

	// Return encrypted data length
	return dataLength + ((dataLength % CX_AES_BLOCK_SIZE) ? (CX_AES_BLOCK_SIZE - dataLength % CX_AES_BLOCK_SIZE) : CX_AES_BLOCK_SIZE);
}

// Encrypt data
void encryptData(volatile uint8_t *result, const uint8_t *data, size_t dataLength, const uint8_t *key, size_t keyLength) {

	// Pad the data
	uint8_t paddedData[getEncryptedDataLength(dataLength)];
	memcpy(paddedData, data, dataLength);
	memset(&paddedData[dataLength], sizeof(paddedData) - dataLength, sizeof(paddedData) - dataLength);
	
	// Initialize encryption key
	volatile cx_aes_key_t encryptionKey;
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Initialize the encryption key with the key
			cx_aes_init_key(key, keyLength, (cx_aes_key_t *)&encryptionKey);
			
			// Encrypt the padded data with the encryption key
			cx_aes((cx_aes_key_t *)&encryptionKey, CX_ENCRYPT | CX_PAD_NONE | CX_CHAIN_CBC | CX_LAST, paddedData, sizeof(paddedData), (uint8_t *)result, sizeof(paddedData));
		}
		
		// Finally
		FINALLY {
		
			// Clear the encryption key
			explicit_bzero((cx_aes_key_t *)&encryptionKey, sizeof(encryptionKey));
		}
	}
	
	// End try
	END_TRY;
}

// Get X25519 private key from Ed25519 private key
void getX25519PrivateKeyFromEd25519PrivateKey(volatile cx_ecfp_private_key_t *x25519PrivateKey, const cx_ecfp_private_key_t *ed25519PrivateKey) {

	// Initialize hash
	volatile uint8_t hash[CX_SHA512_SIZE];

	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {

			// Get hash of the Ed25519 private key
			cx_hash_sha512(ed25519PrivateKey->d, ed25519PrivateKey->d_len, (uint8_t *)hash, sizeof(hash));
			
			// Clamp the hash
			hash[0] &= 0b11111000;
			hash[31] &= 0b01111111;
			hash[31] |= 0b01000000;
			
			// Swap the hash's endianness
			swapEndianness((uint8_t *)hash, sizeof(x25519PrivateKey->d));
			
			// Get X25519 private key from the hash
			cx_ecfp_init_private_key(CX_CURVE_Curve25519, (uint8_t *)hash, sizeof(x25519PrivateKey->d), (cx_ecfp_private_key_t *)x25519PrivateKey);
		}
		
		// Finally
		FINALLY {
		
			// Clear the hash
			explicit_bzero((uint8_t *)hash, sizeof(hash));
		}
	}
	
	// End try
	END_TRY;
}

// Get X25519 public key from Ed25519 public key
void getX25519PublicKeyFromEd25519PublicKey(uint8_t *x25519PublicKey, const uint8_t *ed25519PublicKey) {

	// Uncompress the Ed25519 public key
	uint8_t uncompressedEd25519PublicKey[UNCOMPRESSED_PUBLIC_KEY_SIZE];
	uncompressedEd25519PublicKey[0] = ED25519_COMPRESSED_PUBLIC_KEY_PREFIX;
	memcpy(&uncompressedEd25519PublicKey[PUBLIC_KEY_PREFIX_SIZE], ed25519PublicKey, ED25519_PUBLIC_KEY_SIZE);
	
	cx_edwards_decompress_point(CX_CURVE_Ed25519, uncompressedEd25519PublicKey, sizeof(uncompressedEd25519PublicKey));
	
	// Get uncompressed Ed25519 public key's y value
	uint8_t *y = &uncompressedEd25519PublicKey[PUBLIC_KEY_PREFIX_SIZE + PUBLIC_KEY_COMPONENT_SIZE];

	// Initialize one
	uint8_t one[PUBLIC_KEY_COMPONENT_SIZE] = {};
	one[sizeof(one) - 1] = 1;
	
	// Compute (1 + y) mod p
	cx_math_addm(x25519PublicKey, one, y, ED25519_CURVE_ORDER, PUBLIC_KEY_COMPONENT_SIZE);
	
	// Compute (1 - y) mod p
	cx_math_subm(y, one, y, ED25519_CURVE_ORDER, PUBLIC_KEY_COMPONENT_SIZE);
	
	// Compute the X25519 public key as (1 + y) / (1 - y) mod p
	cx_math_invprimem(y, y, ED25519_CURVE_ORDER, PUBLIC_KEY_COMPONENT_SIZE);
	cx_math_multm(x25519PublicKey, x25519PublicKey, y, ED25519_CURVE_ORDER, PUBLIC_KEY_COMPONENT_SIZE);
	
	// Swap the X25519 public key's endianness
	swapEndianness(x25519PublicKey, X25519_PUBLIC_KEY_SIZE);
}

// Get payment proof message length
size_t getPaymentProofMessageLength(uint64_t value, size_t senderAddressLength) {

	// Check sender address length
	switch(senderAddressLength) {
	
		// MQS address size
		case MQS_ADDRESS_SIZE:
		
			// Check currency doesn't allow MQS addresses
			if(!currencyInformation.mqsAddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Return payment proof message length
			return COMMITMENT_SIZE * HEXADECIMAL_CHARACTER_SIZE + senderAddressLength + getStringLength(value);
		
		// Tor address size
		case TOR_ADDRESS_SIZE:
		
			// Check currency doesn't allow TOR addresses
			if(!currencyInformation.torAddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Return payment proof message length
			return COMMITMENT_SIZE * HEXADECIMAL_CHARACTER_SIZE + senderAddressLength + getStringLength(value);
		
		// Ed25519 address size
		case ED25519_PUBLIC_KEY_SIZE:
		
			// TODO test Grin
		
			// Check currency doesn't allow Ed25519 addresses
			if(!currencyInformation.ed25519AddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Return payment proof message length
			return sizeof(value) + COMMITMENT_SIZE + senderAddressLength;
		
		// Default
		default:
		
			// Throw invalid parameters error
			THROW(INVALID_PARAMETERS_ERROR);
	}
}

// Get payment proof message
void getPaymentProofMessage(uint8_t *message, uint64_t value, const uint8_t *commitment, const uint8_t *senderAddress, size_t senderAddressLength) {

	// Check sender address length
	switch(senderAddressLength) {
	
		// MQS address size
		case MQS_ADDRESS_SIZE:
		
			// Check currency doesn't allow MQS addresses
			if(!currencyInformation.mqsAddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Check if sender address isn't a valid MQS address
			if(!getPublicKeyFromMqsAddress(NULL, senderAddress, senderAddressLength)) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Append commitment as a hex string to the message
			toHexString((char *)message, commitment, COMMITMENT_SIZE);
			
			// Append sender address to the message
			memcpy(&message[COMMITMENT_SIZE * HEXADECIMAL_CHARACTER_SIZE], senderAddress, senderAddressLength);
			
			// Append value as a string to the message
			toString((char *)&message[COMMITMENT_SIZE * HEXADECIMAL_CHARACTER_SIZE + senderAddressLength], value, 0);
			
			// Break
			break;
		
		// Tor address size
		case TOR_ADDRESS_SIZE:
		
			// Check currency doesn't allow TOR addresses
			if(!currencyInformation.torAddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Check if sender address isn't a valid Tor address
			if(!getPublicKeyFromTorAddress(NULL, senderAddress, senderAddressLength)) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Append commitment as a hex string to the message
			toHexString((char *)message, commitment, COMMITMENT_SIZE);
			
			// Append sender address to the message
			memcpy(&message[COMMITMENT_SIZE * HEXADECIMAL_CHARACTER_SIZE], senderAddress, senderAddressLength);
			
			// Append value as a string to the message
			toString((char *)&message[COMMITMENT_SIZE * HEXADECIMAL_CHARACTER_SIZE + senderAddressLength], value, 0);
			
			// Break
			break;
		
		// Ed25519 address size
		case ED25519_PUBLIC_KEY_SIZE:
		
			// TODO test Grin
			
			// Check currency doesn't allow Ed25519 addresses
			if(!currencyInformation.ed25519AddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Check if the sender address isn't a valid Ed25519 public key
			if(!isValidEd25519PublicKey(senderAddress, senderAddressLength)) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Convert value to big endian
			swapEndianness((uint8_t *)&value, sizeof(value));
			
			// Append value to the message
			memcpy(message, &value, sizeof(value));
			
			// Append commitment to the message
			memcpy(&message[sizeof(value)], commitment, COMMITMENT_SIZE);
			
			// Append sender address to the message
			memcpy(&message[sizeof(value) + COMMITMENT_SIZE], senderAddress, senderAddressLength);
			
			// Break
			break;
	}
}

// Verify payment proof message
bool verifyPaymentProofMessage(const uint8_t *message, size_t messageLength, const uint8_t *receiverAddress, size_t receiverAddressLength, uint8_t *signature, size_t signatureLength) {

	// Check receiver address length
	switch(receiverAddressLength) {
	
		// MQS address size
		case MQS_ADDRESS_SIZE:
		
			// Check currency doesn't allow MQS addresses
			if(!currencyInformation.mqsAddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Check if signature length is invalid
			if(signatureLength > MAXIMUM_DER_SIGNATURE_SIZE) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			{
				// Check if getting receiver public key from receiver's MQS address failed
				cx_ecfp_public_key_t receiverPublicKey;
				if(!getPublicKeyFromMqsAddress(&receiverPublicKey, receiverAddress, receiverAddressLength)) {
				
					// Throw invalid parameters error
					THROW(INVALID_PARAMETERS_ERROR);
				}
			
				// Get hash of the message
				uint8_t hash[CX_SHA256_SIZE];
				cx_hash_sha256(message, messageLength, hash, sizeof(hash));
				
				// Check if verifying the hash with the receiver public key and signature failed
				if(!cx_ecdsa_verify(&receiverPublicKey, CX_RND_RFC6979 | CX_LAST, CX_SHA256, hash, sizeof(hash), signature, signatureLength)) {
				
					// Return false
					return false;
				}
			}
			
			// Break
			break;
		
		// Tor address size
		case TOR_ADDRESS_SIZE:
		
			// Check currency doesn't allow Tor addresses
			if(!currencyInformation.torAddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Check if signature length is invalid
			if(signatureLength != ED25519_SIGNATURE_SIZE) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			{
				// Check if getting receiver public key from receiver's Tor address failed
				cx_ecfp_public_key_t receiverPublicKey;
				if(!getPublicKeyFromTorAddress(&receiverPublicKey, receiverAddress, receiverAddressLength)) {
				
					// Throw invalid parameters error
					THROW(INVALID_PARAMETERS_ERROR);
				}
				
				// Check if verifying the message with the receiver public key and signature failed
				if(!cx_eddsa_verify(&receiverPublicKey, CX_LAST, CX_SHA512, message, messageLength, NULL, 0, signature, signatureLength)) {
				
					// Return false
					return false;
				}
			}
			
			// Break
			break;
		
		// Ed25519 address size
		case ED25519_PUBLIC_KEY_SIZE:
		
			// TODO test Grin
		
			// Check currency doesn't allow Ed25519 addresses
			if(!currencyInformation.ed25519AddressPaymentProofAllowed) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Check if signature length is invalid
			if(signatureLength != ED25519_SIGNATURE_SIZE) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Check if the receiver address isn't a valid Ed25519 public key
			if(!isValidEd25519PublicKey(receiverAddress, receiverAddressLength)) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			{
				// Uncompress the receiver address to a public key
				uint8_t uncompressedPublicKey[UNCOMPRESSED_PUBLIC_KEY_SIZE];
				uncompressedPublicKey[0] = ED25519_COMPRESSED_PUBLIC_KEY_PREFIX;
				memcpy(&uncompressedPublicKey[PUBLIC_KEY_PREFIX_SIZE], receiverAddress, receiverAddressLength);
				
				cx_edwards_decompress_point(CX_CURVE_Ed25519, uncompressedPublicKey, sizeof(uncompressedPublicKey));
				
				// Initialize the receiver public key with the uncompressed public key
				cx_ecfp_public_key_t receiverPublicKey;
				cx_ecfp_init_public_key(CX_CURVE_Ed25519, uncompressedPublicKey, sizeof(uncompressedPublicKey), &receiverPublicKey);
			
				// Check if verifying the message with the receiver public key and signature failed
				if(!cx_eddsa_verify(&receiverPublicKey, CX_LAST, CX_SHA512, message, messageLength, NULL, 0, signature, signatureLength)) {
				
					// Return false
					return false;
				}
			}
			
			// Break
			break;
		
		// Default
		default:
		
			// Throw invalid parameters error
			THROW(INVALID_PARAMETERS_ERROR);
	}
	
	// Return true
	return true;
}

// Commitment is valid
bool commitmentIsValid(const uint8_t *commitment) {

	// Create context
	volatile uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE)];
	volatile secp256k1_context *context = secp256k1_context_preallocated_create((uint8_t *)contextBuffer, SECP256K1_CONTEXT_NONE);
	
	// Initialize result
	volatile bool result;
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Set result to if parsing commitment was successful
			secp256k1_pedersen_commitment commitmentValue;
			result = secp256k1_pedersen_commitment_parse((secp256k1_context *)context, &commitmentValue, commitment);
		}
		
		// Finally
		FINALLY {
		
			// Destroy context
			secp256k1_context_preallocated_destroy((secp256k1_context *)context);
			explicit_bzero((uint8_t *)contextBuffer, sizeof(contextBuffer));
		}
	}
	
	// End try
	END_TRY;
	
	// Return result
	return result;
}

// Is valid Ed25519 public key
bool isValidEd25519PublicKey(const uint8_t *publicKey, size_t length) {

	// Check if length is invalid
	if(length != ED25519_PUBLIC_KEY_SIZE) {
	
		// Return false
		return false;
	}

	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Uncompress the public key
			uint8_t uncompressedPublicKey[UNCOMPRESSED_PUBLIC_KEY_SIZE];
			uncompressedPublicKey[0] = ED25519_COMPRESSED_PUBLIC_KEY_PREFIX;
			memcpy(&uncompressedPublicKey[PUBLIC_KEY_PREFIX_SIZE], publicKey, length);
			
			cx_edwards_decompress_point(CX_CURVE_Ed25519, uncompressedPublicKey, sizeof(uncompressedPublicKey));
		}
		
		// Catch all errors
		CATCH_ALL {
			
			// Return false
			return false;
		}
		
		// Finally
		FINALLY {
		
		}
	}
	
	// End try
	END_TRY;
	
	// Return true
	return true;
}

// Is valid secp256k1 private key
bool isValidSecp256k1PrivateKey(const uint8_t *privateKey, size_t length) {

	// Check if length is invalid
	if(length != SECP256k1_PRIVATE_KEY_SIZE) {
	
		// Return false
		return false;
	}

	// Create context
	uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE)];
	secp256k1_context *context = secp256k1_context_preallocated_create(contextBuffer, SECP256K1_CONTEXT_NONE);
	
	// Set result to if private key is a valid secp256k1 private key
	const bool result = secp256k1_ec_seckey_verify(context, privateKey);
	
	// Destroy context
	secp256k1_context_preallocated_destroy(context);
	explicit_bzero(contextBuffer, sizeof(contextBuffer));
	
	// Return result
	return result;
}

// Is valid secp256k1 public key
bool isValidSecp256k1PublicKey(const uint8_t *publicKey, size_t length, bool *zeroArray) {

	// Check if length is invalid
	if(length != COMPRESSED_PUBLIC_KEY_SIZE && length != UNCOMPRESSED_PUBLIC_KEY_SIZE) {
	
		// Return false
		return false;
	}

	// Create context
	uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE)];
	secp256k1_context *context = secp256k1_context_preallocated_create(contextBuffer, SECP256K1_CONTEXT_NONE);
	
	// Set result to if public key is a valid secp256k1 public key
	secp256k1_pubkey publicKeyData;
	const bool result = secp256k1_ec_pubkey_parse(context, &publicKeyData, publicKey, length);
	
	// Check if the public key is valid and testing if the public key is a zero array
	if(result && zeroArray) {
	
		// Set if the public key is a zero array
		*zeroArray = isZeroArray(publicKeyData.data, sizeof(publicKeyData.data));
	}
	
	// Destroy context
	secp256k1_context_preallocated_destroy(context);
	explicit_bzero(contextBuffer, sizeof(contextBuffer));
	
	// Return result
	return result;
}

// Uncompress secp256k1 public key
void uncompressSecp256k1PublicKey(uint8_t *publicKey) {

	// Create context
	uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE)];
	secp256k1_context *context = secp256k1_context_preallocated_create(contextBuffer, SECP256K1_CONTEXT_NONE);
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
	
			// Check if parsing the public key failed
			secp256k1_pubkey publicKeyData;
			if(!secp256k1_ec_pubkey_parse(context, &publicKeyData, publicKey, COMPRESSED_PUBLIC_KEY_SIZE)) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Check if serializing the public key failed
			size_t publicKeySize = UNCOMPRESSED_PUBLIC_KEY_SIZE;
			if(!secp256k1_ec_pubkey_serialize(context, publicKey, &publicKeySize, &publicKeyData, SECP256K1_EC_UNCOMPRESSED)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
	
			// Destroy context
			secp256k1_context_preallocated_destroy(context);
			explicit_bzero(contextBuffer, sizeof(contextBuffer));
		}
	}
	
	// End try
	END_TRY;
}

// Get Ed25519 public key
void getEd25519PublicKey(uint8_t *ed25519PublicKey, uint32_t account) {

	// Initialize address private key
	volatile cx_ecfp_private_key_t addressPrivateKey;
	
	// Initialize address public key
	volatile cx_ecfp_public_key_t addressPublicKey;
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Get address private key at the Tor address private key index
			getAddressPrivateKey(&addressPrivateKey, account, TOR_ADDRESS_PRIVATE_KEY_INDEX, CX_CURVE_Ed25519);
			
			// Get address public key from address private key
			cx_ecfp_generate_pair(CX_CURVE_Ed25519, (cx_ecfp_public_key_t *)&addressPublicKey, (cx_ecfp_private_key_t *)&addressPrivateKey, KEEP_PRIVATE_KEY);
			
			// Compress the address public key
			cx_edwards_compress_point(CX_CURVE_Ed25519, (uint8_t *)addressPublicKey.W, addressPublicKey.W_len);
		}
		
		// Finally
		FINALLY {
		
			// Clear the address private key
			explicit_bzero((cx_ecfp_private_key_t *)&addressPrivateKey, sizeof(addressPrivateKey));
		}
	}
	
	// End try
	END_TRY;
	
	// Get Ed25519 public key from the address public key
	memcpy(ed25519PublicKey, (uint8_t *)&addressPublicKey.W[PUBLIC_KEY_PREFIX_SIZE], ED25519_PUBLIC_KEY_SIZE);
}

// Calculate bulletproof tau x
void calculateBulletproofTauX(volatile uint8_t *bulletproofTauX, const uint64_t *value, const uint8_t *blindingFactor, const uint8_t *rewindNonce, const uint8_t *privateNonce, const uint8_t *proofMessage) {

	// Create context
	volatile uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY)];
	volatile secp256k1_context *context = secp256k1_context_preallocated_create((uint8_t *)contextBuffer, SECP256K1_CONTEXT_VERIFY);
	
	// Create scratch space
	volatile uint8_t scratchSpaceBuffer[SCRATCH_SPACE_SIZE];
	volatile secp256k1_scratch_space *scratchSpace = secp256k1_scratch_space_preallocated_create((secp256k1_context *)context, (uint8_t *)scratchSpaceBuffer, SCRATCH_SPACE_SIZE);
	
	// Create generators
	volatile uint8_t generatorsBuffer[secp256k1_bulletproof_generators_preallocated_size(NUMBER_OF_GENERATORS_TAU_X)];
	volatile secp256k1_bulletproof_generators *generators = secp256k1_bulletproof_generators_preallocated_create((secp256k1_context *)context, (uint8_t *)generatorsBuffer, &GENERATOR_G, NUMBER_OF_GENERATORS_TAU_X);
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
	
			// Check if creating bulletproof tau x failed
			if(!secp256k1_bulletproof_rangeproof_preallocated_prove((secp256k1_context *)context, (secp256k1_scratch_space *)scratchSpace, (secp256k1_bulletproof_generators *)generators, NULL, NULL, (uint8_t *)bulletproofTauX, NULL, NULL, value, NULL, &blindingFactor, NULL, 1, &GENERATOR_H, BITS_PROVEN_PER_RANGE, rewindNonce, (uint8_t *)privateNonce, NULL, 0, proofMessage)) {

				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
		
			// Destroy generators
			secp256k1_bulletproof_generators_preallocated_destroy((secp256k1_context *)context, (secp256k1_bulletproof_generators *)generators);
			explicit_bzero((uint8_t *)generatorsBuffer, sizeof(generatorsBuffer));
			
			// Destroy scratch space
			secp256k1_scratch_space_preallocated_destroy((secp256k1_scratch_space *)scratchSpace);
			explicit_bzero((uint8_t *)scratchSpaceBuffer, sizeof(scratchSpaceBuffer));
			
			// Destroy context
			secp256k1_context_preallocated_destroy((secp256k1_context *)context);
			explicit_bzero((uint8_t *)contextBuffer, sizeof(contextBuffer));
		}
	}
	
	// End try
	END_TRY;
}

// Calculate bulletproof t one and t two
void calculateBulletproofTOneAndTTwo(volatile uint8_t *tOne, volatile uint8_t *tTwo, const uint64_t *value, const uint8_t *blindingFactor, const uint8_t *rewindNonce, const uint8_t *privateNonce) {

	// Create context
	volatile uint8_t contextBuffer[secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY)];
	volatile secp256k1_context *context = secp256k1_context_preallocated_create((uint8_t *)contextBuffer, SECP256K1_CONTEXT_VERIFY);
	
	// Create scratch space
	volatile uint8_t scratchSpaceBuffer[SCRATCH_SPACE_SIZE];
	volatile secp256k1_scratch_space *scratchSpace = secp256k1_scratch_space_preallocated_create((secp256k1_context *)context, (uint8_t *)scratchSpaceBuffer, SCRATCH_SPACE_SIZE);
	
	// Create generators
	volatile uint8_t generatorsBuffer[secp256k1_bulletproof_generators_preallocated_size(NUMBER_OF_GENERATORS_T_ONE_AND_T_TWO)];
	volatile secp256k1_bulletproof_generators *generators = secp256k1_bulletproof_generators_preallocated_create((secp256k1_context *)context, (uint8_t *)generatorsBuffer, &GENERATOR_G, NUMBER_OF_GENERATORS_T_ONE_AND_T_TWO);
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
	
			// Check if creating bulletproof t one and t two failed
			secp256k1_pubkey tOneData;
			secp256k1_pubkey tTwoData;
			if(!secp256k1_bulletproof_rangeproof_preallocated_prove((secp256k1_context *)context, (secp256k1_scratch_space *)scratchSpace, (secp256k1_bulletproof_generators *)generators, NULL, NULL, NULL, &tOneData, &tTwoData, value, NULL, &blindingFactor, NULL, 1, &GENERATOR_H, BITS_PROVEN_PER_RANGE, rewindNonce, (uint8_t *)privateNonce, NULL, 0, NULL)) {

				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
			
			// Check if serializing t one failed
			size_t publicKeySize = COMPRESSED_PUBLIC_KEY_SIZE;
			if(!secp256k1_ec_pubkey_serialize((secp256k1_context *)context, (uint8_t *)tOne, &publicKeySize, &tOneData, SECP256K1_EC_COMPRESSED)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
			
			// Check if serializing ttwo failed
			publicKeySize = COMPRESSED_PUBLIC_KEY_SIZE;
			if(!secp256k1_ec_pubkey_serialize((secp256k1_context *)context, (uint8_t *)tTwo, &publicKeySize, &tTwoData, SECP256K1_EC_COMPRESSED)) {
			
				// Throw internal error error
				THROW(INTERNAL_ERROR_ERROR);
			}
		}
		
		// Finally
		FINALLY {
		
			// Destroy generators
			secp256k1_bulletproof_generators_preallocated_destroy((secp256k1_context *)context, (secp256k1_bulletproof_generators *)generators);
			explicit_bzero((uint8_t *)generatorsBuffer, sizeof(generatorsBuffer));
			
			// Destroy scratch space
			secp256k1_scratch_space_preallocated_destroy((secp256k1_scratch_space *)scratchSpace);
			explicit_bzero((uint8_t *)scratchSpaceBuffer, sizeof(scratchSpaceBuffer));
			
			// Destroy context
			secp256k1_context_preallocated_destroy((secp256k1_context *)context);
			explicit_bzero((uint8_t *)contextBuffer, sizeof(contextBuffer));
		}
	}
	
	// End try
	END_TRY;
}
