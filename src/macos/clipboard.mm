#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include "../clipboard.h"

std::vector<std::string> ReadFilePaths() {
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSArray<NSURL *> *urls = [pasteboard readObjectsForClasses:@[NSURL.class] options:@{
            NSPasteboardURLReadingFileURLsOnlyKey: @YES,
    }];
    if (!urls) {
        return {};
    }
    auto result = std::vector<std::string>();
    result.reserve(urls.count);
    for (NSURL *url in urls) {
        result.emplace_back([url.path UTF8String]);
    }
    return result;
}