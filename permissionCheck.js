let rubickAddon = require("bindings")("rubick-addon");

let hasScreenRecordingPermission = false;
let hasAccessibilityPermission = false;

try {
    const permissions = require("@nut-tree/node-mac-permissions");

    const wrapWithWarning = (message, nativeFunction) => (...args) => {
        console.warn(message);
        return nativeFunction(...args);
    };

    const askForAccessibility = (nativeFunction, functionName) => {
        if (process.platform !== 'darwin' || hasAccessibilityPermission) {
            return nativeFunction;
        }
        const accessibilityStatus = permissions.getAuthStatus("accessibility");

        if (accessibilityStatus === 'authorized') {
            hasAccessibilityPermission = true;
            return nativeFunction;
        } else if (accessibilityStatus === 'not determined' || accessibilityStatus === 'denied') {
            permissions.askForAccessibilityAccess();
            return wrapWithWarning(`##### WARNING! The application running this script tries to access accessibility features to execute ${functionName}! Please grant requested access and visit https://github.com/nut-tree/nut.js#macos for further information. #####`, nativeFunction);
        }
    }
    const askForScreenRecording = (nativeFunction, functionName) => {
        if (process.platform !== 'darwin' || hasScreenRecordingPermission) {
            return nativeFunction;
        }
        const screenCaptureStatus = permissions.getAuthStatus("screen");

        if (screenCaptureStatus === 'authorized') {
            hasScreenRecordingPermission = true;
            return nativeFunction;
        } else if (screenCaptureStatus === 'not determined' || screenCaptureStatus === 'denied') {
            permissions.askForScreenCaptureAccess();
            return wrapWithWarning(`##### WARNING! The application running this script tries to screen recording features to execute ${functionName}! Please grant the requested access and visit https://github.com/nut-tree/nut.js#macos for further information. #####`, nativeFunction);
        }
    }

    const accessibilityAccess = [
        "keyTap",
        "keyToggle",
        "typeString",
    ];
    for (const functionName of accessibilityAccess) {
        const originalFunction = rubickAddon[functionName];
        rubickAddon[functionName] = (...args) => askForAccessibility(originalFunction, functionName)(...args);
    }
} catch (e) {
    console.warn(`Encountered error establishing macOS permission checks:`, e.message);
    console.warn(`Returning original module.`);
    rubickAddon = require("bindings")("rubick-addon");
} finally {
    module.exports = rubickAddon;
}
