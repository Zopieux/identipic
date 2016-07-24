#pragma once

#include "SizeListModel.h"
#include <QComboBox>
#include <QList>
#include <QObject>
#include <QSpinBox>

class SizeComboBox : public QComboBox {
  Q_OBJECT

public:
  explicit SizeComboBox(QWidget *parent = Q_NULLPTR);
  ~SizeComboBox();
  void setSizes(const QList<Size> &sizes);
  void setSpinBoxes(QSpinBox *width, QSpinBox *height);

signals:
  void sizeChanged(int width, int height);

protected slots:
  void setPreset();
  void setCustom();

private:
  QSpinBox *m_width_sb, *m_height_sb;
};
