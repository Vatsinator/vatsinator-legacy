/*
 * infolabel.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef INFOLABEL_H
#define INFOLABEL_H

#include <QWidget>

/**
 * The InfoLabel widget is used to provide nice and readable "key"
 * and "value" fields in Vatsinator details windows. It consists
 * of two strings - description, which is displayed over the main text
 * and value, which is painted using standard application font.
 */
class InfoLabel : public QWidget {
  Q_OBJECT
  
  Q_PROPERTY(QString description READ description WRITE setDescription)
  Q_PROPERTY(QString value READ value WRITE setValue)
  Q_PROPERTY(QFont descriptionFont READ descriptionFont WRITE setDescriptionFont)

public:
  explicit InfoLabel(QWidget* = 0, Qt::WindowFlags = 0);
  
  void setDescription(const QString&);
  void setValue(const QString&);
  void setDescriptionFont(const QFont&);
  
  virtual QSize sizeHint() const override;
  virtual QSize minimumSizeHint() const override;
  
  inline const QString& description() const { return __description; }
  inline const QString& value() const { return __val; }
  inline const QFont& descriptionFont() const { return __descriptionFont; }

protected:
  virtual void paintEvent (QPaintEvent*) override;

private:
  QString __description;
  QString __val;
  QFont   __descriptionFont;
  
};

#endif // INFOLABEL_H
