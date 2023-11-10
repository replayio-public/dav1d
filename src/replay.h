#pragma once

#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include <stdarg.h>
#include <stdbool.h>

// ##########################################################################
// Driver/Linker API
// ##########################################################################
void RecordReplayPrint(const char* format, ...);
extern void RecordReplayWarning(const char* format, ...);
extern void RecordReplayAssert(const char* format, ...);
extern void RecordReplayDiagnostic(const char* format, ...);
extern void RecordReplayRegisterPointer(const void* ptr);
extern void RecordReplayUnregisterPointer(const void* ptr);
extern bool RecordReplayFeatureEnabled(const char* feature, const char* subfeature);
extern bool RecordReplayIsRecordingOrReplaying(const char* feature, const char* subfeature);
extern bool RecordReplayAreEventsDisallowed(const char* why);
extern void RecordReplayBeginPassThroughEvents();
extern void RecordReplayEndPassThroughEvents();
extern bool RecordReplayIsReplaying(void);
extern unsigned long RecordReplayValue(const char* why, unsigned long v);


static char gRecordingOrReplaying;
static char gHasDisabledFeatures;

static void (*gRecordReplayPrint)(const char* format, va_list args);
static void (*gRecordReplayWarning)(const char* format, va_list args);
static void (*gRecordReplayAssert)(const char*, va_list);
static void (*gRecordReplayDiagnostic)(const char*, va_list);
static void (*gRecordReplayRegisterPointer)(const void* ptr);
static void (*gRecordReplayUnregisterPointer)(const void* ptr);
static int (*gRecordReplayPointerId)(const void* ptr);

static char (*gRecordReplayHasDisabledFeatures)();
static char (*gRecordReplayFeatureEnabled)(const char* feature, const char* subfeature);
static char (*gRecordReplayAreEventsDisallowed)();
static void (*gRecordReplayBeginPassThroughEvents)();
static void (*gRecordReplayEndPassThroughEvents)();
static char (*gRecordReplayIsReplaying)(void);
static unsigned long (*gRecordReplayValue)(const char* why, unsigned long v);

static void RecordReplayLoadSymbol(const char* name, void** function) {
#ifndef _WIN32
  void* sym = dlsym(RTLD_DEFAULT, name);
#else
  HMODULE module = GetModuleHandleA("windows-recordreplay.dll");
  void* sym = module ? (void*)GetProcAddress(module, name) : 0;
#endif
  if (sym) {
    *function = sym;
  }
}

static inline char EnsureInitialized() {
  RecordReplayLoadSymbol("RecordReplayPrint", (void**)&gRecordReplayPrint);
  RecordReplayLoadSymbol("RecordReplayWarning", (void**)&gRecordReplayWarning);
  RecordReplayLoadSymbol("RecordReplayAssert", (void**)&gRecordReplayAssert);
  RecordReplayLoadSymbol("RecordReplayDiagnostic", (void**)&gRecordReplayDiagnostic);
  RecordReplayLoadSymbol("RecordReplayRegisterPointer", (void**)&gRecordReplayRegisterPointer);
  RecordReplayLoadSymbol("RecordReplayUnregisterPointer", (void**)&gRecordReplayUnregisterPointer);
  RecordReplayLoadSymbol("RecordReplayPointerId", (void**)&gRecordReplayPointerId);

  RecordReplayLoadSymbol("RecordReplayHasDisabledFeatures", (void**)&gRecordReplayHasDisabledFeatures);
  RecordReplayLoadSymbol("RecordReplayFeatureEnabled", (void**)&gRecordReplayFeatureEnabled);
  RecordReplayLoadSymbol("RecordReplayAreEventsDisallowed", (void**)&gRecordReplayAreEventsDisallowed);
  RecordReplayLoadSymbol("RecordReplayBeginPassThroughEvents", (void**)&gRecordReplayBeginPassThroughEvents);
  RecordReplayLoadSymbol("RecordReplayEndPassThroughEvents", (void**)&gRecordReplayEndPassThroughEvents);
  RecordReplayLoadSymbol("RecordReplayIsReplaying", (void**)&gRecordReplayIsReplaying);
  RecordReplayLoadSymbol("RecordReplayValue", (void**)&gRecordReplayValue);

  gRecordingOrReplaying =
          gRecordReplayFeatureEnabled && gRecordReplayFeatureEnabled("record-replay", 0);
  gHasDisabledFeatures = gRecordingOrReplaying && gRecordReplayHasDisabledFeatures();

  return !!gRecordReplayAssert;
}

void RecordReplayPrint(const char* format, ...) {
  if (RecordReplayIsRecordingOrReplaying(0, 0)) {
    va_list args;
    va_start(args, format);
    gRecordReplayPrint(format, args);
    va_end(args);
  }
}

void RecordReplayWarning(const char* format, ...) {
  if (EnsureInitialized()) {
    va_list ap;
    va_start(ap, format);
    gRecordReplayWarning(format, ap);
    va_end(ap);
  }
}

void RecordReplayAssert(const char* format, ...) {
  if (EnsureInitialized()) {
    va_list ap;
    va_start(ap, format);
    gRecordReplayAssert(format, ap);
    va_end(ap);
  }
}

void RecordReplayDiagnostic(const char* format, ...) {
  if (EnsureInitialized()) {
    va_list ap;
    va_start(ap, format);
    gRecordReplayDiagnostic(format, ap);
    va_end(ap);
  }
}

 void RecordReplayRegisterPointer(const void* ptr) {
  if (EnsureInitialized()) {
    gRecordReplayRegisterPointer(ptr);
  }
}

 void RecordReplayUnregisterPointer(const void* ptr) {
  if (EnsureInitialized()) {
    gRecordReplayUnregisterPointer(ptr);
  }
}

 bool RecordReplayFeatureEnabled(const char* feature, const char* subfeature) {
  if (EnsureInitialized()) {
    if (!gHasDisabledFeatures) {
        return true;
    }
    return gRecordReplayFeatureEnabled(feature, subfeature);
  }
  return true;
}

 bool RecordReplayIsRecordingOrReplaying(const char* feature, const char* subfeature) {
  return EnsureInitialized() && gRecordingOrReplaying &&
    (!feature || RecordReplayFeatureEnabled(feature, subfeature));
}

 bool RecordReplayAreEventsDisallowed(const char* why) {
  if (EnsureInitialized()) {
    if (RecordReplayIsRecordingOrReplaying("disallow-events", why)) {
      return gRecordReplayAreEventsDisallowed();
    }
  }
  return false;
}

 void RecordReplayBeginPassThroughEvents() {
  if (EnsureInitialized()) {
    gRecordReplayBeginPassThroughEvents();
  }
}

 void RecordReplayEndPassThroughEvents() {
  if (EnsureInitialized()) {
    gRecordReplayEndPassThroughEvents();
  }
}

 bool RecordReplayIsReplaying(void) {
  return EnsureInitialized() && gRecordReplayIsReplaying();
}

 unsigned long RecordReplayValue(const char* why, unsigned long v) {
  if (RecordReplayIsRecordingOrReplaying("values", why)) {
    return gRecordReplayValue(why, v);
  }
  return v;
}

