#include "SizeListModel.h"

SizeListModel::SizeListModel(QObject *parent)
    : QAbstractListModel(parent), m_data(), m_customSize(nullptr) {
  m_customSize = new Size{trUtf8("Custom"), 1, 1};
}

int SizeListModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_data.size() + 1;
}

QVariant SizeListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  int row = index.row();
  if (row >= rowCount() || index.column() != 0)
    return QVariant();

  const Size *const s = row == m_data.size() ? m_customSize : &m_data.at(row);

  if (role == Qt::DisplayRole)
    return s->name;
  if (role == Qt::UserRole)
    return s->width;
  if (role == Qt::UserRole + 1)
    return s->height;

  return QVariant();
}

void SizeListModel::setSizes(const QList<Size> &data) {
  beginResetModel();
  m_data = data;
  endResetModel();
}

void SizeListModel::setCustom(int width, int height) {
  m_customSize->width = width;
  m_customSize->height = height;
  emit dataChanged(index(m_data.size(), 0), index(m_data.size(), 0));
}
