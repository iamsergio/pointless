// SPDX-FileCopyrightText: 2025 Sergio Martins
// SPDX-License-Identifier: MIT

#include "ical_parser.h"
#include "logger.h"

#include <libical/ical.h>

#include <format>

namespace pointless::core {

namespace {

std::chrono::system_clock::time_point icalTimeToTimePoint(icaltimetype t)
{
    const icaltimezone *tz = icaltime_get_timezone(t);
    if (!tz)
        tz = icaltimezone_get_utc_timezone();

    time_t epoch = icaltime_as_timet_with_zone(t, tz);
    return std::chrono::system_clock::from_time_t(epoch);
}

icaltimetype timePointToIcalTime(std::chrono::system_clock::time_point tp)
{
    time_t t = std::chrono::system_clock::to_time_t(tp);
    return icaltime_from_timet_with_zone(t, 0, icaltimezone_get_utc_timezone());
}

std::string formatRecurrenceId(icaltimetype t)
{
    return std::format("{:04}{:02}{:02}T{:02}{:02}{:02}Z",
                       t.year, t.month, t.day,
                       t.hour, t.minute, t.second);
}

} // namespace

std::vector<ICalEvent> parseICalEvents(const std::string &icalData,
                                       const std::optional<DateRange> &range)
{
    std::vector<ICalEvent> events;

    icalcomponent *root = icalparser_parse_string(icalData.c_str());
    if (!root) {
        P_LOG_WARNING("Failed to parse iCalendar data");
        return events;
    }

    auto processVEvent = [&](icalcomponent *vevent) {
        std::string uid;
        std::string summary;
        bool isAllDay = false;
        icaltimetype dtstartIcal = icaltime_null_time();
        std::chrono::system_clock::time_point dtstartTp;
        std::chrono::system_clock::time_point dtendTp;

        icalproperty *uidProp = icalcomponent_get_first_property(vevent, ICAL_UID_PROPERTY);
        if (uidProp)
            uid = icalproperty_get_uid(uidProp);

        icalproperty *summaryProp = icalcomponent_get_first_property(vevent, ICAL_SUMMARY_PROPERTY);
        if (summaryProp)
            summary = icalproperty_get_summary(summaryProp);

        icalproperty *dtstartProp = icalcomponent_get_first_property(vevent, ICAL_DTSTART_PROPERTY);
        if (dtstartProp) {
            dtstartIcal = icalproperty_get_dtstart(dtstartProp);
            isAllDay = icaltime_is_date(dtstartIcal);
            dtstartTp = icalTimeToTimePoint(dtstartIcal);
        }

        icalproperty *dtendProp = icalcomponent_get_first_property(vevent, ICAL_DTEND_PROPERTY);
        if (dtendProp) {
            dtendTp = icalTimeToTimePoint(icalproperty_get_dtend(dtendProp));
        }

        auto duration = dtendTp - dtstartTp;

        icalproperty *rruleProp = icalcomponent_get_first_property(vevent, ICAL_RRULE_PROPERTY);
        if (rruleProp && range.has_value()) {
            struct icalrecurrencetype recur = icalproperty_get_rrule(rruleProp);
            icalrecur_iterator *iter = icalrecur_iterator_new(recur, dtstartIcal);
            if (!iter)
                return;

            icaltimetype rangeStart = timePointToIcalTime(range->start);
            icaltimetype rangeEnd = timePointToIcalTime(range->end);

            for (icaltimetype next = icalrecur_iterator_next(iter);
                 !icaltime_is_null_time(next);
                 next = icalrecur_iterator_next(iter)) {

                if (icaltime_compare(next, rangeEnd) >= 0)
                    break;

                if (icaltime_compare(next, rangeStart) < 0)
                    continue;

                ICalEvent ev;
                ev.uid = uid + "_" + formatRecurrenceId(next);
                ev.summary = summary;
                ev.isAllDay = isAllDay;
                ev.dtstart = icalTimeToTimePoint(next);
                ev.dtend = ev.dtstart + duration;
                events.push_back(std::move(ev));
            }

            icalrecur_iterator_free(iter);
        } else {
            if (range.has_value()) {
                auto effectiveEnd = (dtendTp == std::chrono::system_clock::time_point {})
                    ? dtstartTp
                    : dtendTp;
                if (effectiveEnd <= range->start || dtstartTp >= range->end)
                    return;
            }

            ICalEvent ev;
            ev.uid = uid;
            ev.summary = summary;
            ev.isAllDay = isAllDay;
            ev.dtstart = dtstartTp;
            ev.dtend = dtendTp;
            events.push_back(std::move(ev));
        }
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
