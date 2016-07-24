#pragma once

#include <QAbstractListModel>

struct Size {
  QString name;
  int width, height;

  operator QString() const {
    return QString("%1 (%2×%3)").arg(name).arg(width).arg(height);
  }
};

class SizeListModel : public QAbstractListModel {
  Q_OBJECT

public:
  explicit SizeListModel(QObject *parent = 0);
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

public slots:
  void setSizes(const QList<Size> &data);
  void setCustom(int width, int height);

private:
  QList<Size> m_data;
  Size *m_customSize;
};
