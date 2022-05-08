﻿#pragma once

#include <QtWidgets/QMainWindow>
#include <QSystemTrayIcon>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMetaEnum>
#include <QTimer>

#include "Utils.h"
#include "ui_ClipShareWindow.h"

class QClipboard;

/// <summary>
/// Package
/// </summary>
struct ClipSharePackage
{
    enum ClipSharePackageType{
        ClipSharePackageNull,
    	ClipSharePackageImage,
        ClipSharePackagePlainText,
        ClipSharePackageRichText,
        ClipSharePackageCustom
    };

    ClipSharePackageType type {ClipSharePackageType::ClipSharePackagePlainText };
    QByteArray data;
    QString sender;
    QString receiver;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClipSharePackage, type, data, sender, receiver);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ClipSharePackage::ClipSharePackageType, {
    {ClipSharePackage::ClipSharePackageNull, nullptr},
    {ClipSharePackage::ClipSharePackageImage, "ClipSharePackageImage"},
    {ClipSharePackage::ClipSharePackagePlainText, "ClipSharePackagePlainText"},
    {ClipSharePackage::ClipSharePackageRichText, "ClipSharePackageRichText"},
    {ClipSharePackage::ClipSharePackageCustom, "ClipSharePackageCustom"},
    });

/// <summary>
/// hearbeat
/// </summary>
struct ClipShareHeartbeatPackage
{
    enum
    {
        Heartbeat = 0x73,
        Response = 0x66
    };

	std::uint8_t magic[4]{ 0x63, 0x73, 0x66, 0x80 };
    std::uint32_t command { Heartbeat };

    bool valid() const
    {
        return magic[0] == 0x63 && magic[1] == 0x73 && magic[2] == 0x66
            && magic[3] == 0x80 && (command == Heartbeat || command == Response);
    }

};
constexpr ClipShareHeartbeatPackage ClipShareHeartbeatPackage_Heartbeat{ { 0x63, 0x73, 0x66, 0x80 }, ClipShareHeartbeatPackage::Heartbeat };
constexpr ClipShareHeartbeatPackage ClipShareHeartbeatPackage_Response{ { 0x63, 0x73, 0x66, 0x80 }, ClipShareHeartbeatPackage::Response };


struct ClipShareNeighbor
{
    QString hostname;
};




struct ClipShareConfig
{
    int heartbeatPort{ 41688 };
    int heartbeatInterval{ 20000 };
    QString heartbeatMulticastGroupHost{ "239.99.115.102" };

    int packagePort{ 41690 };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClipShareConfig, heartbeatPort, heartbeatInterval, heartbeatMulticastGroupHost, packagePort);
};


class ClipShareWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClipShareWindow(QWidget *parent = Q_NULLPTR);

public slots:

    void broadcastHeartbeat();
    void handlePackageReceived(const QTcpSocket*, const ClipSharePackage&);

protected:
    ClipShareConfig config{};

    QTcpServer packageReciver{ this };

    QSystemTrayIcon systemTrayIcon{ this };
    QUdpSocket heartbeatBroadcaster{ this };
    QTimer heartbeatTimer{ this };

    static bool isLocalHost(QHostAddress);

private:
    Ui::ClipShareWindow ui{};
};
