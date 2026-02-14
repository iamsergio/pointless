// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "ical_parser.h"
#include "logger.h"

#include <libical/ical.h>

namespace pointless::core {

std::vector<ICalEvent> parseICalEvents(const std::string &icalData)
{
    std::vector<ICalEvent> events;

    icalcomponent *root = icalparser_parse_string(icalData.c_str());
    if (!root) {
        P_LOG_WARNING("Failed to parse iCalendar data");
        return events;
    }

    auto processVEvent = [&](icalcomponent *vevent) {
        ICalEvent ev;

        icalproperty *uidProp = icalcomponent_get_first_property(vevent, ICAL_UID_PROPERTY);
        if (uidProp)
            ev.uid = icalproperty_get_uid(uidProp);

        icalproperty *summaryProp = icalcomponent_get_first_property(vevent, ICAL_SUMMARY_PROPERTY);
        if (summaryProp)
            ev.summary = icalproperty_get_summary(summaryProp);

        icalproperty *dtstartProp = icalcomponent_get_first_property(vevent, ICAL_DTSTART_PROPERTY);
        if (dtstartProp) {
            icaltimetype dtstart = icalproperty_get_dtstart(dtstartProp);
            ev.isAllDay = icaltime_is_date(dtstart);

            const icaltimezone *tz = icaltime_get_timezone(dtstart);
            if (!tz)
                tz = icaltimezone_get_utc_timezone();

            time_t t = icaltime_as_timet_with_zone(dtstart, tz);
            ev.dtstart = std::chrono::system_clock::from_time_t(t);
        }

        icalproperty *dtendProp = icalcomponent_get_first_property(vevent, ICAL_DTEND_PROPERTY);
        if (dtendProp) {
            icaltimetype dtend = icalproperty_get_dtend(dtendProp);

            const icaltimezone *tz = icaltime_get_timezone(dtend);
            if (!tz)
                tz = icaltimezone_get_utc_timezone();

            time_t t = icaltime_as_timet_with_zone(dtend, tz);
            ev.dtend = std::chrono::system_clock::from_time_t(t);
        }

        events.push_back(std::move(ev));
    };

    if (icalcomponent_isa(root) == ICAL_VCALENDAR_COMPONENT) {
        for (icalcomponent *c = icalcomponent_get_first_component(root, ICAL_VEVENT_COMPONENT);
             c != nullptr;
             c = icalcomponent_get_next_component(root, ICAL_VEVENT_COMPONENT)) {
            processVEvent(c);
        }
    } else if (icalcomponent_isa(root) == ICAL_VEVENT_COMPONENT) {
        processVEvent(root);
    }

    icalcomponent_free(root);
    return events;
}

} // namespace pointless::core
