/*
    urlbutton.h
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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


#ifndef URLBUTTON_H
#define URLBUTTON_H

#include <QPushButton>

/*
 * Use this button to open an url in system browser.
 */
class UrlButton : public QPushButton {
  Q_OBJECT
  
public:
  /**
   * Creates a button with given text, url and parent.
   * @param text Text that appears on the button.
   * @param url URL to be opened after the button is clicked.
   * @param parent
   */
  explicit UrlButton(const QString&, const QString&, QWidget* = 0);
  
  /**
   * Creates a button with no url and given text.
   * @param text Text that appears on the button.
   * @param parent
   */
  explicit UrlButton(const QString&, QWidget* = 0);
  
  /**
   * Creates a button with no url and no text.
   * @param parent
   */
  explicit UrlButton(QWidget* = 0);
  
  void setUrl(const QString&);
  
  inline const QString &
  url() const { return __url; }
  
private:
  QString __url;
  
private slots:
  void __openUrl();
  
};

#endif // URLBUTTON_H
