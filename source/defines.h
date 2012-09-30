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

// set files location
#ifndef Q_OS_DARWIN

#define AIRPORTS_DB      VATSINATOR_PREFIX "WorldAirports.db"
#define FIRS_DB          VATSINATOR_PREFIX "WorldFirs.db"
#define VATSINATOR_DAT   VATSINATOR_PREFIX "vatsinator.dat"
#define MODELS_DAT       VATSINATOR_PREFIX "models.dat"
#define WORLD_MAP        VATSINATOR_PREFIX "WorldMap.db"
#define PIXMAPS_DIR      VATSINATOR_PREFIX "pixmaps"
#define TRANSLATIONS_DIR VATSINATOR_PREFIX "translations"

#else

#define AIRPORTS_DB      QCoreApplication::applicationDirPath() + "/../Resources/WorldAirports.db"
#define FIRS_DB          QCoreApplication::applicationDirPath() + "/../Resources/WorldFirs.db"
#define VATSINATOR_DAT   QCoreApplication::applicationDirPath() + "/../Resources/vatsinator.dat"
#define MODELS_DAT       QCoreApplication::applicationDirPath() + "/../Resources/models.dat"
#define WORLD_MAP        QCoreApplication::applicationDirPath() + "/../Resources/WorldMap.db"
#define PIXMAPS_DIR      QCoreApplication::applicationDirPath() + "/../Resources/pixmaps"
#define TRANSLATIONS_DIR QCoreApplication::applicationDirPath() + "/../Resources/translations"

#endif // Q_OS_DARWIN

// cache file name
#define CACHE_FILE_NAME "lastdata"

// below/above these values user can't zoom the map
#define ZOOM_MINIMUM    1
#define STEP_MINIMUM    0.8

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
