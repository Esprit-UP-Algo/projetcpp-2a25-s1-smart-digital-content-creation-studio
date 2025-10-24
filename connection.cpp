#include "connection.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

// Initialization of the instance pointer to nullptr
Connection* Connection::p_instance = nullptr;

// Private constructor
Connection::Connection() {
    // Connection initialization (if necessary)
}

// Static method to obtain the unique instance of the Connection class
Connection* Connection::instance() {
    if (p_instance == nullptr) {
        p_instance = new Connection();
    }
    return p_instance;
}

// Method to establish the database connection
bool Connection::createConnect() {
    bool test = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName("Source_Projet2A");  // Data source name (ODBC DSN)
    db.setUserName("smart_content");               // Username
    db.setPassword("smart_content123");              // Password

    if (db.open()) {
        test = true;
        qDebug() << "Database connected successfully!";
    } else {
        qDebug() << "Database connection failed:" << db.lastError().text();
    }

    return test;
}

// Private destructor to close the database connection
Connection::~Connection() {
    QSqlDatabase db = QSqlDatabase::database();
    if (db.isOpen()) {
        db.close();
        qDebug() << "Database connection closed.";
    }
}
