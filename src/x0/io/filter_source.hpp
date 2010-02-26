#ifndef sw_x0_io_filter_source_hpp
#define sw_x0_io_filter_source_hpp 1

#include <x0/io/source.hpp>
#include <x0/io/filter.hpp>
#include <x0/io/source_visitor.hpp>

namespace x0 {

//! \addtogroup io
//@{

/** filter source.
 */
class X0_API filter_source :
	public source
{
public:
	explicit filter_source(const source_ptr& source, filter& filter) :
		buffer_(), source_(source), filter_(filter) {}

	virtual buffer_ref pull(buffer& output)
	{
		std::size_t pos = output.size();

		buffer_.clear();

		output.push_back(filter_(source_->pull(buffer_)));

		return output.ref(pos);
	}

	virtual void accept(source_visitor& v)
	{
		v.visit(*this);
	}

protected:
	buffer buffer_;
	source_ptr source_;
	filter& filter_;
};

//@}

} // namespace x0

#endif
