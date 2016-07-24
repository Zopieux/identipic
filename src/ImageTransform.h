#pragma once

#include <Magick++/Image.h>
#include <QObject>
#include <QReadWriteLock>
#include <QWaitCondition>

class ImageTransform final : public QObject {
  Q_OBJECT
public:
  struct Transform {
    Magick::Image image;
    int brightness;
    int contrast;

    operator QString() const {
      return QString(QStringLiteral("Transform(%1, %2)"))
          .arg(brightness)
          .arg(contrast);
    }
  };

  ImageTransform();
  ~ImageTransform();

public slots:
  void run();
  void enqueue(const Transform &transform);

signals:
  void finished(const QPixmap &pixmap);

private:
  QReadWriteLock m_lock;
  QList<Transform> m_queue;
  QWaitCondition m_cond;
};
