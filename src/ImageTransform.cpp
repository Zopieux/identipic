#include "ImageTransform.h"
#include <QPixmap>

ImageTransform::ImageTransform() : QObject(), m_lock(), m_queue(), m_cond() {}

ImageTransform::~ImageTransform() {}

void ImageTransform::enqueue(const Transform &transform) {
  m_lock.lockForWrite();
  m_queue.append(transform);
  m_cond.wakeAll();
  m_lock.unlock();
}

void ImageTransform::run() {
  while (true) {
    m_lock.lockForRead();
    if (m_queue.isEmpty()) {
      m_cond.wait(&m_lock);
    }
    Magick::Blob blob;
    Transform tr = m_queue.takeFirst();
    if (tr.brightness == 0 && tr.contrast == 0) {
      tr.image.write(&blob);
    } else {
      tr.image.brightnessContrast(tr.brightness, tr.contrast);
      tr.image.write(&blob);
    }
    QPixmap pixmap;
    pixmap.loadFromData(static_cast<const uchar *>(blob.data()),
                        static_cast<uint>(blob.length()));
    m_lock.unlock();
    emit finished(pixmap);
  }
}
