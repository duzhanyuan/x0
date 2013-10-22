/* <CompressFilter.h>
 *
 * This file is part of the x0 web server project and is released under AGPL-3.
 * http://www.xzero.io/
 *
 * (c) 2009-2013 Christian Parpart <trapni@gmail.com>
 */

#ifndef sw_x0_io_compress_filter_hpp
#define sw_x0_io_compress_filter_hpp 1

#include <x0/io/Filter.h>
#include <x0/Api.h>
#include <x0/sysconfig.h>

#if defined(HAVE_ZLIB_H)
#	include <zlib.h>
#endif

#if defined(HAVE_BZLIB_H)
#	include <bzlib.h>
#endif

namespace x0 {

//! \addtogroup io
//@{

/** simply transforms all letters into upper-case letters (test filter).
 */
class X0_API CompressFilter :
	public Filter
{
public:
	explicit CompressFilter(int level);

	int level() const;

private:
	int level_;
};

// {{{ CompressFilter impl
inline CompressFilter::CompressFilter(int level) :
	Filter(),
	level_(level)
{
	assert(level >= 0 && level <= 9);
}

inline int CompressFilter::level() const
{
	return level_;
}
// }}}

#if defined(HAVE_ZLIB_H)
// {{{ DeflateFilter
/** deflate compression filter.
 */
class X0_API DeflateFilter :
	public CompressFilter
{
protected:
	DeflateFilter(int level, bool gzip);

private:
	void initialize();

public:
	explicit DeflateFilter(int level);
	~DeflateFilter();

	virtual Buffer process(const BufferRef& data);

private:
	z_stream z_;
	bool raw_;
};
// }}}
// {{{ GZipFilter
/** gzip compression filter.
 */
class X0_API GZipFilter :
	public DeflateFilter
{
public:
	GZipFilter(int level);
};

inline GZipFilter::GZipFilter(int level) :
	DeflateFilter(level, false)
{
}
// }}}
#endif

#if defined(HAVE_BZLIB_H)
// {{{ BZip2Filter
class X0_API BZip2Filter :
	public CompressFilter
{
public:
	explicit BZip2Filter(int level);

	virtual Buffer process(const BufferRef& data);

private:
	bz_stream bz_;
};
// }}}
#endif
//@}

} // namespace x0

#endif
