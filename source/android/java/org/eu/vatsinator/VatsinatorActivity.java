/*
 * VatsinatorActivity.java
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

import android.graphics.Color;
import android.view.Window;

public class VatsinatorActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static VatsinatorActivity instance;

    public VatsinatorActivity()
    {
        instance = this;
    }
    
    public static void setNavigationBarColor(int r, int g, int b, int a)
    {
        final int color = Color.argb(a, r, g, b);
        
        instance.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.LOLLIPOP) {
                    instance.getWindow().setNavigationBarColor(color);
                }
            }
        });
    }
}
