/* <x0/sql/SqlResult.h>
 *
 * This file is part of the x0 web server project and is released under LGPL-3.
 * http://www.xzero.ws/
 *
 * (c) 2010-2011 Christian Parpart <trapni@gentoo.org>
 */

#ifndef sw_x0_sql_SqlResult_h
#define sw_x0_sql_SqlResult_h

#include <string>
#include <vector>
#include <mysql/mysql.h>

namespace x0 {

class SqlConnection;

class SqlResult
{
private:
	MYSQL *handle_;
	MYSQL_RES *result_;
	unsigned currentRow_;
	unsigned numRows_;
	std::vector<MYSQL_FIELD *> fields_;
	std::vector<std::string> data_;

	int errorCode_;
	std::string errorText_;

	explicit SqlResult(MYSQL *h);
	friend class SqlConnection;

public:
	SqlResult();
	SqlResult(SqlResult&& r);
	~SqlResult();

	bool isError() const;

	unsigned currentRow() const;
	unsigned numRows() const;
	unsigned numFields() const;

	bool fetch();

	const char *nameAt(int index) const;
	const std::string& valueOf(const char *name) const;
	const std::string& valueAt(int index) const;

	const std::string& operator[](int index) const;
	const std::string& operator[](const char *name) const;
	operator bool() const;
	SqlResult& operator++();
};

} // namespace x0

#endif
