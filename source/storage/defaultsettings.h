/* This file contains default Vatsinator settings */

#include <QColor>

namespace DefaultSettings {

// default refresh rate (in minutes)
static const int REFRESH_RATE               = 3;

// refresh also METARs
static const bool METARS_REFRESH            = true;

// antyaliasing option
static const bool ANTYALIASING              = false;

// cache enabled option
static const bool CACHE_ENABLED             = true;

// zoom coefficient
static const int ZOOM_COEFFICIENT           = 30;

/* some checkboxes state */
// "Display layers"
static const bool PILOTS_CHECKBOX           = true;
static const bool AIRPORTS_CHECKBOX         = true;
static const bool STAFFED_FIRS_CHECKBOX     = true;
static const bool UNSTAFFED_FIRS_CHECKBOX   = true;
static const bool DISPLAY_AIRPORT_LABELS    = true;
static const bool EMPTY_AIRPORTS_CHECKBOX   = false;
// "Show pilots labels"
static const bool ALWAYS_CHECKBOX           = true;
static const bool WHEN_HOVERED_CHECKBOX     = true;
static const bool AIRPORT_RELATED_CHECKBOX  = true;

// default unstaffed FIRs borders color
static const QColor UNSTAFFED_FIR_BORDERS_COLOR     = { 193, 193, 193 };

// default staffed FIRs border color
static const QColor STAFFED_FIR_BORDERS_COLOR       = { 176, 32, 32 };

// default staffed FIRs background color alpha
static const int STAFFED_FIR_BACKGROUND_ALPHA       = 30;

// default staffed UIRs (i.e. Eurocontrol) border color
static const QColor STAFFED_UIR_BORDERS_COLOR       = { 0, 118, 148 };

// default staffed UIRs background color alpha
static const int STAFFED_UIR_BACKGROUND_ALPHA       = 30;

// default approach circle color (*_APP)
static const QColor APPROACH_CIRCLE_COLOR           = { 127, 0, 0 };

// default seas color
static const QColor SEAS_COLOR                      = { 188, 222, 225 };

// default lands color
static const QColor LANDS_COLOR                     = { 255, 255, 255 };

// default origin-to-pilot line color
static const QColor ORIGIN_TO_PILOT_LINE_COLOR      = { 3, 116, 164 };

// default pilot-to-destination line color
static const QColor PILOT_TO_DESTINATION_LINE_COLOR = { 133, 164, 164 };

} // namespace DefaultSettings

