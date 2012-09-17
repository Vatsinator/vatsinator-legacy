/**
 * defines.h
 *
 * This file contains some const variables.
 */

#ifndef DEFINES_H
#define DEFINES_H

#include "config.h"

// where to get all the data from
#define VATSIM_STATUS_URL "http://status.vatsim.net/status.txt"
// if on mingw there is no PREFIX
#ifdef PREFIX
#define AIRPORTS_DB      PREFIX "/share/vatsinator/WorldAirports.db"
#define FIRS_DB          PREFIX "/share/vatsinator/WorldFirs.db"
#define VATSINATOR_DAT   PREFIX "/share/vatsinator/vatsinator.dat"
#define MODELS_DAT       PREFIX "/share/vatsinator/models.dat"
#define WORLD_MAP        PREFIX "/share/vatsinator/WorldMap.db"
#define PIXMAPS_DIR      PREFIX "/share/vatsinator/pixmaps"
#define TRANSLATIONS_DIR PREFIX "/share/vatsinator/translations"
#else
#define AIRPORTS_DB      "WorldAirports.db"
#define FIRS_DB          "WorldFirs.db"
#define VATSINATOR_DAT   "vatsinator.dat"
#define MODELS_DAT       "models.dat"
#define WORLD_MAP        "WorldMap.db"
#define PIXMAPS_DIR      "pixmaps"
#define TRANSLATIONS_DIR "translations"
#endif

// below/above these values user can't zoom the map
#define ZOOM_MINIMUM    1

// the range below which the mouse is in the object's
#define OBJECT_TO_MOUSE   0.03

// how far from the airport the pilot must be to be recognized as "departing"
// or "arrived"
#define PILOT_TO_AIRPORT  0.1

// how to recognize unavailable metars
#define METAR_NO_AVAIL    "No METAR available"

// for these values the GL's context rendering size reprezents
// (-1.0; 1.0) orthogonal ranges for both x and y axes
#define BASE_SIZE_WIDTH   800
#define BASE_SIZE_HEIGHT  600

#define PILOTS_LABELS_FONT_COLOR    0, 0, 0
#define AIRPORTS_LABELS_FONT_COLOR  250, 250, 250
#define FIRS_LABELS_FONT_COLOR      173, 173, 173

// camera position won't go further
#define RANGE_Y   1.0

// fonts properties
#define PILOT_FONT_PIXEL_SIZE   10
#define PILOT_FONT_WEIGHT       QFont::Normal
#define AIRPORT_FONT_PIXEL_SIZE 8
#define AIRPORT_FONT_WEIGHT     QFont::Bold
#define FIR_FONT_PIXEL_SIZE     10
#define FIR_FONT_WEIGHT         QFont::Bold


// if debug, add MemoryTracker to the buildset
#ifndef NO_DEBUG
# define DEBUG_NEW new(__FILE__, __LINE__)
# include "vdebug/memorytracker.h"
#else
# define DEBUG_NEW new
#endif
#define new DEBUG_NEW

#endif // DEFINES_H
