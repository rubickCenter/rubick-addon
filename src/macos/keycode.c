#include "../keycode.h"

#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h> /* For kVK_ constants, and TIS functions. */

/* Returns string representation of key, if it is printable.
 * Ownership follows the Create Rule; that is, it is the caller's
 * responsibility to release the returned object. */
CFStringRef createStringForKey(CGKeyCode keyCode);

MMKeyCode keyCodeForChar(const char c)
{
	/* OS X does not appear to have a built-in function for this, so instead we
	 * have to write our own. */
	static CFMutableDictionaryRef charToCodeDict = NULL;
	CGKeyCode code;
	UniChar character = c;
	CFStringRef charStr = NULL;

	/* Generate table of keycodes and characters. */
	if (charToCodeDict == NULL)
	{
		size_t i;
		charToCodeDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
												   128,
												   &kCFCopyStringDictionaryKeyCallBacks,
												   NULL);
		if (charToCodeDict == NULL)
			return UINT16_MAX;

		/* Loop through every keycode (0 - 127) to find its current mapping. */
		for (i = 0; i < 128; ++i)
		{
			CFStringRef string = createStringForKey((CGKeyCode)i);
			if (string != NULL)
			{
				CFDictionaryAddValue(charToCodeDict, string, (const void *)i);
				CFRelease(string);
			}
		}
	}

	charStr = CFStringCreateWithCharacters(kCFAllocatorDefault, &character, 1);

	/* Our values may be NULL (0), so we need to use this function. */
	if (!CFDictionaryGetValueIfPresent(charToCodeDict, charStr,
									   (const void **)&code))
	{
		code = UINT16_MAX; /* Error */
	}

	CFRelease(charStr);
	return (MMKeyCode)code;
}

CFStringRef createStringForKey(CGKeyCode keyCode)
{
	TISInputSourceRef currentKeyboard = TISCopyCurrentASCIICapableKeyboardInputSource();
	CFDataRef layoutData =
		TISGetInputSourceProperty(currentKeyboard,
								  kTISPropertyUnicodeKeyLayoutData);
	const UCKeyboardLayout *keyboardLayout =
		(const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);

	UInt32 keysDown = 0;
	UniChar chars[4];
	UniCharCount realLength;

	UCKeyTranslate(keyboardLayout,
				   keyCode,
				   kUCKeyActionDisplay,
				   0,
				   LMGetKbdType(),
				   kUCKeyTranslateNoDeadKeysBit,
				   &keysDown,
				   sizeof(chars) / sizeof(chars[0]),
				   &realLength,
				   chars);
	CFRelease(currentKeyboard);

	return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
}
