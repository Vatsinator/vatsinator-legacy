/*
    SettingsManager.h
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QColor>

#include "Singleton.h"

class SettingsWindow;

class SettingsManager : public QObject, public Singleton< SettingsManager > {
	
	Q_OBJECT
	
	enum {
		WHEN_HOVERED = 1,
		AIRPORT_RELATED = 2,
		ALWAYS = 4
	};
	
public:
	struct DisplayLayersPolicy {
		bool	pilots;
		bool	airports;
		bool	firs;
		bool	uirs;
	};
	
	SettingsManager(QObject* = 0);
	
	inline int
	getRefreshRate() { return __refreshRate; }
	
	inline const DisplayLayersPolicy &
	getDisplayLayersPolicy() { return __displayLayers; }
	
	inline bool
	displayPilotsLabelsWhenHovered() { return __pilotsLabelsDisplayPolicy & WHEN_HOVERED; }
	
	inline bool
	displayPilotsLabelsAirportRelated() { return __pilotsLabelsDisplayPolicy & AIRPORT_RELATED; }
	
	inline bool
	displayPilotsLabelsAlways() { return __pilotsLabelsDisplayPolicy & ALWAYS; }
	
	inline const QColor &
	getUnstaffedFirBordersColor() { return __unstaffedFirBordersColor; }
	
	inline const QColor &
	getStaffedFirBordersColor() { return __staffedFirBordersColor; }
	
	inline const QColor &
	getStaffedUirBordersColor() { return __staffedUirBordersColor; }
	
	inline const QColor &
	getApproachCircleColor() { return __approachCircleColor; }
	
	inline const QColor &
	getBackgroundColor() { return __backgroundColor; }
	
signals:
	void settingsRestored();
	
private:
	/* Saves all settings in the system, using QSettings */
	void __saveSettings();
	
	/* Restores settings from the registry/config files */
	void __restoreSettings();
	
	/* Clears all config entries from the registry/config files */
	void __clearEntries();
	
	SettingsWindow *	__mySettingsWindow;
	
	/* Miscallaneous */
	int			__refreshRate;
	DisplayLayersPolicy	__displayLayers;
	unsigned		__pilotsLabelsDisplayPolicy;
	
	/* Various map colors */
	QColor	__unstaffedFirBordersColor;
	QColor	__staffedFirBordersColor;
	QColor	__staffedUirBordersColor;
	QColor	__approachCircleColor;
	QColor	__backgroundColor;
	
private slots:
	void __updateSettings();
	void __restoreDefaults();
	
	
};

#endif // SETTINGSMANAGER_H
