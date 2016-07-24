#include "SizeComboBox.h"

SizeComboBox::SizeComboBox(QWidget *parent)
    : QComboBox(parent), m_width_sb(nullptr), m_height_sb(nullptr) {
  setModel(new SizeListModel);
  connect(this, static_cast<void (QComboBox::*)(int)>(
                    &QComboBox::currentIndexChanged),
          this, &SizeComboBox::setPreset);
}

SizeComboBox::~SizeComboBox() {
  disconnect(m_width_sb);
  disconnect(m_height_sb);
}

void SizeComboBox::setSizes(const QList<Size> &sizes) {
  static_cast<SizeListModel *>(model())->setSizes(sizes);
}

void SizeComboBox::setSpinBoxes(QSpinBox *width, QSpinBox *height) {
  m_width_sb = width;
  m_height_sb = height;
  connect(m_width_sb,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
          &SizeComboBox::setCustom);
  connect(m_height_sb,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
          &SizeComboBox::setCustom);
  setPreset();
}

void SizeComboBox::setPreset() {
  if (m_width_sb == nullptr || m_height_sb == nullptr)
    return;
  m_width_sb->blockSignals(true);
  m_height_sb->blockSignals(true);
  m_width_sb->setValue(currentData(Qt::UserRole).toInt());
  m_height_sb->setValue(currentData(Qt::UserRole + 1).toInt());
  m_width_sb->blockSignals(false);
  m_height_sb->blockSignals(false);
  emit sizeChanged(m_width_sb->value(), m_height_sb->value());
}

void SizeComboBox::setCustom() {
  static_cast<SizeListModel *>(model())->setCustom(m_width_sb->value(),
                                                   m_height_sb->value());
  setCurrentIndex(model()->rowCount() - 1);
  emit sizeChanged(m_width_sb->value(), m_height_sb->value());
}
