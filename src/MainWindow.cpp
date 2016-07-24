#include "MainWindow.h"
#include "SizeListModel.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QStringListModel>
#include <QThread>
#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow),
      m_workImage(std::make_unique<Magick::Image>()),
      m_originalImage(std::make_unique<Magick::Image>()), m_scene(nullptr),
      m_transform(nullptr), m_newImage(true), m_lastLoadLocation(),
      m_lastSaveLocation() {
  m_ui->setupUi(this);

  m_ui->imageView->setMouseTracking(true);
  m_ui->imageView->setRenderHints(QPainter::Antialiasing |
                                  QPainter::HighQualityAntialiasing |
                                  QPainter::SmoothPixmapTransform);
  m_ui->imageView->setSceneRect(QRectF(QPointF(0, 0), m_ui->imageView->size()));
  m_scene = new FaceScene();
  m_ui->imageView->setScene(m_scene);

  m_ui->previewView->setScene(new QGraphicsScene);

  m_ui->outputFileBrowse->setDefaultAction(m_ui->actionOutputFileBrowse);
  m_ui->outputFileOpen->setDefaultAction(m_ui->actionOutputFileOpen);

  QList<Size> sizes({
      {trUtf8("Standard 13×9"), 130, 90},
      {trUtf8("Standard 15×10"), 150, 100},
      {trUtf8("Standard 18×13"), 180, 130},
      {trUtf8("Standard 21×15"), 210, 150},
      {trUtf8("Standard 30×20"), 300, 200},
      {trUtf8("Digital 13×10"), 130, 100},
      {trUtf8("Digital 15×11"), 150, 110},
      {trUtf8("Digital 17×13"), 170, 130},
      {trUtf8("Digital 20×15"), 200, 150},
      {trUtf8("Digital 27×20"), 270, 200},
      {trUtf8("Square 13×13"), 130, 130},
      {trUtf8("Square 20×20"), 200, 200},
  });
  m_ui->sizeCombo->setSizes(sizes);
  m_ui->sizeCombo->setSpinBoxes(m_ui->outputWidth, m_ui->outputHeight);

  QList<Size> picSizes({
      {trUtf8("Australia"), 35, 45},
      {trUtf8("Belgium"), 35, 45},
      {trUtf8("Brazil ID/passport"), 50, 70},
      {trUtf8("Brazil Visa"), 51, 51},
      {trUtf8("France"), 35, 45},
      {trUtf8("Vietnam"), 40, 60},
  });
  m_ui->picSizeCombo->setSizes(picSizes);
  m_ui->picSizeCombo->setSpinBoxes(m_ui->picWidth, m_ui->picHeight);

  m_ui->autoOutputRows->setChecked(true);
  m_ui->autoOutputCols->setChecked(true);
  m_ui->autoOutputMargin->setChecked(true);

  connect(m_ui->autoOutputRows, &QCheckBox::toggled, this,
          &MainWindow::updateAutoRowsCols);
  connect(m_ui->autoOutputCols, &QCheckBox::toggled, this,
          &MainWindow::updateAutoRowsCols);
  connect(m_ui->autoOutputMargin, &QCheckBox::toggled, this,
          &MainWindow::updateAutoRowsCols);

  connect(m_ui->outputMargin,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
          &MainWindow::updateAutoRowsCols);

  connect(m_ui->sizeCombo, &SizeComboBox::sizeChanged, this,
          &MainWindow::updateOutputSize);
  connect(m_ui->picSizeCombo, &SizeComboBox::sizeChanged, this,
          &MainWindow::updatePicSize);
  m_ui->sizeCombo->setCurrentIndex(1);
  m_ui->picSizeCombo->setCurrentIndex(4);

  connect(m_ui->outputCols,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
          &MainWindow::updateAutoRowsCols);
  connect(m_ui->outputRows,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
          &MainWindow::updateAutoRowsCols);

  connect(m_ui->imageView->scene(), &QGraphicsScene::changed, this,
          &MainWindow::redrawPreview);

  connect(m_ui->brightnessSlider, &QSlider::valueChanged, this,
          &MainWindow::adjustmentsUpdated);
  connect(m_ui->brightnessSlider, &QSlider::sliderReleased, this,
          &MainWindow::adjustmentsUpdated);
  connect(m_ui->contrastSlider, &QSlider::valueChanged, this,
          &MainWindow::adjustmentsUpdated);
  connect(m_ui->contrastSlider, &QSlider::sliderReleased, this,
          &MainWindow::adjustmentsUpdated);

  connect(m_ui->actionOpen, &QAction::triggered, this,
          &MainWindow::pickPictureFile);
  connect(m_ui->actionBuild, &QAction::triggered, this,
          &MainWindow::buildFinal);
  connect(m_ui->actionOutputFileBrowse, &QAction::triggered, this,
          &MainWindow::outputFileBrowse);
  connect(m_ui->actionOutputFileOpen, &QAction::triggered, this,
          &MainWindow::outputFileOpen);
  connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

  Q_ASSERT(!m_originalImage->isValid());
  Q_ASSERT(!m_workImage->isValid());

  QThread *thread = new QThread;
  m_transform = new ImageTransform();
  m_transform->moveToThread(thread);
  connect(thread, &QThread::started, m_transform, &ImageTransform::run);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);
  connect(m_transform, &ImageTransform::finished, this,
          &MainWindow::imageTransformDone);
  connect(this, &MainWindow::destroyed, thread, &QThread::terminate);
  thread->start();

  m_ui->splitter->setStretchFactor(0, 4);
  m_ui->splitter->setStretchFactor(1, 1);

  QSettings settings;
  settings.beginGroup("window");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("state").toByteArray());
  settings.endGroup();
  settings.beginGroup("last");
  const QString &home =
      QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  m_lastLoadLocation = settings.value("load", home).toString();
  m_lastSaveLocation = settings.value("save", home).toString();
  settings.endGroup();

  m_ui->outputFile->setText(QDir(m_lastSaveLocation).filePath("identipic.jpg"));

  setUiEnabled(false);
  adjustmentsUpdated();
}

