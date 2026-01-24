// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "apple_calendar_provider.h"

#import <EventKit/EventKit.h>
#import <Foundation/Foundation.h>

#include <iostream>

namespace pointless::core {

struct AppleCalendarProvider::Private {
    EKEventStore* eventStore = nil;
};

AppleCalendarProvider::AppleCalendarProvider()
    : _d(std::make_unique<Private>())
{
    _d->eventStore = [[EKEventStore alloc] init];
}

AppleCalendarProvider::~AppleCalendarProvider() = default;

std::vector<Calendar> AppleCalendarProvider::getCalendars() const
{
    std::vector<Calendar> calendars;

    EKAuthorizationStatus status = [EKEventStore authorizationStatusForEntityType:EKEntityTypeEvent];
    if (status == EKAuthorizationStatusNotDetermined) {
        std::cerr << "Requesting access to calendars...\n";
        dispatch_semaphore_t sema = dispatch_semaphore_create(0);

        if (@available(macOS 14.0, iOS 17.0, *)) {
            [_d->eventStore requestFullAccessToEventsWithCompletion:^(BOOL granted, NSError* _Nullable error) {
                if (granted) {
                    std::cerr << "Access granted.\n";
                } else {
                    std::cerr << "Access denied.\n";
                    if (error != nullptr) {
                        const char* errStr = [[error localizedDescription] UTF8String];
                        std::cerr << "Error: " << ((errStr != nullptr) ? errStr : "Unknown") << "\n";
                    }
                }
                dispatch_semaphore_signal(sema);
            }];
        } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            [_d->eventStore requestAccessToEntityType:EKEntityTypeEvent
                                           completion:^(BOOL granted, NSError* _Nullable /*error*/) {
                                               if (granted) {
                                                   std::cerr << "Access granted.\n";
                                               } else {
                                                   std::cerr << "Access denied.\n";
                                               }
                                               dispatch_semaphore_signal(sema);
                                           }];
#pragma clang diagnostic pop
        }
        dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
        status = [EKEventStore authorizationStatusForEntityType:EKEntityTypeEvent];
    }

    bool hasAccess = false;
    if (@available(macOS 14.0, iOS 17.0, *)) {
        hasAccess = (status == EKAuthorizationStatusFullAccess);
    } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        hasAccess = (status == EKAuthorizationStatusAuthorized);
#pragma clang diagnostic pop
    }

    if (!hasAccess) {
        std::cerr << "Access to calendars is denied, restricted, or write-only.\n";
        return {};
    }

    NSArray<EKCalendar*>* ekCalendars = [_d->eventStore calendarsForEntityType:EKEntityTypeEvent];

    for (EKCalendar* ekCalendar in ekCalendars) {
        Calendar cal;
        cal.id = [ekCalendar.calendarIdentifier UTF8String];
        cal.title = [ekCalendar.title UTF8String];
        cal.writeable = ekCalendar.allowsContentModifications;

        if (ekCalendar.CGColor) {
            CGColorRef color = ekCalendar.CGColor;

            // Convert to sRGB if needed, but for now take components directly if count matches
            size_t count = CGColorGetNumberOfComponents(color);
            const CGFloat* components = CGColorGetComponents(color);

            if (count >= 3) {
                int r = static_cast<int>(components[0] * 255.0 + 0.5);
                int g = static_cast<int>(components[1] * 255.0 + 0.5);
                int b = static_cast<int>(components[2] * 255.0 + 0.5);
                char buffer[8];
                snprintf(buffer, sizeof(buffer), "#%02X%02X%02X", r, g, b);
                cal.color = buffer;
            }
        }

        calendars.push_back(cal);
    }

    return calendars;
}

} // namespace pointless::core
