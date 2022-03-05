// Header files
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
#include "bech32.h"


// Constants

// Input
static const uint8_t INPUT[] = {0x00, 0x14, 0x75, 0x1E, 0x76, 0xE8, 0x19, 0x91, 0x96, 0xD4, 0x54, 0x94, 0x1C, 0x45, 0xD1, 0xB3, 0xA3, 0x23, 0xF1, 0x43, 0x3B, 0xD6};

// Human-readable part
static const char *HUMAN_READABLE_PART = "hrp";

// Output
static const char OUTPUT[] = "hrp1qq2828nkaqver9k52j2pc3w3kw3j8u2r80tqyrjeaq";


// Function prototypes

// Test encode
static void testEncode(void **state);

// Test decode
static void testDecode(void **state);


// Main function
int main(void) {

	// Initialize tests
	const struct CMUnitTest tests[] = {
	
		// Test encode
		cmocka_unit_test(testEncode),
		
		// Test decode
		cmocka_unit_test(testDecode)
	};
	
	// Return performing tests
	return cmocka_run_group_tests(tests, NULL, NULL);
}


// Supporting function implementation

// Test Encode
void testEncode(void **state) {

	// Get output length
	const size_t outputLength = getBech32EncodedLength(sizeof(INPUT), HUMAN_READABLE_PART);
	
	// Assert output length is correct
	assert_int_equal(outputLength, sizeof(OUTPUT) - sizeof((char)'\0'));
	
	// Get output by encoding input
	char output[outputLength + sizeof((char)'\0')];
	bech32Encode(output, INPUT, sizeof(INPUT), HUMAN_READABLE_PART);
	output[outputLength] = '\0';
	
	// Assert output is correct
	assert_string_equal(output, OUTPUT);
}

// Test Decode
void testDecode(void **state) {

	// Get input length
	const size_t inputLength = getBech32DecodedLength(OUTPUT, sizeof(OUTPUT) - sizeof((char)'\0'));
	
	// Assert input length is correct
	assert_int_equal(inputLength, sizeof(INPUT));
	
	// Get input by decoding output
	uint8_t input[inputLength];
	bech32Decode(input, OUTPUT, sizeof(OUTPUT) - sizeof((char)'\0'));
	
	// Assert input is correct
	assert_memory_equal(input, INPUT, sizeof(INPUT));
}
