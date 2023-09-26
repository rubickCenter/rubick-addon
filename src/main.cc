#include <napi.h>

#include "keypress.h"
#include "clipboard.h"

int keyboardDelay = 10;

/*
 _  __          _                         _
| |/ /___ _   _| |__   ___   __ _ _ __ __| |
| ' // _ \ | | | '_ \ / _ \ / _` | '__/ _` |
| . \  __/ |_| | |_) | (_) | (_| | | | (_| |
|_|\_\___|\__, |_.__/ \___/ \__,_|_|  \__,_|
          |___/
*/
struct KeyNames {
    const char *name;
    MMKeyCode key;
};

static KeyNames key_names[] =
        {
                {"backspace",         K_BACKSPACE},
                {"delete",            K_DELETE},
                {"enter",             K_RETURN},
                {"tab",               K_TAB},
                {"escape",            K_ESCAPE},
                {"up",                K_UP},
                {"down",              K_DOWN},
                {"right",             K_RIGHT},
                {"left",              K_LEFT},
                {"home",              K_HOME},
                {"end",               K_END},
                {"pageup",            K_PAGEUP},
                {"pagedown",          K_PAGEDOWN},
                {"f1",                K_F1},
                {"f2",                K_F2},
                {"f3",                K_F3},
                {"f4",                K_F4},
                {"f5",                K_F5},
                {"f6",                K_F6},
                {"f7",                K_F7},
                {"f8",                K_F8},
                {"f9",                K_F9},
                {"f10",               K_F10},
                {"f11",               K_F11},
                {"f12",               K_F12},
                {"f13",               K_F13},
                {"f14",               K_F14},
                {"f15",               K_F15},
                {"f16",               K_F16},
                {"f17",               K_F17},
                {"f18",               K_F18},
                {"f19",               K_F19},
                {"f20",               K_F20},
                {"f21",               K_F21},
                {"f22",               K_F22},
                {"f23",               K_F23},
                {"f24",               K_F24},
                {"command",           K_META},
                {"cmd",               K_CMD},
                {"right_cmd",         K_RIGHTCMD},
                {"win",               K_WIN},
                {"right_win",         K_RIGHTWIN},
                {"alt",               K_ALT},
                {"right_alt",         K_RIGHTALT},
                {"control",           K_CONTROL},
                {"right_control",     K_RIGHTCONTROL},
                {"shift",             K_SHIFT},
                {"right_shift",       K_RIGHTSHIFT},
                {"space",             K_SPACE},
                {"printscreen",       K_PRINTSCREEN},
                {"insert",            K_INSERT},
                {"menu",              K_MENU},
                {"fn",                K_FUNCTION},

                {"caps_lock",         K_CAPSLOCK},
                {"num_lock",          K_NUMLOCK},
                {"scroll_lock",       K_SCROLL_LOCK},

                {"audio_mute",        K_AUDIO_VOLUME_MUTE},
                {"audio_vol_down",    K_AUDIO_VOLUME_DOWN},
                {"audio_vol_up",      K_AUDIO_VOLUME_UP},
                {"audio_play",        K_AUDIO_PLAY},
                {"audio_stop",        K_AUDIO_STOP},
                {"audio_pause",       K_AUDIO_PAUSE},
                {"audio_prev",        K_AUDIO_PREV},
                {"audio_next",        K_AUDIO_NEXT},
                {"audio_rewind",      K_AUDIO_REWIND},
                {"audio_forward",     K_AUDIO_FORWARD},
                {"audio_repeat",      K_AUDIO_REPEAT},
                {"audio_random",      K_AUDIO_RANDOM},

                {"numpad_0",          K_NUMPAD_0},
                {"numpad_1",          K_NUMPAD_1},
                {"numpad_2",          K_NUMPAD_2},
                {"numpad_3",          K_NUMPAD_3},
                {"numpad_4",          K_NUMPAD_4},
                {"numpad_5",          K_NUMPAD_5},
                {"numpad_6",          K_NUMPAD_6},
                {"numpad_7",          K_NUMPAD_7},
                {"numpad_8",          K_NUMPAD_8},
                {"numpad_9",          K_NUMPAD_9},
                {"numpad_decimal",    K_NUMPAD_DECIMAL},

                {"add",               K_ADD},
                {"subtract",          K_SUBTRACT},
                {"multiply",          K_MULTIPLY},
                {"divide",            K_DIVIDE},
                {"clear",             K_CLEAR},

                {"add",               K_ADD},
                {"subtract",          K_SUBTRACT},
                {"multiply",          K_MULTIPLY},
                {"divide",            K_DIVIDE},
                {"clear",             K_CLEAR},

                {"lights_mon_up",     K_LIGHTS_MON_UP},
                {"lights_mon_down",   K_LIGHTS_MON_DOWN},
                {"lights_kbd_toggle", K_LIGHTS_KBD_TOGGLE},
                {"lights_kbd_up",     K_LIGHTS_KBD_UP},
                {"lights_kbd_down",   K_LIGHTS_KBD_DOWN},

                {NULL,                K_NOT_A_KEY} /* end marker */
        };

int CheckKeyCodes(std::string &keyName, MMKeyCode *key) {
    if (!key)
        return -1;

    if (keyName.length() == 1) {
        *key = keyCodeForChar(*keyName.c_str());
        return 0;
    }

    *key = K_NOT_A_KEY;

    KeyNames *kn = key_names;
    while (kn->name) {
        if (keyName.compare(kn->name) == 0) {
            *key = kn->key;
            break;
        }
        kn++;
    }

    if (*key == K_NOT_A_KEY) {
        return -2;
    }

    return 0;
}

