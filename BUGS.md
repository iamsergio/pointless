# Bugs

## Calendar import

Found while looking at real Exchange/Outlook feeds (KDAB, Claas, QuidelOrtho meetings).

- [ ] **Cancelled occurrences are imported as tasks.** `parseICalEvents` ignores `STATUS:CANCELLED`, so
  "Canceled: KDAB Weekly" / "Abgesagt: KDAB Daily" show up as tasks.
- [ ] **Moved occurrences are imported twice.** A `RECURRENCE-ID` override is emitted as a standalone event
  (with the plain master `UID` as id), and the master's `RRULE` expansion still emits the original slot.
  `RecurringEventExceptionInsideRange` only checks that the override exists, not that the original is gone.
  Several overrides of the same series also share one event id.
- [ ] **Deleted occurrences come back.** `EXDATE` is ignored; `icalrecur_iterator` only applies the `RRULE`.
- [ ] **HTTP errors are parsed as calendars.** `fetchICalUrl` only treats 401 as a failure, so a 403 HTML
  page goes to the iCal parser.
