#ifndef O2_H
#define O2_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QList>
#include <QPair>

#include "o2reply.h"

class O2ReplyServer;
class SimpleCrypt;

/// Simple OAuth2 authenticator.
class O2: public QObject {
    Q_OBJECT

public:
    /// Are we authenticated?
    Q_PROPERTY(bool linked READ linked NOTIFY linkedChanged)
    bool linked();

    /// Authentication token.
    Q_PROPERTY(QString token READ token NOTIFY tokenChanged)
    QString token();

    /// Client ID.
    /// O2 instances with the same (client ID, client secret) share the same "linked" and "token" properties.
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    QString clientId();
    void setClientId(const QString &value);

    /// Client secret.
    /// O2 instances with the same (client ID, client secret) share the same "linked" and "token" properties.
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)
    QString clientSecret();
    void setClientSecret(const QString &value);

    /// Scope of authentication.
    Q_PROPERTY(QString scope READ scope WRITE setScope NOTIFY scopeChanged)
    QString scope();
    void setScope(const QString &value);

    /// Request URL.
    Q_PROPERTY(QString requestUrl READ requestUrl WRITE setRequestUrl NOTIFY requestUrlChanged)
    QString requestUrl();
    void setRequestUrl(const QString &value);

    /// Token request URL.
    Q_PROPERTY(QString tokenUrl READ tokenUrl WRITE setTokenUrl NOTIFY tokenUrlChanged)
    QString tokenUrl();
    void setTokenUrl(const QString &value);

    /// Token refresh URL.
    Q_PROPERTY(QString refreshTokenUrl READ refreshTokenUrl WRITE setRefreshTokenUrl NOTIFY refreshTokenUrlChanged)
    QString refreshTokenUrl();
    void setRefreshTokenUrl(const QString &value);

    /// TCP port number to use in local redirections.
    /// The OAuth 2.0 "redirect_uri" will be set to "http://localhost:<localPort>/".
    /// If localPort is set to 0 (default), O2 will replace it with a free one.
    Q_PROPERTY(int localPort READ localPort WRITE setLocalPort NOTIFY localPortChanged)
    int localPort();
    void setLocalPort(int value);

public:
    /// Request type.
    enum RequestType {
        Get, Post
    };

    /// Constructor.
    /// @param  parent  Parent object.
    explicit O2(QObject *parent = 0);

    /// Destructor.
    virtual ~O2();

    /// Get authentication code.
    QString code();

    /// Get refresh token.
    QString refreshToken();

    /// Get token expiration time (seconds from Epoch).
    int expires();

public slots:
    /// Authenticate.
    Q_INVOKABLE void link();

    /// De-authenticate.
    Q_INVOKABLE void unlink();

    /// Refresh token.
    void refresh();

signals:
    /// Emitted when client needs to open a web browser window, with the given URL.
    void openBrowser(const QUrl &url);

    /// Emitted when client can close the browser window.
    void closeBrowser();

    /// Emitted when authentication/deauthentication succeeded.
    void linkingSucceeded();

    /// Emitted when authentication/deauthentication failed.
    void linkingFailed();

    /// Emitted when a token refresh has been completed or failed.
    void refreshFinished(QNetworkReply::NetworkError error);

    // Property change signals
    void linkedChanged();
    void tokenChanged();
    void clientIdChanged();
    void clientSecretChanged();
    void scopeChanged();
    void requestUrlChanged();
    void tokenUrlChanged();
    void refreshTokenUrlChanged();
    void localPortChanged();

protected slots:
    /// Handle verification response.
    virtual void onVerificationReceived(QMap<QString, QString>);

    /// Handle completion of a token request.
    virtual void onTokenReplyFinished();

    /// Handle failure of a token request.
    virtual void onTokenReplyError(QNetworkReply::NetworkError error);

    /// Handle completion of a refresh request.
    virtual void onRefreshFinished();

    /// Handle failure of a refresh request.
    virtual void onRefreshError(QNetworkReply::NetworkError error);

protected:
    /// Build HTTP request body.
    QByteArray buildRequestBody(const QMap<QString, QString> &parameters);

    /// Set authentication code.
    void setCode(const QString &v);

    /// Set authentication token.
    void setToken(const QString &v);

    /// Set refresh token.
    void setRefreshToken(const QString &v);

    /// Set token expiration time.
    void setExpires(int v);

protected:
    QString clientId_;
    QString clientSecret_;
    QString scope_;
    QUrl requestUrl_;
    QUrl tokenUrl_;
    QUrl refreshTokenUrl_;
    QString redirectUri_;
    QNetworkAccessManager *manager_;
    O2ReplyServer *replyServer_;
    QString code_;
    SimpleCrypt *crypt_;
    O2ReplyList timedReplies_;
    quint16 localPort_;
    QString pid_;
};

#endif // O2_H
