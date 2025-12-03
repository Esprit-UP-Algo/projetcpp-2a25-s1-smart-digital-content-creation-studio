#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>

class Connection
{
public:
    static Connection* instance();  //acces to the unique instance
    bool createConnect();           //method to create the connection

private:
    Connection();                   //private constructor to prevent external instantiation
    ~Connection();                  //private destructor to control destruction
    Connection(const Connection&)=delete;  //delete the copy constructor
    Connection& operator=(const Connection&)=delete;  //delete the assignement operator

    static Connection* p_instance;   //pointer to the unique instance
};

#endif // CONNECTION_H
