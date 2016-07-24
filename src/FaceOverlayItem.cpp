#include "FaceOverlayItem.h"
#include <QPainter>

QRectF FaceOverlayItem::boundingRect() const {
  return QRectF(0, 0, m_width, m_height);
}

void FaceOverlayItem::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);
  qreal width = boundingRect().width();
  qreal height = boundingRect().height();
  painter->save();
  painter->setPen(QPen(Qt::black, 0));
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(boundingRect());
  painter->setPen(QPen(Qt::black, 0));
  painter->setBrush(QColor::fromHslF(.7, .9, .5, .2));
  painter->drawEllipse(QPointF(width * .5, height * .45), width * .33,
                       height * .378);
  painter->setPen(Qt::NoPen);
  painter->setBrush(QColor::fromHslF(.1, .9, .5, .2));
  painter->drawRect(0, qIntCast(height * 27 / 97), qIntCast(width),
                    qIntCast(height * (49 - 27) / 97));
  painter->restore();
}

void FaceOverlayItem::setSize(int width, int height) {
  prepareGeometryChange();
  m_width = width;
  m_height = height;
}
