# UniQOS - TODO List for Future Development

This document outlines pending features and improvements for the next team of students.

---

## Critical Tasks

### 1. Storage Task (FreeRTOS)
**Status:** No storage task exists, user data not persisted

Create a dedicated FreeRTOS task to manage all SD card storage and retrieval:
- [ ] Create new `storage_task.c` with queue-based command interface
- [ ] Implement SD card initialization and FatFs mounting on startup
- [ ] Handle storage requests from other tasks via message queue
- [ ] Store and load user settings (use minIni library)
- [ ] Migrate contacts B+ tree from FILE* to FatFs
- [ ] Save/load call logs with timestamps and metadata
- [ ] Save/load SMS messages with conversation threading
- [ ] Save/load calendar events and reminders
- [ ] Handle SD card errors and report status to other tasks
- [ ] Implement proper task priority (likely Low, like Power Task)

### 2. SD Card Integration with B+ Tree
**Status:** Uses standard C library (fopen/fread/fwrite), not FatFs

The contacts B+ tree needs to use FatFs API for SD card access:
- [ ] Replace `fopen/fread/fwrite/fclose` with `f_open/f_read/f_write/f_close`
- [ ] Change `FILE*` to `FIL` objects in B+ tree structure
- [ ] Add SD card mounting/unmounting lifecycle management
- [ ] Implement error handling for card failures (missing, corrupted, etc.)
- [ ] Add UI indicator for SD card status
- [ ] Test thoroughly with physical hardware

### 3. Settings Page
**Status:** Currently just toggles theme

Replace the theme toggle with a full settings system:
- [ ] Create settings UI page with scrollable options
- [ ] Store settings persistently (use minIni library in `third_party/`)
- [ ] Add key settings: brightness, volume, auto-lock, date/time format, network APN
- [ ] Load and apply settings on system boot
- [ ] Add "About" page with device info and credits

### 4. Contact Editing & Deletion
**Status:** UI placeholders exist, no functionality

Complete the contact management features:
- [ ] Implement edit contact page with multitap text input
- [ ] Add contact update function (may need delete + re-insert in B+ tree)
- [ ] Wire up delete contact button with confirmation dialog
- [ ] Fix B+ tree node rebalancing after deletion
- [ ] Handle duplicate names and invalid inputs

---

## High Priority

### 5. B+ Tree Improvements
Current issues documented in `docs/BPTREE_README.md`:
- [ ] Fix node merging/rebalancing after deletion
- [ ] Add case-insensitive search and sorting
- [ ] Prevent or warn on duplicate contact names
- [ ] Add thread safety with FreeRTOS mutexes
- [ ] Support additional fields (email, notes, multiple numbers)

### 6. New Contact Creation
- [ ] Add "New Contact" button to contacts page
- [ ] Create contact entry UI with multitap keyboard
- [ ] Validate and save to B+ tree on SD card

### 7. SMS Message Persistence
**Status:** Messages lost on reboot

- [ ] Store SMS messages on SD card with FatFs
- [ ] Implement conversation threading by contact
- [ ] Add message deletion and forwarding
- [ ] Show unread message count in menu

### 8. Call History
**Status:** Placeholder file exists

- [ ] Store call logs (incoming, outgoing, missed) with timestamps
- [ ] Display in UI with contact names and call duration
- [ ] Add options to call back or clear logs

---

## Medium Priority

### 9. Security Features
- [ ] PIN/password lock screen
- [ ] SIM PIN handling
- [ ] Auto-lock timeout (configurable in settings)
- [ ] Emergency call bypass

### 10. Calendar Enhancements
- [ ] Event/reminder creation and storage on SD card
- [ ] Event notifications with RTC integration
- [ ] Repeating events support

### 11. Battery & Power Management
- [ ] Battery usage statistics by feature
- [ ] Configurable power saving mode
- [ ] Charging status animations

### 12. Alarm Clock
- [ ] Alarm creation and management UI
- [ ] RTC-based triggering
- [ ] Snooze functionality

---

## Nice to Have

### 13. Additional Features
- [ ] Media player (MP3/WAV playback from SD card)
- [ ] More games (Tetris, Pong) with high score storage
- [ ] Network type indicator (2G/3G/4G) and carrier name
- [ ] Enhanced signal strength visualization

---

## Testing & Documentation

### 14. Testing
- [ ] Unit tests for B+ tree operations
- [ ] SD card error condition testing
- [ ] Memory leak testing
- [ ] Battery life testing
- [ ] Multi-carrier cellular testing

### 15. Documentation
- [ ] Update API docs for new features
- [ ] Create user manual
- [ ] Write troubleshooting guide
- [ ] Document hardware revision differences

---

## Known Issues

1. **B+ Tree** - Deletion doesn't rebalance nodes, allows duplicates, case-sensitive search
2. **Settings** - Currently just a theme toggle
3. **Contacts** - Hardcoded, not loaded from SD card
4. **Memory** - No tracking or leak detection

---

## Getting Started

1. Read `README.md` for build instructions
2. Generate docs: `doxygen Doxyfile`
3. Study `docs/BPTREE_README.md` and `docs/mainpage.dox`
4. Start with settings page or contact deletion
5. Test on actual hardware, not just simulation

---

**Important Notes:**
- Use `memwrap.h` functions for UI page memory allocation
- Use FreeRTOS mutexes for shared resources
- Handle SD card errors (card removed during operation, full card)
- Update Makefile and Doxygen for new files

---

**Last Updated:** October 4, 2025  
**UQ Phone Team Year 1 (2025)**

