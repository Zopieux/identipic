#include "FaceScene.h"
#include <QGraphicsSceneWheelEvent>
#include <QGuiApplication>
#include <QPainter>

static const int snap = 90 / 6;

FaceScene::FaceScene(QObject *parent)
    : QGraphicsScene(parent), m_rotateOrigin(), m_rotateBase(0), m_mousePos(),
      m_overlay(nullptr), m_picture(nullptr) {
  m_overlay = new FaceOverlayItem();
  m_overlay->setZValue(1);
  m_overlay->setScale(5);
  addItem(m_overlay);
  m_picture = new QGraphicsPixmapItem();
  m_picture->setZValue(0);
  m_picture->setFlag(QGraphicsItemGroup::ItemIsMovable);
  addItem(m_picture);
}

FaceScene::~FaceScene() {}

static bool shiftPressed() {
  const Qt::KeyboardModifiers &modifiers = QGuiApplication::keyboardModifiers();
  return modifiers.testFlag(Qt::ShiftModifier);
}

static bool ctrlPressed() {
  const Qt::KeyboardModifiers &modifiers = QGuiApplication::keyboardModifiers();
  return modifiers.testFlag(Qt::ControlModifier);
}

void FaceScene::setPixmap(const QPixmap &pixmap) {
  m_picture->setPixmap(pixmap);
}

void FaceScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent) {
  qreal delta = .1 * (wheelEvent->delta() > 0 ? 1 : -1);
  if (ctrlPressed())
    delta /= 10;
  else if (shiftPressed())
    delta *= 2;
  m_picture->setScale(m_picture->scale() * (1 + delta));
}

void FaceScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
  QGraphicsScene::mouseMoveEvent(mouseEvent);
  m_mousePos = mouseEvent->scenePos();
  if (!m_rotateOrigin.isNull()) {
    const QLineF &line = QLineF(m_rotateOrigin, m_mousePos);
    qreal dist = line.dy();
    if (ctrlPressed())
      dist /= 10;
    else if (shiftPressed())
      dist = -m_rotateBase + qIntCast(dist / snap) * snap;
    m_picture->setRotation(m_rotateBase + dist);
  }
}

void FaceScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
  QGraphicsScene::mousePressEvent(mouseEvent);
  if (mouseEvent->button() == Qt::RightButton) {
    m_rotateOrigin = mouseEvent->scenePos();
    m_rotateBase = m_picture->rotation();
  }
}

void FaceScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
  QGraphicsScene::mouseReleaseEvent(mouseEvent);
  if (mouseEvent->button() == Qt::RightButton) {
    m_rotateOrigin = QPointF();
  }
}

void FaceScene::resetPixmap() {
  m_picture->setPos(0, 0);
  m_picture->setRotation(0);
  m_picture->setScale(1);
}

QPixmap FaceScene::preview(qreal dpi) {
  const qreal os = m_overlay->scale();
  const qreal scale = m_picture->scale() / os * dpi;
  const QPoint &p = m_picture->pos().toPoint() / os * dpi;
  QTransform matrix;
  matrix.scale(scale, scale);
  matrix.rotate(m_picture->rotation());
  // TODO: size from spinbox
  QPixmap pixmap =
      m_picture->pixmap()
          .transformed(matrix, Qt::FastTransformation)
          .copy(-p.x(), -p.y(), m_overlay->boundingRect().width() * dpi,
                m_overlay->boundingRect().height() * dpi);
  QPixmap result(m_overlay->boundingRect().width() * dpi,
                 m_overlay->boundingRect().height() * dpi);
  QPainter painter(&result);
  painter.fillRect(result.rect(), Qt::white);
  painter.drawPixmap(p.x() > 0 ? p.x() : 0, p.y() > 0 ? p.y() : 0, pixmap);
  return result;
}

QPixmap FaceScene::render(qreal dpi) {
  const qreal os = m_overlay->scale();
  const qreal scale = m_picture->scale() / os * dpi;
  const QPoint &p = m_picture->pos().toPoint() / os * dpi;
  QTransform matrix;
  matrix.scale(scale, scale);
  matrix.rotate(m_picture->rotation());
  // TODO: size from spinbox
  QPixmap pixmap =
      m_picture->pixmap()
          .transformed(matrix, Qt::SmoothTransformation)
          .copy(-p.x(), -p.y(), m_overlay->boundingRect().width() * dpi,
                m_overlay->boundingRect().height() * dpi);
  QPixmap result(m_overlay->boundingRect().width() * dpi,
                 m_overlay->boundingRect().height() * dpi);
  QPainter painter(&result);
  painter.fillRect(result.rect(), Qt::white);
  painter.drawPixmap(p.x() > 0 ? p.x() : 0, p.y() > 0 ? p.y() : 0, pixmap);
  return result;
}

void FaceScene::setOverlaySize(int width, int height) {
  m_overlay->setSize(width, height);
}
