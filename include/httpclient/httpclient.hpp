#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

/**
 * @file httpclient.h
 * @author Dr. Abiira Nathan (nabiira2by2@gamil.com)
 * @brief
 * @version 0.1
 * @date 2023-05-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <QFile>
#include <QImageReader>  // Requires linking to QtGui
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QUrl>
#include <exception>
#include <string>

/**
 * @brief Custom exception thrown when syncronous network calls fail.
 * The caller must catch this exception to avoid segmentation faults.
 */
class NetworkException : public std::exception {
public:
    /**
     * @brief Construct a new NetworkException object
     *
     * @param statusCode
     * @param message
     */
    NetworkException(int statusCode, const QString& message);

    /**
     * @brief Get the Status Code of the response.
     *
     * @return int
     */
    [[nodiscard]] int getStatusCode() const;

    /**
     * @brief Virtual override for the std::exception.
     *
     * @return const char*
     */
    [[nodiscard]] const char* what() const noexcept override;

private:
    int statusCode;       // response status
    std::string message;  // error message
};

/**
 * @brief HttpClient is a wrapper around the QNetworkAccessManager to simplify
 * performing http requests in Qt.
 *
 * It supports both syncronous and asyncronous methods.
 * All syncronous have a suffix of _sync and throw a NetworkException if the request fails
 * or the status code is > 300.
 *
 * The asyncronous methods use signals success and error to communicate when data is ready
 * or when an error occurs. The error returned is read from the request body as a QByteArray.
 *
 * Each request uses the same QNetworkAccessManager instance but different QNetwork object.
 * This means you can use the same client to perform multiple subsequent requests.
 */
class HttpClient : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a new Http Client object
     *
     * @param parent *QObject
     */
    HttpClient(QObject* parent = nullptr);

    /**
     * @brief Construct a new Http Client object with parent and default http headers.
     * The headers are a QMap of strings and are automatically added to every request.
     *
     * @param parent *QObject
     * @param headers QMap<QString, QString>
     */
    HttpClient(QObject* parent, const QMap<QString, QString>& headers);

    /**
     * @brief Destroy the Http Client object
     *
     */
    ~HttpClient() override;

    /**
     * @brief Set the Root CA object
     *
     * @param certPath QString
     */
    static void setRootCA(const QString& certPath);

    void setGlobalTimeout(std::chrono::milliseconds ms);
    void resetGlobalTimeout();

    /**
     * @brief Set the Bearer Token string. This will be used to Bearer Auth.
     *
     * @param jwtToken QString.
     */
    static void setBearerToken(const QString& jwtToken);

    /**
     * @brief Perform a GET request asyncronously. You will need to access the response by connecting
     * to the success signal and error to error signal.
     *
     * @param url QString
     */
    void get(const QString& url) noexcept;

    /**
     * @brief Perform a HEAD request asyncronously. You will need to access the response by connecting
     * to the success signal and error to error signal.
     *
     * @param url QString
     */
    void head(const QString& url) noexcept;

    /**
     * @brief Perform a POST request asyncronously. You will need to access the response by connecting
     * to the success signal and error to error signal.
     *
     * @param url QString
     * @param data QByteArray
     */
    void post(const QString& url, const QByteArray& data) noexcept;

    /**
     * @brief Perform a PUT request asyncronously. You will need to access the response by connecting
     * to the success signal and error to error signal.
     *
     * @param url QString
     * @param data QByteArray
     */
    void put(const QString& url, const QByteArray& data) noexcept;

    /**
     * @brief Perform a PATCH request asyncronously. You will need to access the response by connecting
     * to the success signal and error to error signal.
     *
     * @param url QString
     * @param data QByteArray
     */
    void patch(const QString& url, const QByteArray& data) noexcept;

    /**
     * @brief Perform a DELETE request asyncronously. You will need to access the response by connecting
     * to the success signal and error to error signal.
     *
     * @param url QString
     */
    void del(const QString& url) noexcept;

    /** Perform syncronous GET request and block until the response arrives
     * Returns data in request body if successful or throws a NetworkException if it fails.
     */
    QByteArray get_sync(const QString& url);

    /** Perform syncronous HEAD request and block until the response arrives
     * Returns data in request body if successful or throws a NetworkException if it fails.
     * You must catch this error to avoid segmentation faults.
     */
    QByteArray head_sync(const QString& url);

    /** Perform syncronous POST request and block until the response arrives
     * Returns data in request body if successful or throws a NetworkException if it fails.
     */
    QByteArray post_sync(const QString& url, const QByteArray& data);

    /** Perform syncronous PUT request and block until the response arrives
     * Returns data in request body if successful or throws a NetworkException if it fails.
     */
    QByteArray put_sync(const QString& url, const QByteArray& data);

    /** Perform syncronous PATCH request and block until the response arrives
     * Returns data in request body if successful or throws a NetworkException if it fails.
     */
    QByteArray patch_sync(const QString& url, const QByteArray& data);

    /** Perform syncronous DELETE request and block until the response arrives
     * Returns data in request body if successful or throws a NetworkException if it fails.
     */
    QByteArray del_sync(const QString& url);

private:
    QNetworkAccessManager* manager;
    QMap<QString, QString> headers;
    void setHeaders(QNetworkRequest* request);

    static QString token;  // The JWT

    // Used by all syncronous method to process reply, read data and return it to caller
    // and is responsible for throwing the NetworkException is the reply failed or status
    // code is > 300.
    QByteArray waitForResponse(QNetworkReply* reply);

signals:
    /**
     * @brief Signal which is emitted when an asyncronous network call has succeded.
     * response data is passed as a parameter to the slot.
     *
     * @param data
     */
    void success(const QByteArray& data);

    /**
     * @brief Signal which is emitted when an asyncronous network call has failed.
     * response error string is passed as a parameter to the slot.
     *
     * @param errorString
     */
    void error(const QString& errorString);

private slots:

    /**
     * @brief Slot for internal use to process network reply.
     *
     */
    void onReplyFinished();
};

/**
 * @brief writeFile writes the data to a file at the given path.
 * @param path QString
 * @param data QByteArray
 */
void writeFile(const QString& path, const QByteArray& data);

/**
 * @brief imageFromBytes converts a byte array to a QImage.
 * @param data QByteArray
 * @return QImage
 */
QImage imageFromBytes(const QByteArray& data);

#endif /* __HTTPCLIENT_H__ */