MainWindow::~MainWindow() {
  m_originalImage.reset();
  m_workImage.reset();
}

void MainWindow::adjustmentsUpdated() {
  int brightness = m_ui->brightnessSlider->value();
  int contrast = m_ui->contrastSlider->value();
  m_ui->brightnessValue->setText(QString(QStringLiteral("%1")).arg(brightness));
  m_ui->contrastValue->setText(QString(QStringLiteral("%1")).arg(contrast));

  if (!m_workImage->isValid())
    return;

  ImageTransform::Transform transform{
      .image = Magick::Image(*m_workImage),
      .brightness = brightness,
      .contrast = contrast,
  };
  m_transform->enqueue(transform);
}

void MainWindow::setUiEnabled(bool enabled) {
  m_ui->actionBuild->setEnabled(enabled);
  m_ui->actionOutputFileBrowse->setEnabled(enabled);
  m_ui->actionOutputFileOpen->setEnabled(enabled);
  m_ui->centralwidget->setEnabled(enabled);
}

void MainWindow::pickPictureFile() {
  const QString &fileName = QFileDialog::getOpenFileName(
      this, trUtf8("Pick face picture"), m_lastLoadLocation,
      QStringLiteral("Image (*.png *.jpg *.jpeg)"));
  if (fileName.isEmpty())
    return;
  openPicture(fileName);
}

void MainWindow::outputFileBrowse() {
  QString location = m_ui->outputFile->text();
  if (location.isEmpty())
    location = m_lastSaveLocation;
  const QString &fileName =
      QFileDialog::getSaveFileName(this, trUtf8("Pick output file"), location,
                                   QStringLiteral("Image (*.jpg *.jpeg)"));
  if (fileName.isEmpty())
    return;
  const QFileInfo &info(fileName);
  m_ui->outputFile->setText(info.absoluteFilePath());
}