int CheckKeyFlags(std::string &flagString, MMKeyFlags *flags) {
    if (!flags)
        return -1;

    if (flagString == "alt" || flagString == "right_alt") {
        *flags = MOD_ALT;
#if defined(IS_MACOSX)
    } else if (flagString == "command" || flagString == "cmd" || flagString == "right_cmd") {
        *flags = MOD_META;
#else
        } else if (flagString == "command" || flagString == "win" || flagString == "right_win") {
    *flags = MOD_META;
#endif
    } else if (flagString == "control" || flagString == "right_control") {
        *flags = MOD_CONTROL;
    } else if (flagString == "shift" || flagString == "right_shift") {
        *flags = MOD_SHIFT;
    } else if (flagString == "none") {
        *flags = MOD_NONE;
    } else {
        return -2;
    }

    return 0;
}

int GetFlagsFromString(Napi::Value value, MMKeyFlags *flags) {
    std::string fstr = value.As<Napi::String>();
    return CheckKeyFlags(fstr, flags);
}

int GetFlagsFromValue(Napi::Value value, MMKeyFlags *flags) {
    if (!flags)
        return -1;

    //Optionally allow an array of flag strings to be passed.
    if (value.IsArray()) {
        Napi::Array a = value.As<Napi::Array>();
        for (uint32_t i = 0; i < a.Length(); ++i) {
            Napi::Value v = a.Get(i);
            if (!v.IsString())
                return -2;

            MMKeyFlags f = MOD_NONE;
            const int rv = GetFlagsFromString(v, &f);
            if (rv)
                return rv;

            *flags = (MMKeyFlags) (*flags | f);
        }
        return 0;
    }

    //If it's not an array, it should be a single string value.
    return GetFlagsFromString(value, flags);
}

Napi::Number _keyTap(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    MMKeyFlags flags = MOD_NONE;
    MMKeyCode key;

    std::string keyName = info[0].As<Napi::String>();

    switch (info.Length()) {
        case 2:
            switch (GetFlagsFromValue(info[1], &flags)) {
                case -1:
                    throw Napi::Error::New(env, "Null pointer in key flag.");
                    break;
                case -2:
                    throw Napi::Error::New(env, "Invalid key flag specified.");
                    break;
            }
            break;
        case 1:
            break;
        default:
            throw Napi::Error::New(env, "Invalid number of arguments.");
    }

    switch (CheckKeyCodes(keyName, &key)) {
        case -1:
            throw Napi::Error::New(env, "Null pointer in key code.");
            break;
        case -2:
            throw Napi::Error::New(env, "Invalid key code specified.");
            break;
        default:
            tapKeyCode(key, flags);
    }

    return Napi::Number::New(env, 1);
}

Napi::Number _keyToggle(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    MMKeyFlags flags = MOD_NONE;
    MMKeyCode key;

    bool down = false;

    //Get arguments from JavaScript.
    std::string keyName = info[0].As<Napi::String>();

    //Check and confirm number of arguments.
    switch (info.Length()) {
        case 3:
            //Get key modifier.
            switch (GetFlagsFromValue(info[2], &flags)) {
                case -1:
                    throw Napi::Error::New(env, "Null pointer in key flag.");
                    break;
                case -2:
                    throw Napi::Error::New(env, "Invalid key flag specified.");
                    break;
            }
            break;
        case 2:
            break;
        default:
            throw Napi::Error::New(env, "Invalid number of arguments.");
    }

    //Get down value if provided.
    if (info.Length() > 1) {
        std::string directionString = info[1].As<Napi::String>();

        if (directionString.compare("down") == 0) {
            down = true;
        } else if (directionString.compare("up") == 0) {
            down = false;
        } else {
            throw Napi::Error::New(env, "Invalid key state specified.");
        }
    }

    //Get the acutal key.
    switch (CheckKeyCodes(keyName, &key)) {
        case -1:
            throw Napi::Error::New(env, "Null pointer in key code.");
            break;
        case -2:
            throw Napi::Error::New(env, "Invalid key code specified.");
            break;
        default:
            toggleKeyCode(key, down, flags);
    }

    return Napi::Number::New(env, 1);
}

Napi::Number _typeString(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    std::string stringToType = info[0].As<Napi::String>();

    typeString(stringToType.c_str());

    return Napi::Number::New(env, 1);
}

Napi::Number _typeStringDelayed(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    std::string stringToType = info[0].As<Napi::String>();
    size_t cpm = info[1].As<Napi::Number>().Int32Value();

    typeStringDelayed(stringToType.c_str(), (unsigned int) cpm);

    return Napi::Number::New(env, 1);
}

Napi::Number _setKeyboardDelay(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
        throw Napi::Error::New(env, "Invalid number of arguments.");
    }

    keyboardDelay = info[0].As<Napi::Number>().Int32Value();

    return Napi::Number::New(env, 1);
}

/*
Clipboard
*/

Napi::Array _readFilePathsJs(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    const auto file_paths = ReadFilePaths();
    auto result = Napi::Array::New(env, file_paths.size());
    for (size_t i = 0; i != file_paths.size(); ++i)
    {
        result.Set(i, file_paths[i]);
    }
    return result;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "keyTap"), Napi::Function::New(env, _keyTap));
    exports.Set(Napi::String::New(env, "keyToggle"), Napi::Function::New(env, _keyToggle));
    exports.Set(Napi::String::New(env, "typeString"), Napi::Function::New(env, _typeString));
    exports.Set(Napi::String::New(env, "typeStringDelayed"), Napi::Function::New(env, _typeStringDelayed));
    exports.Set(Napi::String::New(env, "setKeyboardDelay"), Napi::Function::New(env, _setKeyboardDelay));

    exports.Set(Napi::String::New(env, "readFilePaths"), Napi::Function::New(env, _readFilePathsJs));

    return exports;
}

NODE_API_MODULE(rubick-addon, Init)
