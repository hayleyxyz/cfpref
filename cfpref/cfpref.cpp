//
//  cfpref.cpp
//  cfpref - A pointless program to read keys under the "CPU Names" dictionary in the com.apple.SystemProfiler
//      preference set using CoreFoundation preference API. The preference plist can be found/edited directly
//      at ~/Library/Preferences/com.apple.SystemProfiler.plist
//
//  Created by yui on 19/05/2019.
//  Copyright © 2019 yui. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <iostream>

/*
 * WARNING
 * process cfprefsd can cause plist values to be cached, kill the instance(s) of this process to
 * refresh any manual edits to ~/Library/Preferences/com.apple.SystemProfiler.plist
 * $ pkill cfprefsd
 */

inline char *CFStringToCString(CFStringRef string) {
    assert(CFGetTypeID(string) == CFStringGetTypeID());
    auto length = CFStringGetLength(string);
    auto result = new char[length + 1] {0};
    CFStringGetCString(string, result, length + 1, kCFStringEncodingUTF8);
    return result;
}

inline long long CFNumberToLongLong(CFNumberRef number) {
    assert(CFGetTypeID(number) == CFNumberGetTypeID());
    auto result = 0ll;
    CFNumberGetValue(number, kCFNumberNSIntegerType, &result);
    return result;
}

int main(int argc, const char * argv[]) {
    
    CFPreferencesSynchronize(CFSTR("com.apple.SystemProfiler"), kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
    
    CFPropertyListRef ref = CFPreferencesCopyValue(CFSTR("CPU Names"), CFSTR("com.apple.SystemProfiler"),
                                                   kCFPreferencesCurrentUser, kCFPreferencesAnyHost);
    
    assert(CFGetTypeID(ref) == CFDictionaryGetTypeID());
    
    auto count = CFDictionaryGetCount((CFDictionaryRef)ref);
    
    auto keys = new const void *[count];
    auto values = new const void *[count];
    
    CFDictionaryGetKeysAndValues(reinterpret_cast<CFDictionaryRef>(ref), keys, values);
    
    for (auto i = 0; i < count; i++) {
        // Pref. dictionary keys should always be strings
        assert(CFGetTypeID(keys[i]) == CFStringGetTypeID());
        
        auto key = CFStringToCString(reinterpret_cast<CFStringRef>(keys[i]));
        std::cout << key << " => ";
        delete[] key;
        
        auto value = values[i];
        auto typeId = CFGetTypeID(value);
        
        if(typeId == CFNumberGetTypeID()) {
            std::cout << CFNumberToLongLong(reinterpret_cast<CFNumberRef>(value));
        }
        else if(typeId == CFStringGetTypeID()) {
            auto str = CFStringToCString(reinterpret_cast<CFStringRef>(value));
            std::cout << str;
            delete[] str;
        }
        else {
            std::cout << "ERROR UNKNOWN TYPE ID " << typeId;
        }
        
        std::cout << std::endl;
    }
    
    return 0;
}
