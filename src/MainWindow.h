#pragma once

#include "FaceScene.h"
#include "ImageTransform.h"
#include <Magick++/Image.h>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QMutex>
#include <memory>
#include <ui_MainWindow.h>

namespace Ui {
class MainWindow;
}

class MainWindow final : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void setUiEnabled(bool);
  void adjustmentsUpdated();
  void imageTransformDone(const QPixmap &pixmap);
  void updateOutputSize();
  void updatePicSize(int width, int height);
  void updateAutoRowsCols();
  void redrawPreview();
  void buildFinal();
  void pickPictureFile();
  void outputFileBrowse();
  void outputFileOpen();
  void openPicture(QString fileName);

private:
  static const int m_previewDpi = 5;

  Ui::MainWindow *m_ui;
  std::unique_ptr<Magick::Image> m_workImage, m_originalImage;
  FaceScene *m_scene;
  ImageTransform *m_transform;
  bool m_newImage;
  QString m_lastLoadLocation, m_lastSaveLocation;

  inline int outputMargin() const {
    if (!m_ui->autoOutputMargin->isChecked())
      return m_ui->outputMargin->value();
    const int rows = m_ui->outputRows->value(),
              cols = m_ui->outputCols->value();
    if (rows == 1 && cols == 1)
      return 0;
    const int picWidth = m_ui->picWidth->value(),
              picHeight = m_ui->picHeight->value();
    const int width = m_ui->outputWidth->value(),
              height = m_ui->outputHeight->value();
    const int x = cols > 1
                      ? qIntCast((((qreal)width) - cols * picWidth)) /
                            (cols - 1)
                      : INT_MAX,
              y = rows > 1
                      ? qIntCast((((qreal)height) - rows * picHeight)) /
                            (rows - 1)
                      : INT_MAX;
    return qMin(x, y);
  }
  void closeEvent(QCloseEvent *event);
};