void MainWindow::outputFileOpen() {
  const QFileInfo &info(m_ui->outputFile->text());
  if (!info.exists())
    return;
  QDesktopServices::openUrl(
      QUrl(QStringLiteral("file://") + info.filePath(), QUrl::TolerantMode));
}

void MainWindow::openPicture(QString fileName) {
  setUiEnabled(false);
  m_newImage = true;
  try {
    m_originalImage->read(fileName.toStdString());
    m_workImage.reset(new Magick::Image(*m_originalImage));
    m_workImage->resize(Magick::Geometry("512x512>"));
    m_workImage->quality(20);
  } catch (Magick::Exception &error) {
    m_originalImage->isValid(false);
    m_workImage->isValid(false);
    QMessageBox::warning(
        this, trUtf8("Error"),
        trUtf8("<p>There was an error opening this file:</p>"
               "<p><pre>%1</pre></p>")
            .arg(QString::fromStdString(error.what()).toHtmlEscaped()));
    return;
  }
  m_lastLoadLocation = QFileInfo(fileName).absolutePath();
  m_ui->statusbar->showMessage(trUtf8("Loaded image %1").arg(fileName));
  adjustmentsUpdated();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  QSettings settings;
  settings.beginGroup("window");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("state", saveState());
  settings.endGroup();
  settings.beginGroup("last");
  settings.setValue("load", m_lastLoadLocation);
  settings.setValue("save", m_lastSaveLocation);
  settings.endGroup();
  QMainWindow::closeEvent(event);
}

void MainWindow::imageTransformDone(const QPixmap &pixmap) {
  m_scene->setPixmap(pixmap);
  if (m_newImage) {
    m_newImage = false;
    m_ui->brightnessSlider->setValue(0);
    m_ui->contrastSlider->setValue(0);
    m_scene->resetPixmap();
    setUiEnabled(true);
  }
  redrawPreview();
}

void MainWindow::updateOutputSize() {
  updateAutoRowsCols();
  redrawPreview();
}

void MainWindow::updatePicSize(int width, int height) {
  updateAutoRowsCols();
  m_scene->setOverlaySize(width, height);
  redrawPreview();
}

void MainWindow::updateAutoRowsCols() {
  const int margin =
      m_ui->autoOutputMargin->isChecked() ? 0 : m_ui->outputMargin->value();
  int maxCols = m_ui->outputWidth->value() / (m_ui->picWidth->value() + margin);
  if (maxCols == 0)
    maxCols = 1;
  m_ui->outputCols->setMaximum(maxCols);
  if (m_ui->autoOutputCols->isChecked()) {
    m_ui->outputCols->setValue(maxCols);
  }
  int maxRows =
      m_ui->outputHeight->value() / (m_ui->picHeight->value() + margin);
  if (maxRows == 0)
    maxRows = 1;
  m_ui->outputRows->setMaximum(maxRows);
  if (m_ui->autoOutputRows->isChecked()) {
    m_ui->outputRows->setValue(maxRows);
  }
  redrawPreview();
}

void MainWindow::redrawPreview() {
  const int picWidth = m_ui->picWidth->value() * m_previewDpi,
            picHeight = m_ui->picHeight->value() * m_previewDpi;
  const int width = m_ui->outputWidth->value() * m_previewDpi,
            height = m_ui->outputHeight->value() * m_previewDpi;
  const int cols = m_ui->outputCols->value(), rows = m_ui->outputRows->value();
  const int margin = outputMargin() * m_previewDpi;
  const QPixmap &thumb = m_scene->preview(m_previewDpi);
  QSize finalSize(width, height);
  QPixmap pixmap(finalSize);
  QPainter painter(&pixmap);
  painter.save();
  painter.setBrush(Qt::NoBrush);
  painter.setPen(Qt::NoPen);
  painter.fillRect(pixmap.rect(), Qt::darkGray);
  const QSize thumbSize(picWidth, picHeight);
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      const QPoint &p =
          QPoint(col * (margin + picWidth), row * (margin + picHeight));
      painter.drawRect(QRect(p, thumbSize));
      painter.drawPixmap(QRect(p, thumbSize), thumb);
    }
  }
  painter.restore();
  QGraphicsScene *scene = m_ui->previewView->scene();
  scene->clear();
  scene->addPixmap(pixmap);
  m_ui->previewView->fitInView(pixmap.rect(), Qt::KeepAspectRatio);
}

