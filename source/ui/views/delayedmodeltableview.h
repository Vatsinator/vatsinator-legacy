/*
 * delayedmodeltableview.h
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

#ifndef DELAYEDMODELTABLEVIEW_H
#define DELAYEDMODELTABLEVIEW_H

#include <QTableView>
#include <QString>

class DelayedModelTableView : public QTableView {
  
  /**
   * This view is designed for these models that do not come
   * at once, i. e. have to be fetched from the internet before
   * they can be shown. If the model is null, this view will
   * show "Loading" message in the center of the view.
   */
  
  Q_OBJECT
  
public:
  explicit DelayedModelTableView(QWidget* = nullptr);
  
  void setLoadingText(const QString&);
  void setErrorText(const QString&);
  void setErrorOnNoData(bool);
  
  inline const QString& loadingText() const {
    return __loadingText;
  }
  
  inline const QString& errorText() const {
    return __errorText;
  }
  
  inline bool errorOnNoData() const {
    return __errorOnNoData;
  }

protected:
  void paintEvent(QPaintEvent*) override;

private:
  QString __loadingText;
  QString __errorText;
  bool    __errorOnNoData;
  
};

#endif // DELAYEDMODELTABLEVIEW_H
