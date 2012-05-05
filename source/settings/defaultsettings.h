/* This file contains default Vatsinator settings */

#include <QColor>

namespace DefaultSettings {

// default refresh rate (in minutes)
static const int REFRESH_RATE			= 3;

// antyaliasing option
static const bool ANTYALIASING			= false;

/* some checkboxes state */
// "Display layers"
static const bool PILOTS_CHECKBOX		= true;
static const bool AIRPORTS_CHECKBOX		= true;
static const bool FIRS_CHECKBOX			= true;
static const bool UIRS_CHECKBOX			= true;
// "Show pilots labels"
static const bool ALWAYS_CHECKBOX		= false;
static const bool WHEN_HOVERED_CHECKBOX		= true;
static const bool AIRPORT_RELATED_CHECKBOX	= true;

// default unstaffed FIRs borders color
static const QColor UNSTAFFED_FIR_BORDERS_COLOR		= { 193, 193, 193 };

// default staffed FIRs border color
static const QColor STAFFED_FIR_BORDERS_COLOR		= { 176, 32, 32 };

// default staffed UIRs (i.e. Eurocontrol) border color
static const QColor STAFFED_UIR_BORDERS_COLOR		= { 128, 128, 0 };

// default approach circle color (*_APP)
static const QColor APPROACH_CIRCLE_COLOR		= { 127, 0, 0 };

// default seas color
static const QColor SEAS_COLOR				= { 207, 244, 248 };

// default lands color
static const QColor LANDS_COLOR				= { 255, 255, 255 };

} // namespace DefaultSettings

