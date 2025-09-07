#ifndef DBCONNECT_H
#define DBCONNECT_H

#include <iostream>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>



class dbConnect 
{
public:


    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;

    sql::Driver *driversino;
    sql::Connection *consino;
    sql::Statement *stmtsino;  
/*
    dbConnect(char *dbLocal[3], char *dbsino[3]);
    ~dbConnect();
    bool searchOrders(void);
*/

private:
};

#endif