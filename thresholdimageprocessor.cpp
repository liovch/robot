#include "thresholdimageprocessor.h"
#include "camera.h"

// TODO: These shouldn't be in pixels, but in mm?
#define MAX_V_GAP 20 // maximum vertical gap while processing marker pixels
#define MAX_H_GAP 2
#define MIN_WIDTH 2  // minimum accepted marker width
#define MIN_HEIGHT 1 // minimum accepted marker height
#define MAX_DISTANCE 5.0 // maximum accepted distance to marker
// TODO: What about MIN_DISTANCE ?

ThresholdImageProcessor::ThresholdImageProcessor(QObject *parent) :
    ImageProcessor(parent)
{
    Q_ASSERT(!gMarkerParams.isEmpty());
    emit needNextImage();
}

void ThresholdImageProcessor::processImage(const QImage &image)
{
    m_lastImage = image;
    processLastImage();
}

void ThresholdImageProcessor::processLastImage()
{
    QVector<MarkerParams::MarkerId> markerMap = buildMarkerMap(m_lastImage);
    QList<Marker> markers = buildMarkerList(markerMap, m_lastImage.width(), m_lastImage.height());
    emit newMarkers(markers);
    emit newMarkerMap(markerMap, m_lastImage.width(), m_lastImage.height()); // TODO: Probably need a special class for the marker map
    emit needNextImage();
}

QVector<MarkerParams::MarkerId> ThresholdImageProcessor::buildMarkerMap(const QImage &image)
{
    QVector<MarkerParams::MarkerId> markerMap(image.width() * image.height(), MARKER_NULL);

    // TODO: speed optimization
    foreach (QObject *obj, gMarkerParams) {
        MarkerParams *params = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(params);
        QPair<QRgb, QRgb> range = params->range();
        for (int y = 0; y < image.height(); y++)
            for (int x = 0; x < image.width(); x++) {
                QRgb pixel = image.pixel(x, y);
                if ((qRed(pixel) >= qRed(range.first) && qRed(pixel) <= qRed(range.second)) &&
                    (qGreen(pixel) >= qGreen(range.first) && qGreen(pixel) <= qGreen(range.second)) &&
                    (qBlue(pixel) >= qBlue(range.first) && qBlue(pixel) <= qBlue(range.second))) {
                    markerMap[x + y * image.width()] = params->id();
                }
            }
    }

    return markerMap;
}

QList<Marker> ThresholdImageProcessor::buildMarkerList(QVector<MarkerParams::MarkerId> markerMap, int width, int height)
{
    QList<Marker> markerList;

    // markers data for each vertical line:
    // first and last marker pixel above the middle line
    // first and last marker pixel below the middle line
    QVector<int> markersOnLine(width * 4, 0);

    int hGap = 0; // current gap between marker in horizontal direction
    Marker curMarker;
    bool isMarkerInProgress = false;

    // TODO: speed optimization
    for (int i = 0; i < width; i++) {
        // look up
        int vGap = 0; // current gap between marker in vertical direction
        int first = -1; // first marker pixel in current vertical line
        int last = -1;
        for (int j = height / 2; j >= 0; j--) { // TODO: should it be height / 2 - 1 ???
            if (markerMap[i + j * width] != MARKER_NULL) {
                if (first < 0) {
                    first = (height / 2) - j;
                    // TODO: make sure markers don't overlap
                    curMarker.setId(markerMap[i + j * width]);
                }
                last = (height / 2) - j;
                vGap = 0;
            } else {
                vGap++;
                if (vGap >= maxVerticalGap()) break;
            }
        }

        if (first >= 0 && last >= 0) {
            markersOnLine[i + width * 0] = first;
            markersOnLine[i + width * 1] = last;
        }

        // look down
        vGap = 0; first = -1; last = -1;
        for (int j = height / 2; j < height; j++) {
            if (markerMap[i + j * width] != MARKER_NULL) {
                if (first < 0) {
                    first = j - (height / 2);
                    // TODO: make sure markers don't overlap
                    curMarker.setId(markerMap[i + j * width]);
                }
                last = j - (height / 2);
                vGap = 0;
            } else {
                vGap++;
                if (vGap >= maxVerticalGap()) break;
            }
        }

        if (first >= 0 && last >= 0) {
            markersOnLine[i + width * 2] = first;
            markersOnLine[i + width * 3] = last;
        }

        int above = markersOnLine[i + width * 1];
        int below = markersOnLine[i + width * 3];
        int h = above + below;

        if (h <= 0) {
            hGap++;
            if (!isMarkerInProgress) continue;

            if (hGap > maxHorizontalGap()) {
                // finish current marker
                curMarker.setHeight(computeMarkerHeight(markersOnLine.data() + (int)curMarker.offset(), (int)curMarker.width(), width, above, below));
                if (curMarker.width() >= MIN_WIDTH && curMarker.height() >= MIN_HEIGHT) {
                    curMarker.setAbove(above);
                    curMarker.setBelow(below);
                    if (curMarker.distance() <= MAX_DISTANCE) markerList.push_back(curMarker);
                }
                isMarkerInProgress = false;
                hGap = 0;
            }
            continue;
        }

        if (!isMarkerInProgress) {
            // Make sure there's at least max_h_gap on the left side. Do not count first blob if its cut off.
            if (markerList.empty() && hGap < maxHorizontalGap()) continue;

            isMarkerInProgress = true;
            curMarker.setOffset(i);
            curMarker.setWidth(1.0);
        } else {
            curMarker.setWidth(i - curMarker.offset() + 1.0);
        }

        hGap = 0;
    }

    return markerList;
}