void MainWindow::buildFinal() {
  QString path = m_ui->outputFile->text().trimmed();
  if (path.isEmpty() || !QFileInfo(path).dir().isReadable())
    outputFileBrowse();
  path = m_ui->outputFile->text().trimmed();
  if (path.isEmpty() || !QFileInfo(path).dir().isReadable())
    return;

  path = QFileInfo(path).absoluteFilePath();

  m_ui->statusbar->showMessage(trUtf8("Generating output picture…"));
  qreal dpi = m_ui->outputDpi->value();
  const int cols = m_ui->outputCols->value(), rows = m_ui->outputRows->value();
  const int thumbWidth = m_ui->picWidth->value() * dpi,
            thumbHeight = m_ui->picHeight->value() * dpi;
  const int width = m_ui->outputWidth->value() * dpi,
            height = m_ui->outputHeight->value() * dpi;
  const int margin = outputMargin() * dpi;

  const qreal workScale =
      ((qreal)m_workImage->size().width()) / m_originalImage->size().width();
  const qreal scale = workScale * m_scene->scale() * dpi;

  // original geo
  const Magick::Geometry &picImageGeo = m_originalImage->size();
  // thumb geo
  Magick::Geometry thumbGeo(thumbWidth, thumbHeight);

  // copy original
  Magick::Image picImage(*m_originalImage);
  // resize to scale
  picImage.resize(Magick::Geometry(picImageGeo.width() * scale,
                                   picImageGeo.height() * scale));
  // crop
  const QPoint &offset = -m_scene->offset() * dpi;
  picImage.crop(Magick::Geometry(thumbGeo.width(), thumbGeo.height(),
                                 offset.x(), offset.y()));
  // cropped geo
  const Magick::Geometry &picImageGeoCropped = picImage.size();

  // apply brightness/contrast
  if (m_ui->brightnessSlider->value() != 0 ||
      m_ui->contrastSlider->value() != 0)
    picImage.brightnessContrast(m_ui->brightnessSlider->value(),
                                m_ui->contrastSlider->value());

  // restrict thumb geo for pasting
  thumbGeo.width(qMin(thumbGeo.width(), picImageGeoCropped.width()));
  thumbGeo.height(qMin(thumbGeo.height(), picImageGeoCropped.height()));
  int xOff = qMax(0, -offset.x()), yOff = qMax(0, -offset.y());

  // final white-bg image
  Magick::Image image(Magick::Geometry(width, height), "white");
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      image.copyPixels(picImage, thumbGeo,
                       Magick::Offset(col * (margin + thumbWidth) + xOff,
                                      row * (margin + thumbHeight) + yOff));
    }
  }
  image.quality(100);
  try {
    image.write(path.toStdString());
    m_ui->statusbar->showMessage(trUtf8("Wrote output file %1").arg(path));
    m_lastSaveLocation = QFileInfo(path).absolutePath();
  } catch (Magick::Exception &error) {
    m_ui->statusbar->showMessage(trUtf8("Could not write output file"));
    QMessageBox::warning(
        this, trUtf8("Error saving output file"),
        trUtf8(
            "<p>There was an error writing the output file:</p><pre>%1</pre>")
            .arg(QString::fromStdString(error.what()).toHtmlEscaped()));
  }
}
