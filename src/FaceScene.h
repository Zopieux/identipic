#pragma once

#include "FaceOverlayItem.h"
#include <QGraphicsScene>
#include <QPixmap>

class FaceScene final : public QGraphicsScene {
  Q_OBJECT

public:
  explicit FaceScene(QObject *parent = nullptr);
  ~FaceScene();
  void setPixmap(const QPixmap &pixmap);
  void resetPixmap();
  QPixmap preview(qreal dpi);
  QPixmap render(qreal dpi);
  inline QPoint offset() const {
    return m_picture->pos().toPoint() / m_overlay->scale();
  }
  inline qreal scale() const { return m_picture->scale() / m_overlay->scale(); }

public slots:
  void setOverlaySize(int width, int height);

private:
  QPointF m_rotateOrigin;
  qreal m_rotateBase;
  QPointF m_mousePos;
  FaceOverlayItem *m_overlay;
  QGraphicsPixmapItem *m_picture;

  void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
  void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
};