qreal ThresholdImageProcessor::computeMarkerHeight(int *markersOnLine, int markerWidth, int lineSize, int &above, int &below)
{
    if (markerWidth <= 0) return 0.0;

    int aboveHeight = 0;
    int belowHeight = 0;
    for (int i = 0; i < markerWidth; i++) {
        // process above and below arrays at the same time
        if (markersOnLine[i + lineSize * 1] > aboveHeight) aboveHeight = markersOnLine[i + lineSize * 1];
        if (markersOnLine[i + lineSize * 3] > belowHeight) belowHeight = markersOnLine[i + lineSize * 3];

        // in case the whole marker line is above or below the middle line
        if (markersOnLine[i + lineSize * 1] == 0 && markersOnLine[i + lineSize * 3] > 0) markersOnLine[i + lineSize * 1] = -markersOnLine[i + lineSize * 2];
        if (markersOnLine[i + lineSize * 3] == 0 && markersOnLine[i + lineSize * 1] > 0) markersOnLine[i + lineSize * 3] = -markersOnLine[i + lineSize * 0];
    }

    leastSquaresFitting(markersOnLine + lineSize * 1, markerWidth, aboveHeight);
    leastSquaresFitting(markersOnLine + lineSize * 3, markerWidth, belowHeight);

    // TODO: We assume it to be an isosceles ([aisosiliz]) trapezoid. So we could probably simplify stuff here.
    qreal sum = 0.0;
    above = 0;
    below = 0;
    for (int i = 0; i < markerWidth; i++) {
      above += markersOnLine[i + lineSize * 1];
      below += markersOnLine[i + lineSize * 3];
      sum += markersOnLine[i + lineSize * 1] + markersOnLine[i + lineSize * 3];
    }

    above /= markerWidth;
    below /= markerWidth;
    return sum / (qreal)markerWidth;
}

void ThresholdImageProcessor::leastSquaresFitting(int *markersOnLine, int width, int height)
{
    qreal sumX = 0.0, sumY = 0.0, sumX2 = 0.0, sumXY = 0.0;
    qreal n = 0.0;
    for (int i = 0; i < width; i++) {
      int y = markersOnLine[i];
      if (y < height / 4) continue; // skip line in it differs by more than max height / 4
      sumX += (qreal)i;
      sumY += (qreal)y;
      sumX2 += (qreal)i * i;
      sumXY += (qreal)i * y;
      n += 1.0;
    }

    qreal d = (sumX2 - n * sumX * sumX);
    if (d == 0.0) return;

    qreal a = (sumY * sumX2 - sumX * sumXY) / d;
    qreal b = (sumXY - n * sumX * sumY) / d;
    for (int i = 0; i < width; i++) markersOnLine[i] = (int)(a + b * i + 0.5); // TODO: avoid aliasing
}

int ThresholdImageProcessor::maxHorizontalGap() const
{
    return MAX_H_GAP;
}

int ThresholdImageProcessor::maxVerticalGap() const
{
    return int(MAX_V_GAP / qreal(2 << gCamera.scale()) + 0.5);
}
