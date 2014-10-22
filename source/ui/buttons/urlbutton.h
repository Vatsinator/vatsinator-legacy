/*
    urlbutton.h
    Copyright (C) 2013-2014  Michał Garapich michal@garapich.pl

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
#include <QUrl>

/**
 * The UrlButton class is used to open external links by pressing the button.
 */
class UrlButton : public QPushButton {
  Q_OBJECT
  
  /**
   * Describes the URL that will be opened in external browser when user
   * presses the button.
   */
  Q_PROPERTY(QUrl url READ url WRITE setUrl)
  
public:
  /**
   * Creates a button with given _text_, _url_ and _parent_.
   * 
   * \param text Text that appears on the button.
   * \param url URL to be opened after the button is clicked.
   * \param parent Passed to QPushButton's constructor.
   */
  explicit UrlButton(const QString& text, const QUrl& url, QWidget* parent = nullptr);
  
  /**
   * Creates a button with empty _url_ and given _text_.
   * 
   * \param text Text that appears on the button.
   * \param parent Passed to QPushButton's constructor.
   */
  explicit UrlButton(const QString& text, QWidget* parent = nullptr);
  
  /**
   * Creates a button with empty _url_ and empty _text_.
   * 
   * \param parent Passed to QPushButton's constructor.
   */
  explicit UrlButton(QWidget* parent = nullptr);
  
  void setUrl(const QUrl& url);
  inline const QUrl& url() const { return __url; }
  
private:
  QUrl __url;
  
private slots:
  void __openUrl();
  
};

#endif // URLBUTTON_H
