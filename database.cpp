#include <mysql_driver.h>
#include <mysql_connection.h>
#include<cppconn/statement.h>
#include<cppconn/prepared_statement.h>
#include<cppconn/resultset.h>
#include<cppconn/exception.h>
#include<iostream>

using namespace std;


int main() {
    try {
        // Initialize MySQL driver
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;

        driver = sql::mysql::get_mysql_driver_instance();

        // Establish the connection
        con = driver->connect("tcp://127.0.0.1:3306", "root", "Mysql@password3");

        // Set the schema (database)
        con->setSchema("testdb");

        // Create a statement
        sql::PreparedStatement *stmt = con->prepareStatement("select * from test where id =?");
        // Execute a query
        stmt->setInt(1,3);
        sql::ResultSet *res=stmt->executeQuery();

        // Process the result set
        while (res->next()) {
            cout<<res->getInt("id")<<" "<<res->getString("name")<<" "<<res->getString("email")<<endl;
        }

        // Clean up
        con->close();
        delete res;
        delete stmt;
        delete con;
    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what() << std::endl;
        std::cerr << "MySQL error code: " << e.getErrorCode() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return EXIT_FAILURE;
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
