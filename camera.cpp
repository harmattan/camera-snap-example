#include "camera.h"

Camera::Camera(QObject *parent) :
    QObject(parent)
{
}

void Camera::disconnectSignals()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.disconnect(CAMERA_SERVICE, "/", CAMERA_INTERFACE,
                   "captureCanceled", this, SLOT(captureCanceled(QString)));

    bus.disconnect(CAMERA_SERVICE, "/", CAMERA_INTERFACE,
                   "captureCompleted", this, SLOT(captureCompleted(QString,QString)));
}

void Camera::captureCanceled(const QString &mode)
{
    Q_UNUSED(mode);
    disconnectSignals();
}

void Camera::captureCompleted(const QString &mode, const QString &fileName)
{
    disconnectSignals();

    emit captured(mode, fileName);
}

void Camera::startCamera(Camera::CaptureMode mode)
{
    QString modeString = STILL_MODE;

    switch (mode) {
    case Camera::CameraStillShot:
        modeString = STILL_MODE;
        break;
    case Camera::CameraVideo:
        modeString = VIDEO_MODE;
        break;
    default:
        break;
    }

    QDBusConnection bus = QDBusConnection::sessionBus();

    bus.connect(CAMERA_SERVICE, "/", CAMERA_INTERFACE,
                "captureCanceled", this, SLOT(captureCanceled(QString)));

    bus.connect(CAMERA_SERVICE, "/", CAMERA_INTERFACE,
                "captureCompleted", this, SLOT(captureCompleted(QString,QString)));

    QDBusMessage message = QDBusMessage::createMethodCall(CAMERA_SERVICE, "/",
                CAMERA_INTERFACE, "showCamera");

    QList<QVariant> arguments;
    uint someVar = 0;
    arguments << someVar << "" << modeString << true;
    message.setArguments(arguments);

    QDBusMessage reply = bus.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        disconnectSignals();
    }
}
