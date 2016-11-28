/*
 * VatsinatorActivity.java
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

package org.eu.vatsinator;

import android.content.Context;
import android.content.res.Resources;

public class VatsinatorActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    /**
     * http://stackoverflow.com/questions/3407256/height-of-status-bar-in-android
     */
    public int getStatusBarHeight()
    {
        Context context = (Context) this;
        Resources resources = context.getResources();
        int resourceId = resources.getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            return resources.getDimensionPixelSize(resourceId);
        } else {
            return 0;
        }
    }

    /**
     * http://stackoverflow.com/questions/20264268/how-to-get-height-and-width-of-navigation-bar-programmatically
     */
    public int getNavigationBarHeight()
    {
        Context context = (Context) this;
        Resources resources = context.getResources();
        int resourceId = resources.getIdentifier("navigation_bar_height", "dimen", "android");
        if (resourceId > 0) {
            return resources.getDimensionPixelSize(resourceId);
        } else {
            return 0;
        }
    }
}
