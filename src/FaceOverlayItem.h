#pragma once

#include <QGraphicsItem>

class FaceOverlayItem final : public QGraphicsItem {
public:
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget);
  void setSize(int width, int height);

private:
  int m_width, m_height;
};
